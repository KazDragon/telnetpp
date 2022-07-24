project(ROT13_SERVER)

cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include (${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS NO_OUTPUT_DIRS)

    set(ROT13_SERVER_PRIVATE_LIBRARIES
        CONAN_PKG::serverpp
        CONAN_PKG::telnetpp
        CONAN_PKG::boost
    )
else()
    find_package(serverpp REQUIRED)
    find_package(telnetpp 3.0.0 REQUIRED)
    find_package(Boost COMPONENTS container REQUIRED)
    find_package(gsl-lite REQUIRED)
    find_package(ZLIB REQUIRED)
    find_package(Threads REQUIRED)

    set(ROT13_SERVER_PRIVATE_LIBRARIES
        KazDragon::serverpp
        KazDragon::telnetpp
        Boost::boost
    )
endif()

# The required C++ Standard for Server++ is C++17.
set(CMAKE_CXX_STANDARD 17)
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
    ${ROT13_SERVER_PRIVATE_LIBRARIES}
)
