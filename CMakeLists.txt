cmake_minimum_required(VERSION 2.8)
project(Tsuka)

if(MSVC)
    SET(CMAKE_CXX_FLAGS "/W4 /EHsc /std:c++17")
else()
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra -std=c++17")
endif()

file(GLOB_RECURSE SRCS "src/*.cpp")
include_directories(inc)

add_executable(${PROJECT_NAME} ${SRCS})