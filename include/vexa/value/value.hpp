#pragma once

#include <llvm/IR/Value.h>
#include <llvm/Analysis/ConstantFolding.h>

#include <z3++.h>

namespace vexa
{
    class symex;

    class value
    {
    public:
        enum class types
        {
            symbolic,
		    concrete
        };
        
        value() = default;
        value(llvm::Value* _lval, const llvm::DataLayout* DL, z3::expr e, std::shared_ptr<vexa::symex> symex);

        value(const value&) = default;
        value(value&&) noexcept = default;
        value& operator=(const value&) = default;
        value& operator=(value&&) noexcept = default;
        /*
        value operator+(const value&);
        value operator&(const value&);
        value operator|(const value&);
        value operator<<(const value&);
        value operator>>(const value&);
        */

        llvm::Value* as_llvm() const;
        z3::expr as_expr() const;
		types type() const;
		uint64_t as_uint64() const;
        uint64_t size() const;
        bool is_symbolic() const;
        bool is_concrete() const;
        std::string name() const;
    private:
        llvm::Value* val;
        const llvm::DataLayout* DL;
        std::shared_ptr<vexa::symex> symex;
    };
}
