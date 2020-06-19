#include <iostream>

#include "Process.hpp"

int main(int, char **, char **env)
{
    Tsuka::Process cmake("cmake", true);
    Tsuka::Process git("git");
    try
    {
        cmake.Start("--version", env);
        git.Start("--version", env);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}