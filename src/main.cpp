#include <iostream>

#include "Process.hpp"
#include "IO.hpp"

int main(int, char **, char **env)
{
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
        std::exit(1);
    }
    Tsuka::IO::CreateDirectory("Tsuka");
    Tsuka::IO::SetCurrentDirectory("Tsuka");
    return 0;
}