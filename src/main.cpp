#include <iostream>

#include "Process.hpp"

int main(int, char **, char **env)
{
    Tsuka::Process cmake("cmake");
    try
    {
        cmake.Start("--version", env);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}