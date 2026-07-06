#pragma once
#include "global.hpp"

namespace vexa {
template <typename To, typename From>
To* dyn_cast(From *Val) {
    return To::classof(Val) ? static_cast<To*>(Val) : nullptr;
}

template <typename To, typename From>
std::shared_ptr<To> dyn_cast(std::shared_ptr<From> Val) {
    if (To::classof(Val.get())) {
        return std::static_pointer_cast<To>(Val);
    }
    return nullptr;
}

struct mem_page;
class symex;
class value
{
public:
    enum struct kind
    {
        value,
        pointer
    };

    value() : _kind(kind::value) {}
    value(kind k) : _kind(k) {}
    value(z3::expr e) : _kind(kind::value), expr(e) {}
    value(z3::expr e, kind k) : _kind(k), expr(e) {}

    void simplify();
    z3::expr as_expr() const;
    uint64_t as_uint64() const;
    uint64_t size() const;
    bool is_symbolic() const;
    bool is_concrete() const;
    std::string name() const;
    kind get_kind() const {
        return _kind;
    }

    virtual ~value() = default;
private:
    const kind _kind;
    std::optional<z3::expr> expr;
    //std::shared_ptr<vexa::symex> symex;
};

class pointer : public value
{
public:
    pointer() : value(kind::pointer) {}
    pointer(vexa::value v, std::shared_ptr<mem_page> p) : value(v.as_expr(), kind::pointer), page(p) {}

    static bool classof(const value* v) {
        return v->get_kind() == kind::pointer;
    }
    std::shared_ptr<mem_page> get_page() { return page; }
private:
    std::shared_ptr<mem_page> page;
};

using shared_value = std::shared_ptr<value>;
using shared_pointer = std::shared_ptr<pointer>;

// a wrapper class holds both llvm pointer and it's symbolic expression
class dual_value
{
public:
    dual_value() {}
    dual_value(llvm::Value* _l, vexa::shared_value _v) : l(_l), v(_v) {}
    llvm::Value* l;
    vexa::shared_value v;
};

template <typename From>
vexa::shared_pointer to_ptr(std::shared_ptr<From> Val) {
    if (pointer::classof(Val.get())) {
        return std::static_pointer_cast<pointer>(Val);
    }
    return nullptr;
}
}
