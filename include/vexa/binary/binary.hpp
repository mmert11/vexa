#pragma once
#include <LIEF/LIEF.hpp>

namespace vexa
{
    class binary
    {
    public:
        binary(std::string filename);
        binary(std::vector<uint8_t> bytes);
        
        LIEF::Binary* lief() { return _binary.get(); }
        bool is_elf() const;
        LIEF::ELF::Binary* as_elf() const;
        bool is_pe() const;
        LIEF::PE::Binary* as_pe() const;
        void write(std::string filename) const;
        
        std::string filename;
        //void patch_at_rva(uint64_t rva, const std::vector<uint8_t>& data);

    private:
        std::unique_ptr<LIEF::Binary> _binary;
    };
}