#pragma once
#include "engine/engine.hpp"
#include "ir/recompiler.hpp"

#include <iostream>
#include <filesystem>
#include <sstream>
#include <exception>

static std::string extract_full_func(const std::string& pretty)
{
    auto paren_pos = pretty.find('(');
    if (paren_pos == std::string::npos)
        return pretty;

    std::string before_paren = pretty.substr(0, paren_pos);
    auto space_pos = before_paren.find(' ');
    if (space_pos != std::string::npos)
        return before_paren.substr(space_pos + 1);

    return before_paren;
}

class vexa_exception : public std::exception {
public:
    vexa_exception(
        const std::string& file,
        int line,
        const std::string& func,
        const std::string& msg)
    {
        std::stringstream ss;
        ss << "\033[91m[exception]\033[0m ";
        ss << "\033[96m[" << std::filesystem::path(file).filename().string() << ":" << line << "]\033[0m ";
        std::string func_name = extract_full_func(func);
        ss << "\033[92m[" << func_name << "]\033[0m ";
        ss << msg;
        formatted = ss.str();
    }

    const char* what() const noexcept override {
        return formatted.c_str();
    }

private:
    std::string formatted;
};

#define TRY() \
try \
{

#define CATCH() \
    } catch (const vexa_exception& e) { \
        std::cerr << e.what() << std::endl; \
        std::cerr << "program halted" << std::endl; \
        std::exit(-1); \
    } catch (const std::exception& e) { \
        std::string func_name = extract_full_func(__PRETTY_FUNCTION__); \
        std::cerr << "\033[91m[exception]\033[0m "; \
        std::cerr << "\033[96m[" << std::filesystem::path(__FILE__).filename().string() << ":" << __LINE__ << "]\033[0m "; \
        std::cerr << "\033[92m[" << func_name << "]\033[0m "; \
        std::cerr << e.what() << std::endl; \
        std::cerr << "program halted" << std::endl; \
        std::exit(-1); \
    } catch (...) { \
        std::cerr << "\033[31mUnknown error occurred.\033[0m" << std::endl; \
        std::cerr << "program halted" << std::endl; \
        std::exit(-1); \
    }

#define THROW(error) \
    throw vexa_exception(__FILE__, __LINE__, __PRETTY_FUNCTION__, (error))

namespace vexa
{
    namespace utils
    {
        std::string addr_to_str(uint64_t addr);
    }
}