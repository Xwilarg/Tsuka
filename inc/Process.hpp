#pragma once

# include <string>

namespace Tsuka
{
    class Process final
    {
    public:
        Process(std::string &&name, char **env, bool createNewConsole = false);
        std::string Start(const std::string &args, int &returnValue) const;
        std::string GetVersion();

    private:
        bool IsPathValid(const std::string &path) const noexcept;
        std::string GetLastError() const noexcept;
        std::string _name;
        std::string _path;
        bool _createNewConsole;
        const size_t _versionLength;
        char **_env;
    };
}