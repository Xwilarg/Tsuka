#pragma once

# include <string>

namespace Tsuka
{
    class Process final
    {
    public:
        Process(std::string &&name, bool createNewConsole = false);
        void Start(const std::string &args, char **env) const;

    private:
        bool IsPathValid(const std::string &path) const noexcept;
        std::string GetLastError() const noexcept;
        std::string _name;
        std::string _path;
        bool _createNewConsole;
    };
}