#pragma once

# include <string>

namespace Tsuka
{
    class IO
    {
    public:
        static void CreateDirectory(const std::string &path) noexcept;
    };
}