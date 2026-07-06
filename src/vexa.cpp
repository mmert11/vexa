#include <vexa/vexa.h>
#include <bitwuzla/cpp/bitwuzla.h>

std::string vexa::utils::addr_to_str(uint64_t addr)
{
    std::stringstream ss;
    ss << std::hex << addr;
    return ss.str();
}