set(ASIO_CMAKE_ASIO_TAG asio-1-18-0)
set(ASIO_CMAKE_ASIO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/build)
set(ASIO_CMAKE_ASIO_DEP_DIR "${ASIO_CMAKE_ASIO_SOURCE_DIR}/asio-${ASIO_CMAKE_ASIO_TAG}-src")

include(FetchContent)
FetchContent_Declare(asio
  GIT_REPOSITORY  https://github.com/chriskohlhoff/asio.git
  GIT_TAG         ${ASIO_CMAKE_ASIO_TAG}
  GIT_SHALLOW     TRUE  # shallow clone
  SOURCE_DIR      ${ASIO_CMAKE_ASIO_DEP_DIR}
)
FetchContent_MakeAvailable(asio)

add_library(asio INTERFACE)
target_include_directories(asio INTERFACE ${asio_SOURCE_DIR}/asio/include)
target_compile_definitions(asio INTERFACE
    ASIO_STANDALONE     # don't use Boost
    ASIO_NO_DEPRECATED
)

if(UNIX)
    target_link_libraries(asio INTERFACE pthread)
elseif(WIN32)
    # macro see @ https://stackoverflow.com/a/40217291/1746503
    macro(get_WIN32_WINNT version)
        if (CMAKE_SYSTEM_VERSION)
        set(ver ${CMAKE_SYSTEM_VERSION})
        string(REGEX MATCH "^([0-9]+).([0-9])" ver ${ver})
        string(REGEX MATCH "^([0-9]+)" verMajor ${ver})
        # Check for Windows 10, b/c we'll need to convert to hex 'A'.
        if ("${verMajor}" MATCHES "10")
            set(verMajor "A")
            string(REGEX REPLACE "^([0-9]+)" ${verMajor} ver ${ver})
        endif ("${verMajor}" MATCHES "10")
        # Remove all remaining '.' characters.
        string(REPLACE "." "" ver ${ver})
        # Prepend each digit with a zero.
        string(REGEX REPLACE "([0-9A-Z])" "0\\1" ver ${ver})
        set(${version} "0x${ver}")
        endif(CMAKE_SYSTEM_VERSION)
    endmacro(get_WIN32_WINNT)

    if(NOT DEFINED _WIN32_WINNT)
        get_WIN32_WINNT(ver)
        set(_WIN32_WINNT ${ver})
    endif()

    message(STATUS "Set _WIN32_WINNET=${_WIN32_WINNT}")

    target_compile_definitions(asio
        INTERFACE
        _WIN32_WINNT=${_WIN32_WINNT}
        WIN32_LEAN_AND_MEAN
    )
endif()

add_subdirectory(cxxopts)
add_subdirectory(yaml-cpp)

if(BUILD_TEST)
  add_subdirectory(googletest)
endif()

if(ENABLE_LOG)
  add_subdirectory(spdlog)
endif()