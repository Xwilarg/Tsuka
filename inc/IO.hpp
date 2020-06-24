#pragma once

# include <string>

namespace Tsuka
{
    class IO
    {
    public:
        static bool CreateDirectory(const std::string &path) noexcept;
        static void DeleteDirectory(const std::string &path) noexcept;
        static void SetCurrentDirectory(const std::string &path) noexcept;
    };
}