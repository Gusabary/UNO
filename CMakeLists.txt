# enable x.y.z version format
cmake_policy(SET CMP0048 NEW)
project(UNO VERSION 0.0.1)

cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(lib)
add_subdirectory(src)
add_subdirectory(test)