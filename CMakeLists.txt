project(telnetpp)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 NEW) # Allow hidden visibility for static libs
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake/Modules")

# Boost is required as we used Boost.Variant, Boost.Optional, and 
# Boost.Signals2.  All of these are header-only libraries, so no subcomponents
# are required.
include(FindBoost)
find_package(Boost 1.39.0 REQUIRED)

# When building shared objects, etc., we only want to export certain symbols.
# Therefore, we need to generate a header suitable for declaring which
# symbols should be included.
include(GenerateExportHeader)

# Due to the strange requirements for Google Test, it is assumed to have been
# built in the "gtest" directory.  For this, it is required to run the moral
# equivalent of the following script before building Telnet++:
#
#  mkdir gtest
#  cd gtest
#  cmake /usr/src/gtest && make
#  export GTEST_ROOT=$PWD
#  cd ..
find_package(GTest)

# For GTest, it is necessary to link with the appropriate local threading 
# library.
find_package(Threads)

set (telnetpp_public_src
    src/client_option.cpp
    src/command.cpp
    src/negotiation.cpp
    src/options/echo/client.cpp
    src/options/echo/server.cpp
    src/options/naws/client.cpp
    src/options/naws/server.cpp
    src/options/terminal_type/client.cpp
    src/options/suppress_ga/client.cpp
    src/options/suppress_ga/server.cpp
    src/server_option.cpp
    src/session.cpp
    src/subnegotiation.cpp
)

set (telnetpp_private_src
    src/detail/generate_helper.cpp
    src/detail/parse_helper.cpp
    src/detail/registration.cpp
    src/detail/routing_visitor.cpp
)

add_library(telnetpp
    ${telnetpp_public_src}
    ${telnetpp_private_src}
)

set_target_properties(telnetpp
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VERSION 1.0
        SOVERSION 1.0
)

target_include_directories(telnetpp
    PUBLIC 
        "${PROJECT_SOURCE_DIR}/include"
        "${Boost_INCLUDE_DIRS}"   
)

target_compile_features(telnetpp
    PUBLIC
        cxx_strong_enums
        cxx_relaxed_constexpr
)

generate_export_header(telnetpp
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/include/telnetpp/detail/export.hpp"
)

install(
    TARGETS
        telnetpp
    ARCHIVE DESTINATION
        lib
    LIBRARY DESTINATION
        lib
)

install(
    DIRECTORY
        include/
    DESTINATION
        include
)

if (GTEST_FOUND)
    enable_testing()

    set (telnetpp_tester_external_tests
        test/client_option_test.cpp
        test/command_router_test.cpp
        test/echo_client_test.cpp
        test/echo_server_test.cpp
        test/naws_client_test.cpp
        test/naws_server_test.cpp
        test/negotiation_router_test.cpp
        test/pass_through_test.cpp
        test/server_option_test.cpp
        test/session_test.cpp
        test/subnegotiation_router_test.cpp
        test/suppress_ga_client_test.cpp
        test/suppress_ga_server_test.cpp
        test/terminal_type_client_test.cpp
    )

    set (telnetpp_tester_internal_tests
        test/parser_test.cpp
        test/generator_test.cpp
        test/routing_visitor_test.cpp
    )

    # If we are building shared libraries, then we wont be able to test any of
    # the modules in the detail directory since they're not visible (this is ok,
    # since all those are tested indirectly via other parts of the API anyway.)
    # For a normal archive, all symbols are visible, so they can be tested directly.
    if (BUILD_SHARED_LIBS)
        set(telnetpp_tester_tests
            ${telnetpp_tester_external_tests}
        )
    else()
        set(telnetpp_tester_tests
            ${telnetpp_tester_internal_tests}
            ${telnetpp_tester_external_tests}
        )
    endif()

    add_executable(telnetpp_tester 
        ${telnetpp_tester_tests}
        test/expect_elements.cpp
    )

    target_compile_features(telnetpp_tester
        PUBLIC
            cxx_auto_type
            cxx_generic_lambdas
            cxx_strong_enums
            cxx_uniform_initialization
    )

    target_link_libraries(telnetpp_tester 
        telnetpp 
        ${GTEST_LIBRARY}
        ${GTEST_MAIN_LIBRARY}
        ${CMAKE_THREAD_LIBS_INIT}
    )

    add_test(telnetpp_test telnetpp_tester)
endif()
