project(telnetpp)
cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.2)

if (POLICY CMP0063)
    cmake_policy(SET CMP0063 OLD) # Do not allow hidden visibility for static libs
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

# ZLib is required for the MCCP options.
include(FindZLIB)
find_package(ZLIB REQUIRED)

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

if (GTEST_FOUND)
    # For GTest, it is necessary to link with the appropriate local threading
    # library.
    find_package(Threads REQUIRED)
endif()

set (telnetpp_public_src
    src/byte_converter.cpp
    src/client_option.cpp
    src/command.cpp
    src/negotiation.cpp
    src/options/mccp/client.cpp
    src/options/mccp/codec.cpp
    src/options/mccp/server.cpp
    src/options/mccp/zlib/compressor.cpp
    src/options/mccp/zlib/decompressor.cpp
    src/options/msdp/client.cpp
    src/options/msdp/server.cpp
    src/options/msdp/variable.cpp
    src/options/naws/client.cpp
    src/options/naws/server.cpp
    src/options/new_environ/client.cpp
    src/options/new_environ/server.cpp
    src/options/terminal_type/client.cpp
    src/server_option.cpp
    src/session.cpp
    src/subnegotiation.cpp
)

set (telnetpp_private_src
    src/options/msdp/detail/decoder.cpp
    src/options/msdp/detail/encoder.cpp
    src/options/new_environ/detail/request_parser.cpp
    src/options/new_environ/detail/response_parser.cpp
    src/options/new_environ/detail/stream.cpp
    src/detail/generate_helper.cpp
    src/detail/parse_helper.cpp
    src/detail/registration.cpp
    src/detail/routing_visitor.cpp
)

add_library(telnetpp
    ${telnetpp_public_src}
    ${telnetpp_private_src}
)

target_link_libraries(telnetpp
    PRIVATE
        ${ZLIB_LIBRARIES}
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
        cxx_auto_type
        cxx_generic_lambdas
        cxx_strong_enums
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
        test/byte_converter_test.cpp
        test/client_option_test.cpp
        test/command_router_test.cpp
        test/echo_client_test.cpp
        test/echo_server_test.cpp
        test/mccp_client_test.cpp
        test/mccp_server_test.cpp
        test/mccp_zlib_compressor_test.cpp
        test/mccp_zlib_decompressor_test.cpp
        test/msdp_client_test.cpp
        test/msdp_server_test.cpp
        test/msdp_server_robustness_test.cpp
        test/naws_client_test.cpp
        test/naws_server_test.cpp
        test/new_environ_client_test.cpp
        test/new_environ_server_test.cpp
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
        ${ZLIB_LIBRARIES}
    )

    add_test(telnetpp_test telnetpp_tester)
endif()
