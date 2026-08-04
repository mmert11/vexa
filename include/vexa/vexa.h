#pragma once
#include "engine/engine.hpp"

#include <iostream>
#include <filesystem>
#include <sstream>
#include <exception>

#include <quill/LogMacros.h>
#include <quill/SimpleSetup.h>
#include <quill/Backend.h>
#include <quill/Frontend.h>

namespace vexa
{

class vexa_exception : public std::exception {
public:
    vexa_exception(const char* file, int line, const std::string& msg) {
        std::stringstream ss;
        ss << "\033[91m[exception]\033[0m"
           << "\033[96m[" << std::filesystem::path(file).filename().string() << ":" << line << "]\033[0m "
           << msg;
        formatted = ss.str();
    }

    const char* what() const noexcept override {
        return formatted.c_str();
    }

private:
    std::string formatted;
};

enum class logging_mode
{
    NONE,
    DEFAULT,
    DEBUG
};

extern quill::Logger *logger;
void init(logging_mode mode = logging_mode::DEFAULT);
void set_logging_mode(vexa::logging_mode mode);

namespace utils {

std::string addr_to_str(uint64_t addr);
void set_logger_error_mode();
uint64_t hash_file_fnv1a64(const std::string& path);

}

}

#define THROW(msg, ...) \
    do { \
        utils::set_logger_error_mode();                      \
        LOG_ERROR(logger, msg, ##__VA_ARGS__);               \
        throw std::runtime_error("");  \
    } while (0)

#define VEXA_ASSERT(cond)                                \
    do {                                                 \
        if (!(cond))                                    \
            THROW("assert failed: " #cond);             \
    } while (0)
