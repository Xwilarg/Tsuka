#include <filesystem>

#include "IO.hpp"

namespace Tsuka
{
    void IO::CreateDirectory(const std::string &path) noexcept
    {
        if (!std::filesystem::exists(path))
            std::filesystem::create_directory(path);
    }
}