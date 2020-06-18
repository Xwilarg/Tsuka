#pragma once

# include <string>

namespace Tsuka
{
    class Process final
    {
    public:
        Process(std::string &&name) noexcept;
        void Start(const std::string &args) const;

    private:
        std::string GetLastError() const noexcept;
        const std::string _name;
    };
}