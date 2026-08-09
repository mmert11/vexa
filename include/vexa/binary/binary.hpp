#pragma once
#include <LIEF/LIEF.hpp>

namespace vexa
{
class binary
{
  public:
    binary(std::string filename);
    binary(std::vector<uint8_t> bytes);

    LIEF::Binary *lief() const;
    bool is_elf() const;
    LIEF::ELF::Binary *as_elf() const;
    bool is_pe() const;
    LIEF::PE::Binary *as_pe() const;
    bool is_raw() const;
    std::vector<uint8_t> &raw_data();
    void write(std::string filename) const;

    std::string filename;

  private:
    std::vector<uint8_t> read_binary_file(const std::string &filename);

    std::unique_ptr<LIEF::Binary> _binary;
    std::vector<uint8_t> data;
};
} // namespace vexa