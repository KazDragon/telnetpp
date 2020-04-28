project(ROT13_SERVER)

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS NO_OUTPUT_DIRS)

# The required C++ Standard for Server++ is C++14.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (ROT13_SERVER_PUBLIC_SOURCE_FILES
    src/connection.cpp
    src/rot13_server.cpp
)

set (ROT13_SERVER_PUBLIC_HEADER_FILES
    include/connection.hpp
)

add_executable(rot13_server
    ${ROT13_SERVER_PUBLIC_HEADER_FILES}
    ${ROT13_SERVER_PUBLIC_SOURCE_FILES}
)

target_include_directories(rot13_server 
    PRIVATE 
        include
)

target_link_libraries(rot13_server
    CONAN_PKG::serverpp
    CONAN_PKG::telnetpp
    CONAN_PKG::boost_range
)
