project(UNO)

cmake_minimum_required(VERSION 3.14)
add_definitions(-std=c++14)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(lib)
add_subdirectory(src)