#include <vexa/vexa.h>

vexa::value::value(llvm::Value* v, const llvm::DataLayout* DL, z3::expr e, std::shared_ptr<vexa::symex> _symex) :
    val(v), DL(DL), symex(_symex)
{
    // if this value is already synchronized with symex, dont sync again
    if (false && symex->is_sync(v))
        return;

    e = e.simplify();
    symex->set(v, e);
}

std::string vexa::value::name() const
{
    return val->getName().str();
}

llvm::Value* vexa::value::as_llvm() const
{
    return val;
}

vexa::value::types vexa::value::type() const
{
    return llvm::isa<llvm::ConstantInt>(val)
        ? types::concrete
        : as_expr().is_numeral()
            ? types::concrete
            : types::symbolic;
}

bool vexa::value::is_symbolic() const
{
    return type() == types::symbolic;
}

bool vexa::value::is_concrete() const
{
    return type() == types::concrete;
}

uint64_t vexa::value::as_uint64() const
{
    return is_concrete()
        ? llvm::isa<llvm::ConstantInt>(val) // if value is concrete in llvm or z3, we find which is in and return it
            ? llvm::dyn_cast<llvm::ConstantInt>(val)->getZExtValue()
            : as_expr().get_numeral_uint64()
        : THROW("cannot concretize value");
}

uint64_t vexa::value::size() const
{
    llvm::Type *Ty = val->getType();
    uint64_t bits = DL->getTypeSizeInBits(Ty);
    return bits;
}

z3::expr vexa::value::as_expr() const
{
    return symex->get(val);
}


/*
z3::expr lift_ite_to_root(z3::expr e)
{
    return e;
    if (!e.is_app()) return e;
    
    unsigned num_args = e.num_args();
    if (num_args == 0) return e;

    z3::func_decl op = e.decl();
    z3::context& ctx = e.ctx();

    if (op.decl_kind() == Z3_OP_ITE) {
        z3::expr cond = lift_ite_to_root(e.arg(0));
        z3::expr t = lift_ite_to_root(e.arg(1));
        z3::expr f = lift_ite_to_root(e.arg(2));
        return z3::ite(cond, t, f).simplify();
    }

    z3::expr_vector args(ctx);
    for (unsigned i = 0; i < num_args; i++) {
        args.push_back(lift_ite_to_root(e.arg(i)));
    }

    for (unsigned i = 0; i < num_args; i++) {
        if (args[i].is_app() && args[i].decl().decl_kind() == Z3_OP_ITE) {
            z3::expr cond = args[i].arg(0);
            z3::expr t_val = args[i].arg(1);
            z3::expr f_val = args[i].arg(2);

            z3::expr_vector t_args(ctx);
            for (unsigned j = 0; j < num_args; j++) 
                t_args.push_back(j == i ? t_val : args[j]);

            z3::expr_vector f_args(ctx);
            for (unsigned j = 0; j < num_args; j++) 
                f_args.push_back(j == i ? f_val : args[j]);

            z3::expr new_t = op(t_args);
            z3::expr new_f = op(f_args);

            return z3::ite(cond, lift_ite_to_root(new_t), lift_ite_to_root(new_f)).simplify();
        }
    }

    return op(args).simplify();
}

*/