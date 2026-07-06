#pragma once
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <stack>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <z3++.h>

#include "value/value.hpp"

#ifdef linux
#undef linux
#endif

namespace vexa
{
enum class arch
{
    x86_64
};

enum class os
{
    linux,
    windows
};

namespace ir {class builder;}
class cpu;
class symex;
class memory;

class context
{
public:
    context(vexa::arch _arch);
    std::shared_ptr<vexa::cpu> cpu;
    std::shared_ptr<vexa::memory> memory;
    std::shared_ptr<vexa::symex> symex;
    std::shared_ptr<vexa::ir::builder> builder;

    std::unique_ptr<llvm::LLVMContext> llvm_context;
    std::unique_ptr<llvm::Module> llvm_module;
    std::shared_ptr<z3::context> z3_context;
    llvm::FunctionCallee MarkerFunc;
};

namespace utils {
std::string addr_to_str(uint64_t addr);

// thanks gemini for this class
template<typename K, typename V>
class OrderedMap
{
    using Node = std::pair<K,V>;
    std::list<Node> order;
    std::unordered_map<K, typename std::list<Node>::iterator> map;

public:
    using iterator = typename std::list<Node>::iterator;
    using const_iterator = typename std::list<Node>::const_iterator;
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

    iterator find_iterator(const K& key) {
        auto it = map.find(key);
        if(it != map.end()) {
            return it->second;
        }
        return order.end();
    }

    const_iterator find_iterator(const K& key) const {
        auto it = map.find(key);
        if(it != map.end()) {
            return it->second;
        }
        return order.end();
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

    size_t size() const {
        return map.size();
    }

    auto begin() {
        return order.begin();
    }
    auto end() {
        return order.end();
    }
};
} // namespace utils
}