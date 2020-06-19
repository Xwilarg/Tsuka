#include <iostream>
#include <filesystem>

#ifdef _WIN32
# include <windows.h>
#else
# include <unistd.h>
# include <cstring>
#endif

#include "Process.hpp"

namespace Tsuka
{
    Process::Process(std::string &&name)
        : _name(std::move(name)), _path()
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

    bool Process::IsPathValid(const std::string &path) const noexcept
    {
        return std::filesystem::exists(std::filesystem::path(path) / _name);
    }

    void Process::Start(const std::string &args, char **env) const
    {
#ifdef _WIN32
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
        STARTUPINFO siStartInfo = { sizeof(STARTUPINFO) };
        ::ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
        ::ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
        siStartInfo.hStdError = g_hChildStd_OUT_Wr;
        siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
        siStartInfo.hStdInput = nullptr;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
        std::string fullPath = (std::filesystem::path(_path) / _name).string();
        if (!::CreateProcessA(fullPath.c_str(),
                            const_cast<char*>(args.c_str()),
                            nullptr,
                            nullptr,
                            TRUE,
                            0,
                            nullptr,
                            const_cast<char*>(_path.c_str()),
                            &siStartInfo,
                            &piProcInfo))
        {
            throw std::runtime_error("Failed to create new process: " + GetLastError());
        }
        ::CloseHandle(piProcInfo.hProcess);
        ::CloseHandle(piProcInfo.hThread);
        DWORD dwRead;
        CHAR chBuf[4096];
        BOOL success;
        do
        {
            success = ::ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZ, &dwRead, nullptr);
            if (success && dwRead > 0)
                std::cout << std::string(chBuf) << std::endl;
        } while (!success || dwRead == 0);
#else
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
        char appArgsChr[args.length() + 1];
        std::strcpy(appArgsChr, args.c_str());
        appArgsChr[args.length()] = '\0';
        char* charArgs[] = { appNameChr, appArgsChr, NULL };
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
            if (::execve(appPathChr, charArgs, env) == -1)
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
                std::cout << std::string(buffer) << std::endl;
                size = ::read(pipefd[0], buffer, sizeof(buffer));
            }
        }
#endif
    }

    std::string Process::GetLastError() const noexcept
    {
#ifdef _WIN32
        wchar_t buf[256];
        ::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       nullptr, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       buf, (sizeof(buf) / sizeof(wchar_t)), nullptr);
        std::wstring str(buf);
        return std::string(str.begin(), str.end());
#else
        return strerror(errno);
#endif
    }
}