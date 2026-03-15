#pragma once
#include <stdint.h>
#include <map>
#include <unordered_map>
#include <string>
#include <stack>

#include <Zydis/Zydis.h>

#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>

#include "../vexa.h"
#include "../ir/builder.hpp"
#include "../memory/memory.hpp"

namespace vexa {
namespace utils {

// thanks gemini and chatgpt for this class
template<typename K, typename V>
class OrderedMap
{
    using Node = std::pair<K,V>;
    std::list<Node> order;
    std::unordered_map<K, typename std::list<Node>::iterator> map;

public:
    OrderedMap() = default;

    OrderedMap(const OrderedMap& other) {
        for (const auto& pair : other.order) {
            insert(pair.first, pair.second);
        }
    }

    OrderedMap& operator=(const OrderedMap& other) {
        if (this != &other) {
            order.clear();
            map.clear();
            for (const auto& pair : other.order) {
                insert(pair.first, pair.second);
            }
        }
        return *this;
    }

    void insert(const K& k, const V& v) {
        if(map.find(k) != map.end())
            return;
        order.emplace_back(k,v);
        auto it = std::prev(order.end());
        map[k] = it;
    }

    V& operator[](const K& key) {
        auto it = map.find(key);
        if(it != map.end())
            return it->second->second;

        order.emplace_back(key, V{});
        auto lit = std::prev(order.end());
        map[key] = lit;
        return lit->second;
    }

    size_t count(const K& key) const {
        return map.count(key);
    }

    void erase(const K& key) {
        auto it = map.find(key);
        if(it == map.end()) return;

        order.erase(it->second);
        map.erase(it);
    }

    void erase_range(K a, K b) {
        if (map.find(a) == map.end() || map.find(b) == map.end()) return;
        
        auto itA = map.at(a);
        auto itB = map.at(b);

        for (auto it = itA; ; ) {
            auto next_it = std::next(it);
            K key = it->first;
            
            bool isLast = (it == itB);
            
            order.erase(it);
            map.erase(key);
            
            if (isLast) break;
            it = next_it;
        }
    }

    V* find(const K& key) {
        auto it = map.find(key);
        if(it == map.end()) return nullptr;
        return &it->second->second;
    }

    auto begin() { return order.begin(); }
    auto end() { return order.end(); }
};

} // namespace utils
} // namespace vexa

namespace vexa
{
    typedef uint16_t reg_t;

    struct register_desc
    {
		reg_t base_id;
		uint16_t size_bits;
		uint16_t offset_bits;
	};

    struct cpu_state
    {
    public:
        std::map<reg_t, vexa::value> registers;
    };

    struct snapshot
    {
    public:
        cpu_state cpu_ss;
        mem_state mem_ss;
        vexa::utils::OrderedMap<uint64_t, llvm::BasicBlock*>  lifted_blocks;
    };

    struct path_state
    {
    public:
        snapshot ss;
        vexa::value rip;
        llvm::BasicBlock* bb;
    };

    class cpu
    {
    public:
        cpu(std::shared_ptr<ir::builder> _builder, std::shared_ptr<vexa::symex> _symex,
            std::shared_ptr<vexa::memory> _memory, std::shared_ptr<vexa::context> _context) :
            builder(_builder), symex(_symex), memory(_memory), context(_context) {};
            
        virtual void run() = 0;
        virtual void write_register(reg_t reg, vexa::value value) = 0;
        virtual vexa::value read_register(reg_t reg) = 0;
        int lifted_count;
        vexa::value stack_ptr, original_sp;

        snapshot take_snapshot();
        void restore_snapshot(snapshot ss);
    
        std::stack<path_state> unexplored_paths;
        std::map<reg_t, vexa::value> registers;
        vexa::utils::OrderedMap<uint64_t, llvm::BasicBlock*> lifted_blocks;

        std::shared_ptr<ir::builder> builder;
        std::shared_ptr<vexa::symex> symex;
        std::shared_ptr<vexa::memory> memory;
        std::shared_ptr<vexa::context> context;
    };
}
