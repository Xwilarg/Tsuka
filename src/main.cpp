#include <iostream>

#include "Process.hpp"
#include "IO.hpp"

std::string GetHelp()
{
    return "TODO";
}

int main(int argc, char **argv, char **env)
{
    if (argc <= 1)
    {
        std::cout << GetHelp() << std::endl;
        return 1;
    }
    Tsuka::Process cmake("cmake", env, true);
    Tsuka::Process git("git", env);
    try
    {
        std::cout << "CMake version: " << cmake.GetVersion() << std::endl;
        std::cout << "Git version: " << git.GetVersion() << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error while checking for versions: " << e.what() << std::endl;
        return 1;
    }
    std::cout << std::endl;
    Tsuka::IO::CreateDirectory("Tsuka");
    Tsuka::IO::SetCurrentDirectory("Tsuka");
    int status;
    git.Start("ls-remote " + std::string(argv[1]), status);
    if (status != 0)
    {
        std::cerr << "Invalid URL " << argv[1] << std::endl;
    }
    return 0;
}