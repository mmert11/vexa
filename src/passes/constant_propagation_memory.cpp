#include <vexa/passes/constant_propagation.hpp>
#include <vexa/vexa.h>

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include <llvm/ADT/DenseMap.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/AssumptionCache.h>
#include <llvm/Analysis/BasicAliasAnalysis.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/MemorySSA.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/CFG.h>
#include <llvm/TargetParser/Triple.h>

vexa::passes::constant_propagation::memory_propagation_result
    vexa::passes::constant_propagation::concretize_memory_loads(
        llvm::Function *function,
        llvm::DominatorTree &DT,
        llvm::LoopInfo &LI,
        value_simplifier &simplify_once)
{
    const auto &symex = context->symex;
    const auto &builder = context->builder;
    const llvm::DataLayout &DL = function->getParent()->getDataLayout();

    struct memory_address
    {
        const vexa::mem_page *page = nullptr;
        uint64_t offset = 0;

        bool operator==(const memory_address &) const = default;
    };

    struct concrete_memory_access
    {
        memory_address address;
        uint64_t size;
    };

    std::unordered_map<llvm::Instruction *, concrete_memory_access> concrete_accesses;
    std::unordered_set<llvm::Instruction *> attempted_accesses;
    auto get_memory_access = [&](llvm::Instruction *instruction) -> const concrete_memory_access * {
        auto cached = concrete_accesses.find(instruction);
        if (cached != concrete_accesses.end())
            return &cached->second;
        if (!attempted_accesses.insert(instruction).second)
            return nullptr;

        llvm::Value *pointer_operand = nullptr;
        llvm::Type *accessed_type = nullptr;
        if (auto *load = llvm::dyn_cast<llvm::LoadInst>(instruction)) {
            pointer_operand = load->getPointerOperand();
            accessed_type = load->getType();
        }
        else if (auto *store = llvm::dyn_cast<llvm::StoreInst>(instruction)) {
            pointer_operand = store->getPointerOperand();
            accessed_type = store->getValueOperand()->getType();
        }
        else {
            return nullptr;
        }

        if (!symex->is_sync(pointer_operand))
            return nullptr;

        vexa::value *expression = simplify_once(symex->get(pointer_operand));
        auto *pointer = vexa::dyn_cast<vexa::pointer>(expression);
        if (!pointer || !pointer->is_concrete())
            return nullptr;

        auto [it, inserted] = concrete_accesses.emplace(
            instruction,
            concrete_memory_access{
                .address = {.page = pointer->get_page().get(), .offset = pointer->as_uint64()},
                .size = DL.getTypeStoreSize(accessed_type).getFixedValue()});
        return &it->second;
    };

    llvm::DenseMap<llvm::BasicBlock *, unsigned> predecessor_counts;
    for (auto &BB : *function)
        predecessor_counts.try_emplace(&BB, llvm::pred_size(&BB));

    std::vector<llvm::Instruction *> dead_loads;
    std::vector<llvm::Instruction *> fold_seeds;
    llvm::TargetLibraryInfoImpl TLII(llvm::Triple(function->getParent()->getTargetTriple()));
    llvm::TargetLibraryInfo TLI(TLII);
    llvm::AssumptionCache AC(*function);
    llvm::AAResults AA(TLI);
    llvm::BasicAAResult BasicAA(DL, *function, TLI, AC, &DT);
    AA.addAAResult(BasicAA);
    auto MSSA = std::make_unique<llvm::MemorySSA>(*function, &AA, &DT);

    struct memory_address_hash
    {
        size_t operator()(const memory_address &address) const
        {
            size_t hash = std::hash<const vexa::mem_page *>{}(address.page);
            hash ^= std::hash<uint64_t>{}(address.offset) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            return hash;
        }
    };

    // Build the MemorySSA def forest once, then carry a mutable byte-safety
    // map through it with rollback. Unknown definitions start a new, unsafe
    // memory generation; later concrete stores can make overwritten bytes safe
    // again. This keeps unknown writes conservative without making every load
    // walk backwards through the store chain.
    llvm::DenseMap<llvm::MemoryAccess *, std::vector<llvm::MemoryDef *>> def_children;
    llvm::DenseMap<llvm::MemoryDef *, llvm::StoreInst *> def_stores;
    llvm::DenseMap<llvm::MemoryAccess *, std::vector<llvm::LoadInst *>> access_loads;
    llvm::DenseMap<llvm::LoadInst *, llvm::MemoryAccess *> load_accesses;
    std::vector<llvm::MemoryAccess *> roots{MSSA->getLiveOnEntryDef()};
    std::unordered_set<llvm::MemoryAccess *> root_set{MSSA->getLiveOnEntryDef()};

    auto add_root = [&](llvm::MemoryAccess *access) {
        if (llvm::isa<llvm::MemoryPhi>(access) && root_set.insert(access).second)
            roots.push_back(access);
    };

    for (auto &BB : *function) {
        for (auto &I : BB) {
            if (auto *definition =
                    llvm::dyn_cast_or_null<llvm::MemoryDef>(MSSA->getMemoryAccess(&I)))
            {
                llvm::MemoryAccess *parent = definition->getDefiningAccess();
                def_children[parent].push_back(definition);
                if (auto *store = llvm::dyn_cast<llvm::StoreInst>(&I))
                    def_stores.try_emplace(definition, store);
                add_root(parent);
            }

            if (auto *load = llvm::dyn_cast<llvm::LoadInst>(&I)) {
                auto *memory_use =
                    llvm::dyn_cast_or_null<llvm::MemoryUseOrDef>(MSSA->getMemoryAccess(load));
                if (!memory_use)
                    continue;

                llvm::MemoryAccess *parent = memory_use->getDefiningAccess();
                access_loads[parent].push_back(load);
                load_accesses.try_emplace(load, parent);
                add_root(parent);
            }
        }
    }

    enum class safety
    {
        visiting,
        safe,
        unsafe
    };

    struct byte_state
    {
        llvm::MemoryAccess *generation;
        bool safe;
    };

    struct undo_record
    {
        memory_address address;
        bool existed;
        byte_state old_state;
    };

    std::unordered_map<memory_address, byte_state, memory_address_hash> byte_safety;
    std::vector<undo_record> undo_log;
    llvm::DenseMap<llvm::Value *, safety> value_safety;
    llvm::MemoryAccess *active_access = nullptr;
    llvm::MemoryAccess *active_generation = nullptr;
    bool default_byte_safety = false;

    auto range_is_safe = [&](const concrete_memory_access &access) {
        for (uint64_t offset = 0; offset < access.size; ++offset) {
            const memory_address address{
                .page = access.address.page, .offset = access.address.offset + offset};
            auto byte = byte_safety.find(address);
            const bool safe =
                byte != byte_safety.end() && byte->second.generation == active_generation
                    ? byte->second.safe
                    : default_byte_safety;
            if (!safe)
                return false;
        }
        return true;
    };

    auto set_byte_safety = [&](const memory_address &address, bool safe) {
        auto byte = byte_safety.find(address);
        const bool has_current_state =
            byte != byte_safety.end() && byte->second.generation == active_generation;
        const bool current_safety = has_current_state ? byte->second.safe : default_byte_safety;
        if (current_safety == safe)
            return;

        undo_log.push_back(
            undo_record{
                .address = address,
                .existed = byte != byte_safety.end(),
                .old_state =
                    byte != byte_safety.end() ? byte->second : byte_state{nullptr, false}});
        if (safe == default_byte_safety) {
            byte_safety.erase(byte);
        }
        else {
            byte_safety.insert_or_assign(
                address, byte_state{.generation = active_generation, .safe = safe});
        }
    };

    auto rollback = [&](size_t checkpoint) {
        while (undo_log.size() > checkpoint) {
            const undo_record &undo = undo_log.back();
            if (undo.existed)
                byte_safety.insert_or_assign(undo.address, undo.old_state);
            else
                byte_safety.erase(undo.address);
            undo_log.pop_back();
        }
    };

    std::function<bool(llvm::Value *)> is_safe_value;
    is_safe_value = [&](llvm::Value *value) -> bool {
        if (llvm::isa<llvm::Constant>(value) || llvm::isa<llvm::Argument>(value))
            return true;

        auto *instruction = llvm::dyn_cast<llvm::Instruction>(value);
        if (!instruction)
            return true;

        auto cached = value_safety.find(value);
        if (cached != value_safety.end())
            return cached->second == safety::safe;
        value_safety.try_emplace(value, safety::visiting);

        bool safe = true;
        llvm::BasicBlock *block = instruction->getParent();
        if (LI.getLoopFor(block) || llvm::isa<llvm::PHINode>(instruction)) {
            safe = false;
        }
        else if (auto *load = llvm::dyn_cast<llvm::LoadInst>(instruction)) {
            auto owner = load_accesses.find(load);
            const concrete_memory_access *access = get_memory_access(load);
            safe = predecessor_counts.lookup(block) <= 1 && owner != load_accesses.end()
                   && owner->second == active_access && access && access->size <= 64
                   && is_safe_value(load->getPointerOperand()) && range_is_safe(*access);
        }
        else if (instruction->mayReadOrWriteMemory()) {
            safe = false;
        }
        else {
            for (llvm::Use &operand : instruction->operands()) {
                if (!is_safe_value(operand.get())) {
                    safe = false;
                    break;
                }
            }
        }

        value_safety[value] = safe ? safety::safe : safety::unsafe;
        return safe;
    };

    struct traversal_frame
    {
        llvm::MemoryAccess *access;
        size_t next_child = 0;
        size_t rollback_checkpoint = 0;
        llvm::MemoryAccess *previous_generation = nullptr;
        bool previous_default_safety = false;
        bool entered = false;
    };

    for (llvm::MemoryAccess *root : roots) {
        byte_safety.clear();
        undo_log.clear();
        active_generation = root;
        default_byte_safety = MSSA->isLiveOnEntryDef(root);

        std::vector<traversal_frame> stack;
        stack.push_back(
            traversal_frame{
                .access = root,
                .previous_generation = active_generation,
                .previous_default_safety = default_byte_safety});
        while (!stack.empty()) {
            traversal_frame &frame = stack.back();
            active_access = frame.access;

            if (!frame.entered) {
                frame.entered = true;
                auto loads = access_loads.find(frame.access);
                if (loads != access_loads.end()) {
                    for (llvm::LoadInst *load : loads->second)
                        is_safe_value(load);
                }
            }

            auto children = def_children.find(frame.access);
            if (children != def_children.end() && frame.next_child < children->second.size()) {
                llvm::MemoryDef *definition = children->second[frame.next_child++];
                llvm::StoreInst *store = def_stores.lookup(definition);
                const concrete_memory_access *access = store ? get_memory_access(store) : nullptr;
                const size_t checkpoint = undo_log.size();
                llvm::MemoryAccess *previous_generation = active_generation;
                const bool previous_default_safety = default_byte_safety;

                if (!access) {
                    active_generation = definition;
                    default_byte_safety = false;
                }
                else {
                    bool store_is_safe = false;
                    if (!LI.getLoopFor(store->getParent())
                        && predecessor_counts.lookup(store->getParent()) <= 1)
                    {
                        store_is_safe = is_safe_value(store->getPointerOperand())
                                        && is_safe_value(store->getValueOperand());
                    }

                    for (uint64_t offset = 0; offset < access->size; ++offset) {
                        const uint64_t byte_offset = access->address.offset + offset;
                        if (byte_offset < access->address.offset)
                            break;
                        set_byte_safety(
                            memory_address{.page = access->address.page, .offset = byte_offset},
                            store_is_safe);
                    }
                }

                stack.push_back(
                    traversal_frame{
                        .access = definition,
                        .rollback_checkpoint = checkpoint,
                        .previous_generation = previous_generation,
                        .previous_default_safety = previous_default_safety});
                continue;
            }

            const size_t checkpoint = frame.rollback_checkpoint;
            llvm::MemoryAccess *previous_generation = frame.previous_generation;
            const bool previous_default_safety = frame.previous_default_safety;
            stack.pop_back();
            rollback(checkpoint);
            active_generation = previous_generation;
            default_byte_safety = previous_default_safety;
        }
    }

    active_access = nullptr;
    for (auto &BB : *function) {
        for (auto &I : BB) {
            auto *load = llvm::dyn_cast<llvm::LoadInst>(&I);
            auto cached = load ? value_safety.find(load) : value_safety.end();
            if (!load || !symex->is_sync(load) || cached == value_safety.end()
                || cached->second != safety::safe)
            {
                continue;
            }

            vexa::value *value = simplify_once(symex->get(load));
            if (!value->is_concrete())
                continue;

            llvm::Value *constant = builder->getIntN(value->size(), value->as_uint64());
            if (constant->getType() != load->getType())
                continue;

            for (llvm::User *user : load->users()) {
                if (auto *user_instruction = llvm::dyn_cast<llvm::Instruction>(user))
                    fold_seeds.push_back(user_instruction);
            }

            load->replaceAllUsesWith(constant);
            dead_loads.push_back(load);
        }
    }

    return memory_propagation_result{
        .dead_loads = std::move(dead_loads), .fold_seeds = std::move(fold_seeds)};
}
