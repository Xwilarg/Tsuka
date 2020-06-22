#pragma once

# include <string>

# ifdef _WIN32
# else
    # include <vector>
# endif

namespace Tsuka
{
    class Process final
    {
    public:
        Process(std::string &&name, char **env, bool createNewConsole = false);
        std::string Start(const std::vector<std::string> args, int &returnValue) const;
        std::string Start(const std::string &args, int &returnValue) const;
        std::string GetVersion();

    private:
        std::string ArrayToString(const std::vector<std::string> args, char separator) const noexcept;
        char **ArrayToCArray(const std::string &firstArg, const std::vector<std::string> &args) const
#ifdef _WIN32
#else
    noexcept
#endif
;
        bool IsPathValid(const std::string &path) const noexcept;
        std::string GetLastError() const noexcept;
        std::string _name;
        std::string _path;
        bool _createNewConsole;
        const size_t _versionLength;
        char **_env;
    };
}