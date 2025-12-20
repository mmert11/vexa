#pragma once
#include <llvm/IR/Value.h>
#include <llvm/Analysis/ConstantFolding.h>

namespace vexa
{
    class value
    {
    public:
        enum class types
        {
            symbolic,
		    concrete
        };
        
        value() = default;
        value(llvm::Value* _lval, const llvm::DataLayout* DL);

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
		types type() const;
		uint64_t as_uint64() const;
        uint64_t size() const;
        bool is_symbolic() const;
        bool is_concrete() const;
        std::string name() const;
    private:
        llvm::Value* val;
        const llvm::DataLayout* DL;
    };
}
