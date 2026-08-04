#include <vexa/vexa.h>
#include <bitwuzla/cpp/bitwuzla.h>

namespace vexa
{
quill::Logger* logger = nullptr;
}

static std::unordered_map<vexa::logging_mode, quill::LogLevel> vexa2quill_log_level = {
        {vexa::logging_mode::NONE, quill::LogLevel::None},
        {vexa::logging_mode::DEFAULT, quill::LogLevel::Info},
        {vexa::logging_mode::DEBUG, quill::LogLevel::Debug}
};

void vexa::init(logging_mode mode)
{
    LIEF::logging::set_level(LIEF::logging::LEVEL::OFF);

    // init quill (for logging)
    quill::Backend::start<quill::FrontendOptions>(quill::BackendOptions{}, quill::SignalHandlerOptions{});
    logger = quill::Frontend::create_or_get_logger(
                 "vexa", quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1"),
                 quill::PatternFormatterOptions{"%(logger:<6) "
                         "%(log_level:<8) %(message)",
                         "%H:%M:%S.%Qns", quill::Timezone::GmtTime});
    logger->set_log_level(vexa2quill_log_level.at(mode));
}

void vexa::set_logging_mode(vexa::logging_mode mode)
{
    logger->set_log_level(vexa2quill_log_level.at(mode));
}

void vexa::utils::set_logger_error_mode()
{
    quill::Frontend::remove_logger_blocking(logger);
    logger = quill::Frontend::create_or_get_logger(
                 "vexa", quill::Frontend::create_or_get_sink<quill::ConsoleSink>("sink_id_1"),
                 quill::PatternFormatterOptions{"%(logger:<6) "
                         "%(log_level:<8) %(short_source_location:<15) %(message)",
                         "%H:%M:%S.%Qns", quill::Timezone::GmtTime});
    logger->set_immediate_flush(1);
}

std::string vexa::utils::addr_to_str(uint64_t addr)
{
    std::stringstream ss;
    ss << std::hex << addr;
    return ss.str();
}

uint64_t vexa::utils::hash_file_fnv1a64(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file)
        THROW("Couldn't open file ", path);

    constexpr std::uint64_t offset_basis = 14695981039346656037ULL;
    constexpr std::uint64_t prime = 1099511628211ULL;

    std::uint64_t hash = offset_basis;

    char buffer[8192];

    while (file) {
        file.read(buffer, sizeof(buffer));
        const std::streamsize size = file.gcount();

        for (std::streamsize i = 0; i < size; ++i) {
            hash ^= static_cast<unsigned char>(buffer[i]);
            hash *= prime;
        }
    }

    if (file.bad())
        THROW("Error while reading the file", path);

    return hash;
}