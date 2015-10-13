project(telnetpp)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

# Boost is required as we used Boost.Variant, Boost.Optional, and 
# Boost.Signals2.  All of these are header-only libraries, so no subcomponents
# are required.
find_package(Boost 1.39.0 REQUIRED)

# Optionally, Telnet++ support CppUnit for its unit tests.  These will be built
# and a target added automatically if it is installed in the current 
# environment.
find_package(CppUnit)

add_library(telnetpp
    src/client_option.cpp
    src/command.cpp
    src/detail/generate_helper.cpp
    src/detail/parse_helper.cpp
    src/negotiation.cpp
    src/options/echo/client.cpp
    src/options/echo/server.cpp
    src/options/naws/client.cpp
    src/options/naws/server.cpp
    src/options/terminal_type/client.cpp
    src/options/suppress_ga/client.cpp
    src/options/suppress_ga/server.cpp
    src/routing_visitor.cpp
    src/server_option.cpp
    src/subnegotiation.cpp
)

target_include_directories(telnetpp
    PUBLIC "${PROJECT_SOURCE_DIR}/include"
)

target_compile_features(telnetpp
    PUBLIC
        cxx_strong_enums
        cxx_relaxed_constexpr
)

enable_testing()

add_executable(telnetpp_tester 
    test/command_router_test.cpp
    test/client_option_test.cpp
    test/echo_client_test.cpp
    test/echo_server_test.cpp
    test/expect_elements.cpp
    test/generator_test.cpp
    test/main.cpp
    test/naws_client_test.cpp
    test/naws_server_test.cpp    
    test/negotiation_router_test.cpp
    test/parser_test.cpp
#    test/routing_visitor_test.cpp
    test/server_option_test.cpp
    test/subnegotiation_router_test.cpp
    test/suppress_ga_client_test.cpp
    test/suppress_ga_server_test.cpp
    test/terminal_type_client_test.cpp
)

target_compile_features(telnetpp_tester
    PUBLIC
        cxx_strong_enums
        cxx_relaxed_constexpr)

target_link_libraries(telnetpp_tester telnetpp cppunit)

add_test(telnetpp_test telnetpp_tester)

install(
    TARGETS
        telnetpp
    ARCHIVE DESTINATION
        lib
)

install(
    DIRECTORY
        include/
    DESTINATION
        include
)
