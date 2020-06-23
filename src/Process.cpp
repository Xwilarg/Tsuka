#include <iostream>
#include <filesystem>

#ifdef _WIN32
# include <windows.h>
# include <codecvt>
#else
# include <unistd.h>
# include <sys/wait.h>
# include <cstring>
#endif

#include "Process.hpp"

namespace Tsuka
{
    Process::Process(std::string &&name, char **env, bool createNewConsole)
        : _name(std::move(name)), _path(), _createNewConsole(createNewConsole), _versionLength(5), _env(env)
    {
#ifdef _WIN32
        _name += ".exe";
        char* buf = nullptr;
        size_t sz = 0;
        _dupenv_s(&buf, &sz, "PATH");
        std::string path(buf);
        char delimitator = ';';
#else
        std::string path = std::getenv("PATH");
        char delimitator = ':';
#endif
        size_t pos;
        std::string token;
        while ((pos = path.find(delimitator)) != std::string::npos) {
            token = path.substr(0, pos);
            if (IsPathValid(token))
            {
                _path = token;
                return;
            }
            path.erase(0, pos + 1);
        }
        if (IsPathValid(path))
            _path = path;
        else
            throw std::invalid_argument("Invalid file name " + _name);
    }

    std::string Process::GetVersion()
    {
        int tmp;
        std::string version = Start("--version", tmp);
        size_t pos = version.find('\n');
        if (pos != std::string::npos) {
            version = version.substr(0, pos);
        }
        size_t length = _name.length() + _versionLength;
        return version.substr(length);
    }

    std::string Process::Start(const std::vector<std::string> args, int &returnValue) const
    {
#ifdef _WIN32
        return Start(ArrayToString(args, ' '), returnValue);
#else
        std::string output = "";
        int pipefd[2];
        if (::pipe(pipefd) == -1)
        {
            throw std::runtime_error("Error while creating pipe: " + GetLastError());
        }
        std::string fullPath = _path + '/' + _name;
        char appPathChr[fullPath.length() + 1];
        std::strcpy(appPathChr, fullPath.c_str());
        appPathChr[fullPath.length()] = '\0';
        char appNameChr[_name.length() + 1];
        std::strcpy(appNameChr, _name.c_str());
        appNameChr[_name.length()] = '\0';
        char **charArgs = ArrayToCArray(appNameChr, args);
        int forkId = ::fork();
        if (forkId == -1)
        {
            throw std::runtime_error("Error while doing fork: " + GetLastError());
        }
        if (forkId == 0)
        {
            ::close(pipefd[0]);
            ::dup2(pipefd[1], 1);
            ::dup2(pipefd[1], 2);
            ::close(pipefd[1]);
            if (::execve(appPathChr, charArgs, _env) == -1)
            {
                throw std::runtime_error("Error while doing execve: " + GetLastError());
            }
        }
        else
        {
            char buffer[4096];
            ::close(pipefd[1]);
            int size = read(pipefd[0], buffer, sizeof(buffer) - 1);
            while (size != 0)
            {
                buffer[size] = '\0';
                output += std::string(buffer) + '\n';
                size = ::read(pipefd[0], buffer, sizeof(buffer));
            }
            waitpid(forkId, &returnValue, 0);
        }
        for (int i = 0; charArgs[i] != nullptr; i++)
            free(charArgs[i]);
        free(charArgs);
        return output;
#endif
    }

    std::string Process::Start(const std::string &args, int &returnValue) const
    {
#ifdef _WIN32
        std::string output = "";
        HANDLE g_hChildStd_OUT_Rd = nullptr;
        HANDLE g_hChildStd_OUT_Wr = nullptr;

        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = nullptr;
        if (!::CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0)
                || !::SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
        {
            throw std::runtime_error("Error while creating pipe: " + GetLastError());
        }
        PROCESS_INFORMATION piProcInfo;
        STARTUPINFOW siStartInfo = { sizeof(STARTUPINFOW) };
        ::ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
        ::ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
        siStartInfo.hStdError = g_hChildStd_OUT_Wr;
        siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
        siStartInfo.hStdInput = nullptr;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
        std::string fullPath = (std::filesystem::path(_path) / _name).string();
        std::string fullArgs = " " + args;
        if (!::CreateProcessW(std::wstring(fullPath.begin(), fullPath.end()).c_str(),
                            const_cast<LPWSTR>(std::wstring(fullArgs.begin(), fullArgs.end()).c_str()),
                            nullptr,
                            nullptr,
                            TRUE,
                            _createNewConsole ? CREATE_NEW_CONSOLE : 0,
                            nullptr,
                            std::wstring(_path.begin(), _path.end()).c_str(),
                            &siStartInfo,
                            &piProcInfo))
        {
            throw std::runtime_error("Failed to create new process: " + GetLastError());
        }
        DWORD dwRead;
        CHAR chBuf[BUFSIZ];
        BOOL success;
        do
        {
            success = ::ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZ, &dwRead, nullptr);
            if (success && dwRead > 0)
                output += std::string(chBuf) + '\n';
        } while (!success || dwRead == 0);
        DWORD returnValueWin;
        if (!GetExitCodeProcess(piProcInfo.hProcess, &returnValueWin))
        {
            throw std::runtime_error("Failed to get return status: " + GetLastError());
        }
        ::CloseHandle(piProcInfo.hProcess);
        ::CloseHandle(piProcInfo.hThread);
        returnValue = returnValueWin;
        return output;
#else
        return Start(std::vector<std::string>{args}, returnValue);
#endif
    }

    char** Process::ArrayToCArray([[maybe_unused]]const std::string &firstArg, [[maybe_unused]]const std::vector<std::string> &args) const
#ifdef _WIN32
#else
    noexcept
#endif
    {
#ifdef _WIN32
        throw std::runtime_error("ArrayToCArray must not be called on Windows");
#else
        char **arr = (char**)malloc(sizeof(char*) * (args.size() + 2));
        arr[0] = (char*)malloc(sizeof(char) * (firstArg.length() + 1));
        std::strcpy(arr[0], firstArg.c_str());
        for (size_t i = 0; i < args.size(); i++)
        {
            arr[i + 1] = (char*)malloc(sizeof(char) * (args[i].length() + 1));
            std::strcpy(arr[i + 1], args[i].c_str());
        }
        arr[args.size() + 1] = nullptr;
        return arr;
#endif
    }

    // args length must be > 0
    std::string Process::ArrayToString(const std::vector<std::string> args, char separator) const noexcept
    {
        std::string res = args[0];
        for (size_t i = 1; i < args.size(); i++)
        {
            res += separator + args[i];
        }
        return res;
    }

    bool Process::IsPathValid(const std::string &path) const noexcept
    {
        return std::filesystem::exists(std::filesystem::path(path) / _name);
    }

    std::string Process::GetLastError() const noexcept
    {
#ifdef _WIN32
        wchar_t buf[256];
        ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       buf, (sizeof(buf) / sizeof(wchar_t)), nullptr);
        std::wstring str(buf);
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> conv;
        return conv.to_bytes(str);
#else
        return strerror(errno);
#endif
    }
}