#include <iostream>

#include "Process.hpp"

int main()
{
    Tsuka::Process cmake("cmake.exe");
    try
    {
        cmake.Start("-v");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}