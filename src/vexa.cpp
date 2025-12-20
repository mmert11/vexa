#include <vexa/vexa.h>

std::string vexa::utils::addr_to_str(uint64_t addr)
{
    std::stringstream ss;
    ss << std::hex << addr;
    return ss.str();
}