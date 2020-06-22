#include <filesystem>

#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include "IO.hpp"

namespace Tsuka
{
    void IO::CreateDirectory(const std::string &path) noexcept
    {
        if (!std::filesystem::exists(path))
            std::filesystem::create_directory(path);
    }

    void IO::SetCurrentDirectory(const std::string &path) noexcept
    {
#ifdef _WIN32
        ::_chdir(path.c_str());
#else
        ::chdir(path.c_str());
#endif
    }
}