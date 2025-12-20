#include <vexa/vexa.h>

vexa::snapshot vexa::cpu::take_snapshot()
{
    return vexa::snapshot{cpu_state{registers}, memory->take_snapshot()};
}

void vexa::cpu::restore_snapshot(vexa::snapshot ss)
{
    registers = ss.cpu_ss.registers;
    memory->restore_snapshot(ss.mem_ss);
}