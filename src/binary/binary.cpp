#include <vexa/vexa.h>

std::vector<uint8_t> vexa::binary::read_binary_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        THROW("Couldn't open {}", filename);
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<uint8_t> buffer(size);

    if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        return buffer;
    } else {
        THROW("Couldn't read {}", filename);
    }
}

vexa::binary::binary(std::string _filename)
{
    if (!std::filesystem::exists(_filename))
        THROW("No file named {}", _filename);

    std::vector<uint8_t> _data = read_binary_file(_filename);
    if (_data.empty())
        THROW("Binary is empty: {}", _filename);

    _binary = LIEF::Parser::parse(_filename);
    filename = _filename;
    data = _data;
}

vexa::binary::binary(std::vector<uint8_t> bytes)
{
    _binary = LIEF::Parser::parse(bytes);
    data = bytes;
}

bool vexa::binary::is_raw() const
{
    if (!_binary)
        return true;

    if (!is_elf() && !is_pe())
        return true;

    return false;
}

std::vector<uint8_t>& vexa::binary::raw_data()
{
    return data;
}

bool vexa::binary::is_elf() const
{
    return _binary && _binary->format() == LIEF::Binary::FORMATS::ELF;
}

LIEF::ELF::Binary* vexa::binary::as_elf() const
{
    if (!_binary || !is_elf())
        THROW("Binary is not an ELF");

    return dynamic_cast<LIEF::ELF::Binary*>(_binary.get());
}

bool vexa::binary::is_pe() const
{
    return _binary && _binary->format() == LIEF::Binary::FORMATS::PE;
}

LIEF::PE::Binary* vexa::binary::as_pe() const
{
    if (!_binary || !is_pe())
        THROW("Binary is not a PE");

    return dynamic_cast<LIEF::PE::Binary*>(_binary.get());
}

void vexa::binary::write(std::string filename) const
{
    if (is_elf())
        as_elf()->write(filename);
    else
        as_pe()->write(filename);
}

LIEF::Binary* vexa::binary::lief() const
{
    if (!_binary)
        THROW("This is a raw binary, not LIEF::Binary, use raw_data()");

    return _binary.get();
}