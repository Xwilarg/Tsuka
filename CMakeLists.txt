cmake_minimum_required(VERSION 2.8)
project(Tsuka)

if(MSVC)
    SET(CMAKE_CXX_FLAGS "/W4 /EHsc")
else()
    SET(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif()

file(GLOB_RECURSE SRCS "src/*.cpp")
include_directories(inc)

add_executable(${PROJECT_NAME} ${SRCS})