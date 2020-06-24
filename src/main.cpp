#include <iostream>
#include <regex>

#include "Process.hpp"
#include "IO.hpp"

std::string GetHelp()
{
    return "TODO";
}

void DisplayErrorMessage(const std::string &msg)
{
    std::cerr << "\033[0;31m" << msg << "\033[0m" << std::endl;
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
    git.Start({"ls-remote", url}, status);

    if (status != 0)
    {
        DisplayErrorMessage("Invalid URL " + url);
        return 1;
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
        DisplayErrorMessage("Error while checking for versions: " + std::string(e.what()));
        return 1;
    }

    // Create build directory
    Tsuka::IO::CreateDirectory("Tsuka-build");
    Tsuka::IO::SetCurrentDirectory("Tsuka-build");

    std::regex repoNameRegex("github.com\\/[^\\/]+\\/([^\\/]+)");
    std::smatch matches;
    std::regex_search(url, matches, repoNameRegex); // Is not supposed to fail
    std::string repoName = matches[1];
    if (Tsuka::IO::CreateDirectory(repoName))
    {
        std::cout << std::endl << "Cloning " << repoName << "...";
        std::string output = git.Start({"clone", "--recurse-submodules", url}, status);
        if (status != 0)
        {
            Tsuka::IO::DeleteDirectory(repoName);
            DisplayErrorMessage("\nError while cloning repository:\n" + output);
            return 1;
        }
        std::cout << " Done!" << std::endl << std::endl;
    }
    Tsuka::IO::SetCurrentDirectory(repoName);

    Tsuka::IO::CreateDirectory("build");
    Tsuka::IO::SetCurrentDirectory("build");
    std::cout << "Executing CMake...";
    std::string output;
    output = cmake.Start("..", status);
    if (status != 0)
    {
        DisplayErrorMessage("\nError while executing cmake:\n" + output);
        return 1;
    }
    output = cmake.Start({"--build", ".", "--config", "RELEASE"}, status);
    if (status != 0)
    {
        DisplayErrorMessage("\nError while executing cmake:\n" + output);
        return 1;
    }
    std::cout << " Done!" << std::endl << std::endl;
    return 0;
}