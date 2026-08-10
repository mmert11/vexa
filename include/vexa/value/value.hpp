#pragma once
#include "global.hpp"
#include <bitwuzla/cpp/bitwuzla.h>
#include <llvm/IR/Value.h>
#include <memory>
#include <string>
#include <vector>

namespace bw = bitwuzla;

namespace vexa
{
class context;
template <typename To, typename From> To *dyn_cast(From *Val)
{
    return To::classof(Val) ? static_cast<To *>(Val) : nullptr;
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

    value(vexa::context *c)
        : context(c), _kind(kind::value), term_manager(nullptr), solver(nullptr), simplified(true)
    {}
    value(vexa::context *c, kind k)
        : context(c), _kind(k), term_manager(nullptr), solver(nullptr), simplified(true)
    {}
    value(vexa::context *c, bw::Term e, bw::TermManager &tm, bw::Bitwuzla &bzla)
        : context(c),
          _kind(kind::value),
          term(std::move(e)),
          term_manager(&tm),
          solver(&bzla),
          simplified(term.is_value())
    {}

    std::vector<bw::Term>
        possible_values(const std::vector<bw::Term> &constraints, bw::Result *result = nullptr);
    value *simplify();
    bw::Term as_expr() const;
    bw::Term as_expr_bool() const;
    bw::Term operator+(const value &rhs) const;
    bw::Term operator-(const value &rhs) const;
    bw::Term operator*(const value &rhs) const;
    bw::Term operator/(const value &rhs) const;
    bw::Term operator%(const value &rhs) const;
    bw::Term operator&(const value &rhs) const;
    bw::Term operator|(const value &rhs) const;
    bw::Term operator^(const value &rhs) const;
    bw::Term operator<<(const value &rhs) const;
    bw::Term operator>>(const value &rhs) const;
    bw::Term operator-() const;
    bw::Term operator~() const;
    bw::Term operator==(const value &rhs) const;
    bw::Term operator!() const;
    bw::Term operator!=(const value &rhs) const;
    bw::Term operator<(const value &rhs) const;
    bw::Term operator<=(const value &rhs) const;
    bw::Term operator>(const value &rhs) const;
    bw::Term operator>=(const value &rhs) const;
    bw::Term udiv(const value &rhs) const;
    bw::Term urem(const value &rhs) const;
    bw::Term lshr(const value &rhs) const;
    bw::Term ult(const value &rhs) const;
    bw::Term ule(const value &rhs) const;
    bw::Term ugt(const value &rhs) const;
    bw::Term uge(const value &rhs) const;
    bw::Term concat(const value &rhs) const;
    bw::Term extract(uint64_t high, uint64_t low) const;
    bw::Term zext(uint64_t amount) const;
    bw::Term sext(uint64_t amount) const;
    bw::Term ite(const value &then_value, const value &else_value) const;
    uint64_t as_uint64() const;
    static uint64_t as_uint64(const bw::Term &term);
    uint64_t size() const;
    bool is_symbolic() const;
    bool is_concrete() const;
    std::string name() const;
    kind get_kind() const { return _kind; }

    virtual ~value() = default;

  protected:
    value(const value &other, kind k)
        : context(other.context),
          _kind(k),
          term(other.term),
          term_manager(other.term_manager),
          solver(other.solver),
          simplified(other.simplified)
    {}

  private:
    bw::Term unary(bw::Kind op) const;
    bw::Term binary(bw::Kind op, const value &rhs) const;
    vexa::context *context;
    const kind _kind;
    bw::Term term;
    bw::TermManager *term_manager;
    bw::Bitwuzla *solver;
    bool simplified;
};

class pointer : public value
{
  public:
    pointer(vexa::context* c) : value(c, kind::pointer) {}
    pointer(const vexa::value &v, std::shared_ptr<mem_page> p)
        : value(v, kind::pointer), page(std::move(p))
    {}

    static bool classof(const value *v) { return v->get_kind() == kind::pointer; }
    std::shared_ptr<mem_page> get_page() { return page; }

  private:
    std::shared_ptr<mem_page> page;
};

template <typename From> vexa::pointer *to_ptr(From *Val)
{
    return vexa::dyn_cast<vexa::pointer>(Val);
}

// a wrapper class holds both llvm pointer and it's symbolic expression
class dual_pointer
{
  public:
    dual_pointer() {}
    dual_pointer(llvm::Value *_l, vexa::pointer *_v) : l(_l), v(_v) {}
    llvm::Value *l;
    vexa::pointer *v;
};

class dual_value
{
  public:
    dual_value() {}
    dual_value(llvm::Value *_l, vexa::value *_v) : l(_l), v(_v) {}
    dual_pointer to_ptr() { return {l, vexa::to_ptr(v)}; }

    llvm::Value *l;
    vexa::value *v;
};
} // namespace vexa
