#pragma once

# include <string>

namespace Tsuka
{
    class Process final
    {
    public:
        Process(std::string &&name);
        void Start(const std::string &args, char **env) const;

    private:
        bool IsPathValid(const std::string &path) const noexcept;
        std::string GetLastError() const noexcept;
        const std::string _name;
        std::string _path;
    };
}