#include <iostream>
#include <regex>

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
    Tsuka::Process git("git", env);
    std::string url = std::string(argv[1]);

    // Check is remote URL is valid
    int status;
    git.Start("ls-remote " + url, status);
    if (status != 0)
    {
        std::cerr << "Invalid URL " << url << std::endl;
    }

    // Check if the user have Git and CMake
    Tsuka::Process cmake("cmake", env, true);
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

    // Create build directory
    Tsuka::IO::CreateDirectory("Tsuka");
    Tsuka::IO::SetCurrentDirectory("Tsuka");

    std::regex repoNameRegex("github.com\\/[^\\/]+\\/([^\\/]+)");
    std::smatch matches;
    std::regex_search(url, matches, repoNameRegex); // Is not supposed to fail
    std::string repoName = matches[1];
    if (!Tsuka::IO::CreateDirectory(repoName))
    {
        std::cout << std::endl << "Cloning " << repoName << "...";
        Tsuka::IO::CreateDirectory(repoName);
        git.Start("clone " + url, status);
        std::cout << " Done!" << std::endl;
    }
    return 0;
}