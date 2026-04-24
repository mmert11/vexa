#pragma once
#include "engine/engine.hpp"

#include <iostream>
#include <filesystem>
#include <sstream>
#include <exception>

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

#define THROW(msg) throw vexa_exception(__FILE__, __LINE__, (msg))
#define TRY() try {
#define CATCH() \
    } catch (const vexa_exception& e) { \
        std::cerr << e.what() << std::endl; \
        std::exit(-1); \
    } catch (const std::exception& e) { \
        std::cerr << "\033[96m[" << std::filesystem::path(__FILE__).filename().string() << ":" << __LINE__ << "]\033[0m " << e.what() << std::endl; \
        std::exit(-1); \
    } catch (...) { \
        std::cerr << "\033[91m[exception]\033[0m unknown critical error" << std::endl; \
        std::exit(-1); \
    }

#define VEXA_ASSERT(cond)                                \
    do {                                                 \
        if (!(cond))                                    \
            THROW("assert failed: " #cond);             \
    } while (0)
