# enable x.y version format
cmake_policy(SET CMP0048 NEW)
project(UNO VERSION 1.0)

cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# set(CMAKE_BUILD_TYPE Debug)

option(BUILD_TEST "build tests" OFF)
option(ENABLE_LOG "enable spdlog" OFF)

# build lib before adding ENABLE_LOG definition to avoid recompiling lib when toggling that option
add_subdirectory(lib)

if(ENABLE_LOG)
    add_compile_definitions("ENABLE_LOG")
    # add_definitions(-DENABLE_LOG)
endif()

add_subdirectory(src)

if(BUILD_TEST)
    add_subdirectory(test)
endif()

install(TARGETS uno RUNTIME DESTINATION bin)
