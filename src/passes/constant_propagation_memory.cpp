#include <vexa/vexa.h>
#include <vexa/passes/constant_propagation.hpp>

#include <functional>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include <llvm/ADT/DenseMap.h>
#include <llvm/TargetParser/Triple.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/Analysis/AssumptionCache.h>
#include <llvm/Analysis/BasicAliasAnalysis.h>
#include <llvm/Analysis/LoopInfo.h>
#include <llvm/Analysis/MemorySSA.h>
#include <llvm/Analysis/TargetLibraryInfo.h>
#include <llvm/IR/CFG.h>

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

    std::unordered_map<llvm::Instruction *, concrete_memory_access>
        concrete_accesses;
    std::unordered_set<llvm::Instruction *> attempted_accesses;
    auto get_memory_access =
        [&](llvm::Instruction *instruction)
            -> const concrete_memory_access *
    {
        auto cached = concrete_accesses.find(instruction);
        if (cached != concrete_accesses.end())
            return &cached->second;
        if (!attempted_accesses.insert(instruction).second)
            return nullptr;

        llvm::Value *pointer_operand = nullptr;
        llvm::Type *accessed_type = nullptr;
        if (auto *load = llvm::dyn_cast<llvm::LoadInst>(instruction))
        {
            pointer_operand = load->getPointerOperand();
            accessed_type = load->getType();
        }
        else if (auto *store = llvm::dyn_cast<llvm::StoreInst>(instruction))
        {
            pointer_operand = store->getPointerOperand();
            accessed_type = store->getValueOperand()->getType();
        }
        else
        {
            return nullptr;
        }

        if (!symex->is_sync(pointer_operand))
            return nullptr;

        vexa::value *expression =
            simplify_once(symex->get(pointer_operand));
        auto *pointer = vexa::dyn_cast<vexa::pointer>(expression);
        if (!pointer || !pointer->is_concrete())
            return nullptr;

        auto [it, inserted] = concrete_accesses.emplace(
            instruction,
            concrete_memory_access{
                .address = {
                    .page = pointer->get_page().get(),
                    .offset = pointer->as_uint64()
                },
                .size = DL.getTypeStoreSize(accessed_type).getFixedValue()
            });
        return &it->second;
    };

    struct block_info
    {
        std::vector<llvm::BasicBlock *> predecessors;
    };

    std::unordered_map<llvm::BasicBlock *, block_info> blocks;
    blocks.reserve(function->size());
    for (auto &BB : *function)
    {
        auto &block = blocks[&BB];
        block.predecessors.assign(llvm::pred_begin(&BB),
                                  llvm::pred_end(&BB));
    }

    std::vector<llvm::Instruction *> dead_loads;
    std::vector<llvm::Instruction *> fold_seeds;
    llvm::TargetLibraryInfoImpl TLII(
        llvm::Triple(function->getParent()->getTargetTriple()));
    llvm::TargetLibraryInfo TLI(TLII);
    llvm::AssumptionCache AC(*function);
    llvm::AAResults AA(TLI);
    llvm::BasicAAResult BasicAA(DL, *function, TLI, AC, &DT);
    AA.addAAResult(BasicAA);
    auto MSSA = std::make_unique<llvm::MemorySSA>(
        *function, &AA, &DT);
    llvm::MemorySSAWalker *memory_walker = MSSA->getWalker();

    struct memory_address_hash
    {
        size_t operator()(const memory_address &address) const
        {
            size_t hash =
                std::hash<const vexa::mem_page *>{}(address.page);
            hash ^= std::hash<uint64_t>{}(address.offset) +
                    0x9e3779b9 + (hash << 6) + (hash >> 2);
            return hash;
        }
    };

    std::unordered_set<memory_address, memory_address_hash>
        concretely_written_bytes;
    bool has_unknown_writes = false;
    for (auto &BB : *function)
    {
        for (auto &I : BB)
        {
            if (auto *store = llvm::dyn_cast<llvm::StoreInst>(&I))
            {
                const concrete_memory_access *access =
                    get_memory_access(store);
                if (!access)
                {
                    has_unknown_writes = true;
                    continue;
                }

                for (uint64_t offset = 0; offset < access->size; ++offset)
                {
                    const uint64_t byte_offset =
                        access->address.offset + offset;
                    if (byte_offset < access->address.offset)
                        break;
                    concretely_written_bytes.insert(memory_address{
                        .page = access->address.page,
                        .offset = byte_offset
                    });
                }
            }
            else if (I.mayWriteToMemory())
            {
                has_unknown_writes = true;
            }
        }
    }

    auto crosses_memory_phi =
        [](llvm::MemoryAccess *from, llvm::MemoryAccess *to)
    {
        llvm::MemoryAccess *cursor = from;
        while (cursor && cursor != to)
        {
            if (llvm::isa<llvm::MemoryPhi>(cursor))
                return true;

            auto *use_or_def =
                llvm::dyn_cast<llvm::MemoryUseOrDef>(cursor);
            if (!use_or_def)
                break;
            cursor = use_or_def->getDefiningAccess();
        }
        return false;
    };

    std::unordered_map<llvm::MemoryAccess *, bool> memory_phi_paths;
    auto crosses_any_memory_phi = [&](llvm::MemoryAccess *start)
    {
        std::vector<llvm::MemoryAccess *> traversed;
        llvm::MemoryAccess *cursor = start;
        bool crosses_phi = false;
        while (cursor && !MSSA->isLiveOnEntryDef(cursor))
        {
            auto cached = memory_phi_paths.find(cursor);
            if (cached != memory_phi_paths.end())
            {
                crosses_phi = cached->second;
                break;
            }
            traversed.push_back(cursor);

            if (llvm::isa<llvm::MemoryPhi>(cursor))
            {
                crosses_phi = true;
                break;
            }

            auto *use_or_def =
                llvm::dyn_cast<llvm::MemoryUseOrDef>(cursor);
            if (!use_or_def)
            {
                crosses_phi = true;
                break;
            }
            cursor = use_or_def->getDefiningAccess();
        }

        for (llvm::MemoryAccess *access : traversed)
            memory_phi_paths.emplace(access, crosses_phi);
        return crosses_phi;
    };

    auto has_concrete_write =
        [&](const concrete_memory_access &access)
    {
        for (uint64_t offset = 0; offset < access.size; ++offset)
        {
            const uint64_t byte_offset =
                access.address.offset + offset;
            if (byte_offset < access.address.offset)
                break;
            if (concretely_written_bytes.count(memory_address{
                    .page = access.address.page,
                    .offset = byte_offset
                }))
            {
                return true;
            }
        }
        return false;
    };

    struct concrete_location_key
    {
        memory_address address;
        uint64_t size;

        bool operator==(const concrete_location_key &) const = default;
    };

    struct concrete_location_hash
    {
        size_t operator()(const concrete_location_key &key) const
        {
            size_t hash = memory_address_hash{}(key.address);
            hash ^= std::hash<uint64_t>{}(key.size) +
                    0x9e3779b9 + (hash << 6) + (hash >> 2);
            return hash;
        }
    };

    std::unordered_map<
        concrete_location_key,
        uint32_t,
        concrete_location_hash> concrete_location_ids;
    llvm::DenseMap<uint64_t, llvm::MemoryAccess *> concrete_clobbers;

    auto get_concrete_clobber =
        [&](llvm::MemoryAccess *start,
            const concrete_memory_access &load_access)
    {
        concrete_location_key location{
            .address = load_access.address,
            .size = load_access.size
        };
        auto [location_it, inserted] = concrete_location_ids.emplace(
            location,
            static_cast<uint32_t>(concrete_location_ids.size()));
        const uint64_t location_id = location_it->second;

        auto make_key = [&](llvm::MemoryAccess *access)
        {
            uint64_t access_id;
            if (auto *definition =
                    llvm::dyn_cast<llvm::MemoryDef>(access))
            {
                access_id = definition->getID();
            }
            else
            {
                access_id = llvm::cast<llvm::MemoryPhi>(access)->getID();
            }
            return (location_id << 32) | access_id;
        };

        std::vector<uint64_t> traversed;
        llvm::MemoryAccess *cursor = start;
        llvm::MemoryAccess *clobber = nullptr;

        while (cursor)
        {
            const uint64_t key = make_key(cursor);
            auto cached = concrete_clobbers.find(key);
            if (cached != concrete_clobbers.end())
            {
                clobber = cached->second;
                break;
            }
            traversed.push_back(key);

            if (MSSA->isLiveOnEntryDef(cursor) ||
                llvm::isa<llvm::MemoryPhi>(cursor))
            {
                clobber = cursor;
                break;
            }

            auto *definition = llvm::dyn_cast<llvm::MemoryDef>(cursor);
            auto *store = definition
                ? llvm::dyn_cast<llvm::StoreInst>(
                      definition->getMemoryInst())
                : nullptr;
            if (!store)
            {
                clobber = cursor;
                break;
            }

            const concrete_memory_access *store_access =
                get_memory_access(store);
            if (!store_access)
            {
                clobber = cursor;
                break;
            }

            bool overlaps = false;
            if (store_access->address.page ==
                load_access.address.page)
            {
                for (uint64_t offset = 0;
                     offset < load_access.size;
                     ++offset)
                {
                    const uint64_t byte_address =
                        load_access.address.offset + offset;
                    if (byte_address < store_access->address.offset)
                        continue;
                    if (byte_address - store_access->address.offset <
                        store_access->size)
                    {
                        overlaps = true;
                        break;
                    }
                }
            }

            if (overlaps)
            {
                clobber = cursor;
                break;
            }
            cursor = definition->getDefiningAccess();
        }

        for (uint64_t key : traversed)
            concrete_clobbers.try_emplace(key, clobber);
        return clobber;
    };
    enum class safety
    {
        visiting,
        safe,
        unsafe
    };

    std::unordered_map<llvm::Value *, safety> value_safety;
    std::function<bool(llvm::Value *)> is_safe_value;

    is_safe_value = [&](llvm::Value *value) -> bool
    {
        if (llvm::isa<llvm::Constant>(value) ||
            llvm::isa<llvm::Argument>(value))
        {
            return true;
        }

        auto *instruction = llvm::dyn_cast<llvm::Instruction>(value);
        if (!instruction)
            return true;

        auto cached = value_safety.find(value);
        if (cached != value_safety.end())
            return cached->second == safety::safe;
        value_safety.emplace(value, safety::visiting);

        bool safe = true;
        llvm::BasicBlock *block = instruction->getParent();
        if (LI.getLoopFor(block) ||
            llvm::isa<llvm::PHINode>(instruction))
        {
            safe = false;
        }
        else if (auto *load = llvm::dyn_cast<llvm::LoadInst>(instruction))
        {
            if (blocks.at(block).predecessors.size() > 1)
            {
                safe = false;
            }
            else
            {
                const concrete_memory_access *load_access_ptr =
                    get_memory_access(load);
                safe = load_access_ptr &&
                       load_access_ptr->size <= 64 &&
                       is_safe_value(load->getPointerOperand());

                if (safe)
                {
                    const concrete_memory_access load_access =
                        *load_access_ptr;
                    const uint64_t complete_mask =
                        load_access.size == 64
                            ? ~uint64_t{0}
                            : (uint64_t{1} << load_access.size) - 1;
                    uint64_t resolved_mask = 0;

                    llvm::BatchAAResults BatchAA(AA);
                    llvm::MemoryLocation location =
                        llvm::MemoryLocation::get(load);
                    auto *memory_use = MSSA->getMemoryAccess(load);
                    llvm::MemoryAccess *raw_access =
                        memory_use->getDefiningAccess();
                    llvm::MemoryAccess *clobber;
                    if (!has_unknown_writes &&
                        !has_concrete_write(load_access) &&
                        !crosses_any_memory_phi(raw_access))
                    {
                        clobber = MSSA->getLiveOnEntryDef();
                    }
                    else if (!has_unknown_writes)
                    {
                        clobber = get_concrete_clobber(
                            raw_access, load_access);
                    }
                    else
                    {
                        clobber =
                            memory_walker->getClobberingMemoryAccess(
                                load, BatchAA);
                    }
                    std::unordered_set<llvm::MemoryAccess *> visited;

                    if (crosses_memory_phi(raw_access, clobber))
                        safe = false;

                    while (safe && resolved_mask != complete_mask)
                    {
                        if (!clobber ||
                            !visited.insert(clobber).second ||
                            llvm::isa<llvm::MemoryPhi>(clobber))
                        {
                            safe = false;
                            break;
                        }

                        if (MSSA->isLiveOnEntryDef(clobber))
                        {
                            resolved_mask = complete_mask;
                            break;
                        }

                        auto *definition =
                            llvm::dyn_cast<llvm::MemoryDef>(clobber);
                        auto *store = definition
                            ? llvm::dyn_cast<llvm::StoreInst>(
                                  definition->getMemoryInst())
                            : nullptr;
                        if (!store)
                        {
                            safe = false;
                            break;
                        }

                        const concrete_memory_access *store_access_ptr =
                            get_memory_access(store);
                        if (!store_access_ptr)
                        {
                            safe = false;
                            break;
                        }

                        const concrete_memory_access store_access =
                            *store_access_ptr;
                        uint64_t store_mask = 0;
                        if (store_access.address.page ==
                            load_access.address.page)
                        {
                            for (uint64_t offset = 0;
                                 offset < load_access.size;
                                 ++offset)
                            {
                                const uint64_t mask = uint64_t{1} << offset;
                                if (resolved_mask & mask)
                                    continue;

                                const uint64_t byte_address =
                                    load_access.address.offset + offset;
                                if (byte_address <
                                    store_access.address.offset)
                                {
                                    continue;
                                }

                                const uint64_t store_offset =
                                    byte_address -
                                    store_access.address.offset;
                                if (store_offset < store_access.size)
                                    store_mask |= mask;
                            }
                        }

                        if (store_mask)
                        {
                            if (LI.getLoopFor(store->getParent()) ||
                                blocks.at(store->getParent())
                                        .predecessors.size() > 1 ||
                                !is_safe_value(
                                    store->getPointerOperand()) ||
                                !is_safe_value(
                                    store->getValueOperand()))
                            {
                                safe = false;
                                break;
                            }
                            resolved_mask |= store_mask;
                        }

                        llvm::MemoryAccess *next_raw =
                            definition->getDefiningAccess();
                        llvm::MemoryAccess *next_clobber;
                        if (!has_unknown_writes)
                        {
                            next_clobber = get_concrete_clobber(
                                next_raw, load_access);
                        }
                        else
                        {
                            next_clobber =
                                memory_walker->getClobberingMemoryAccess(
                                    next_raw, location, BatchAA);
                        }
                        if (crosses_memory_phi(next_raw, next_clobber))
                        {
                            safe = false;
                            break;
                        }
                        clobber = next_clobber;
                    }
                }
            }
        }
        else if (instruction->mayReadOrWriteMemory())
        {
            safe = false;
        }
        else
        {
            for (llvm::Use &operand : instruction->operands())
            {
                if (!is_safe_value(operand.get()))
                {
                    safe = false;
                    break;
                }
            }
        }

        value_safety[value] = safe ? safety::safe : safety::unsafe;
        return safe;
    };

    for (auto &BB : *function)
    {
        for (auto &I : BB)
        {
            auto *load = llvm::dyn_cast<llvm::LoadInst>(&I);
            if (!load || !symex->is_sync(load))
                continue;
            vexa::value *value = simplify_once(symex->get(load));
            if (!value->is_concrete())
                continue;
            if (!is_safe_value(load))
                continue;

            llvm::Value *constant =
                builder->getIntN(value->size(), value->as_uint64());
            if (constant->getType() != load->getType())
                continue;

            for (llvm::User *user : load->users())
            {
                if (auto *user_instruction =
                        llvm::dyn_cast<llvm::Instruction>(user))
                {
                    fold_seeds.push_back(user_instruction);
                }
            }

            load->replaceAllUsesWith(constant);
            dead_loads.push_back(load);
        }
    }

    return memory_propagation_result{
        .dead_loads = std::move(dead_loads),
        .fold_seeds = std::move(fold_seeds)
    };
}