#include <vexa/vexa.h>

vexa::binary::binary(std::string _filename)
{
    TRY()
    _binary = LIEF::Parser::parse(_filename);
    filename = _filename;

    if (!_binary) {
        THROW("binary parsing error");
    }
    CATCH()
}

vexa::binary::binary(std::vector<uint8_t> bytes)
{
    TRY()
    _binary = LIEF::Parser::parse(bytes);

    if (!_binary)
        THROW("binary parsing error");
    CATCH()
}

bool vexa::binary::is_elf() const
{
    return _binary->format() == LIEF::Binary::FORMATS::ELF;
}

LIEF::ELF::Binary* vexa::binary::as_elf() const
{
    return dynamic_cast<LIEF::ELF::Binary*>(_binary.get());
}

bool vexa::binary::is_pe() const
{
    return _binary->format() == LIEF::Binary::FORMATS::PE;
}

LIEF::PE::Binary* vexa::binary::as_pe() const
{
    return dynamic_cast<LIEF::PE::Binary*>(_binary.get());
}

void vexa::binary::write(std::string filename) const
{
    if (is_elf())
        as_elf()->write(filename);
    else
        as_pe()->write(filename);
}
