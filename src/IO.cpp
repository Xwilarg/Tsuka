#include <filesystem>

#include "IO.hpp"

namespace Tsuka
{
    void IO::CreateDirectory(const std::string &path) noexcept
    {
        std::filesystem::create_directory(path);
    }
}