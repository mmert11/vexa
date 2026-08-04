#pragma once

#include <array>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

#ifdef _WIN32
#define VEXA_POPEN  _popen
#define VEXA_PCLOSE _pclose
#else
#include <sys/wait.h>
#define VEXA_POPEN  popen
#define VEXA_PCLOSE pclose
#endif

struct ProcessResult
{
    std::string output;
    int exit_code = -1;
};

inline ProcessResult run_command(const std::string& command)
{
    std::array<char, 4096> buffer{};
    std::string output;

    FILE* pipe = VEXA_POPEN(command.c_str(), "r");
    if (!pipe)
        return {};

    while (std::fgets(buffer.data(), buffer.size(), pipe))
        output += buffer.data();

    const int status = VEXA_PCLOSE(pipe);

#ifdef _WIN32
    const int exit_code = status;
#else
    int exit_code = -1;

    if (status != -1) {
        if (WIFEXITED(status))
            exit_code = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            exit_code = 128 + WTERMSIG(status);
    }
#endif

    return {
        .output = std::move(output),
        .exit_code = exit_code
    };
}

inline bool delete_file(const std::filesystem::path& path)
{
    std::error_code error;
    const bool removed = std::filesystem::remove(path, error);
    return removed && !error;
}

inline bool make_executable(const std::filesystem::path& path)
{
#ifdef _WIN32
    return true;
#else
    std::error_code error;
    std::filesystem::permissions(
        path,
        std::filesystem::perms::owner_exec,
        std::filesystem::perm_options::add,
        error);
    return !error;
#endif
}

inline bool write_file(
    const std::filesystem::path& path,
    std::string_view content)
{
    std::ofstream file(path, std::ios::binary);

    if (!file)
        return false;

    file.write(content.data(),
               static_cast<std::streamsize>(content.size()));

    return file.good();
}

#undef VEXA_POPEN
#undef VEXA_PCLOSE