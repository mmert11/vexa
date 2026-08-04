#pragma once
#include "global.hpp"

namespace vexa {
template <typename To, typename From>
To* dyn_cast(From *Val) {
    return To::classof(Val) ? static_cast<To*>(Val) : nullptr;
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

    value* simplify();
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
};

class pointer : public value
{
public:
    pointer() : value(kind::pointer) {}
    pointer(const vexa::value& v, std::shared_ptr<mem_page> p)
        : value(v.as_expr(), kind::pointer), page(std::move(p)) {}

    static bool classof(const value* v) {
        return v->get_kind() == kind::pointer;
    }
    std::shared_ptr<mem_page> get_page() { return page; }
private:
    std::shared_ptr<mem_page> page;
};

// a wrapper class holds both llvm pointer and it's symbolic expression
class dual_value
{
public:
    dual_value() {}
    dual_value(llvm::Value* _l, vexa::value* _v) : l(_l), v(_v) {}
    llvm::Value* l;
    vexa::value* v;
};

template <typename From>
vexa::pointer* to_ptr(From* Val) {
    return vexa::dyn_cast<vexa::pointer>(Val);
}
}
