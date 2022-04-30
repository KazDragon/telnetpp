cmake_minimum_required(VERSION 3.2 FATAL_ERROR)
cmake_policy(VERSION 3.13)

option(TELNETPP_WITH_ZLIB "Build using ZLib" False)
option(TELNETPP_COVERAGE  "Build with code coverage options")
option(TELNETPP_SANITIZE "Build using sanitizers" "")
option(TELNETPP_WITH_TESTS "Build with tests" True)
option(TELNETPP_DOC_ONLY "Build only documentation" False)

message("Building Telnet++ with build type: ${CMAKE_BUILD_TYPE}")
message("Building Telnet++ with zlib: ${TELNETPP_WITH_ZLIB}")
message("Building Telnet++ with code coverage: ${TELNETPP_COVERAGE}")
message("Building Telnet++ with sanitizers: ${TELNETPP_SANITIZE}")
message("Building Telnet++ with tests: ${TELNETPP_WITH_TESTS}")
message("Building Telnet++ with only documentation: ${TELNETPP_DOC_ONLY}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

if (NOT TELNETPP_VERSION)
    include(function-git_version)
    git_version(TELNETPP)
endif()

message(STATUS "Telnet++ Version: ${TELNETPP_VERSION}")
project(TELNETPP VERSION ${TELNETPP_VERSION})

# For producing automatically-generated documentation, we use Doxygen.
find_package(Doxygen)

# Add a rule for generating documentation
if (DOXYGEN_FOUND)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        @ONLY)

    add_custom_target(telnetpp_doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT
            "Generate API documentation with Doxygen" VERBATIM
    )
endif()

if (TELNETPP_DOC_ONLY)
    return()
endif()

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    message("Using Conan due to conanbuildinfo.cmake")
    set(TELNETPP_USE_CONAN ON)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS NO_OUTPUT_DIRS)
endif()

if (TELNETPP_USE_CONAN)
    set(TELNETPP_PUBLIC_LIBRARIES
        CONAN_PKG::boost
        CONAN_PKG::gsl-lite
    )

    set(TELNETPP_PRIVATE_LIBRARIES
    )

    if (${TELNETPP_WITH_ZLIB})
        set(TELNETPP_PRIVATE_LIBRARIES
            ${TELNETPP_PRIVATE_LIBRARIES}
            CONAN_PKG::zlib
        )
    endif()

    set(TELNETPP_TESTER_LIBRARIES
        telnetpp
        CONAN_PKG::gtest
    )
else()
    # Telnet++ requires at least Boost 1.69.
    find_package(Boost 1.69.0 REQUIRED COMPONENTS container)

    # Telnet++ requires at least version 0.38 of gsl-lite.
    find_package(gsl-lite 0.38.0 REQUIRED)

    # If we are building with ZLib, then we require the ZLib library
    if (${TELNETPP_WITH_ZLIB})
        find_package(ZLIB REQUIRED)
    endif()

    # If we are building with tests, then we require the GTest library
    if (${TELNETPP_WITH_TESTS})
        find_package(GTest REQUIRED)
    endif()

    set(TELNETPP_PUBLIC_LIBRARIES
        Boost::boost
        Boost::container
        gsl::gsl-lite
    )

    if (${TELNETPP_WITH_ZLIB})
        set(TELNETPP_PUBLIC_LIBRARIES
            ${TELNETPP_PUBLIC_LIBRARIES}
            ZLIB::ZLIB
        )
    endif()

    if (${TELNETPP_WITH_TESTS})
        set(TELNETPP_TESTER_LIBRARIES
            telnetpp
            GTest::GTest
            GTest::Main
        )
    endif()
endif()

# When building shared objects, etc., we only want to export certain symbols.
# Therefore, we need to generate a header suitable for declaring which
# symbols should be included.
include(GenerateExportHeader)

if (TELNETPP_SANITIZE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=${TELNETPP_SANITIZE}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=${TELNETPP_SANITIZE}")
endif()

if (TELNETPP_COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -g -O0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

add_library(telnetpp)
add_library(KazDragon::telnetpp ALIAS telnetpp)

# The required C++ Standard for Telnet++ is C++14.
target_compile_features(telnetpp PUBLIC cxx_std_14)

target_sources(telnetpp
    PRIVATE
        include/telnetpp/client_option.hpp
        include/telnetpp/command.hpp
        include/telnetpp/core.hpp
        include/telnetpp/element.hpp
        include/telnetpp/generator.hpp
        include/telnetpp/negotiation.hpp
        include/telnetpp/option.hpp
        include/telnetpp/parser.hpp
        include/telnetpp/server_option.hpp
        include/telnetpp/session.hpp
        include/telnetpp/subnegotiation.hpp
        include/telnetpp/telnetpp.hpp
        include/telnetpp/version.hpp
        include/telnetpp/options/basic_client.hpp
        include/telnetpp/options/basic_server.hpp
        include/telnetpp/options/echo/client.hpp
        include/telnetpp/options/echo/server.hpp
        include/telnetpp/options/mccp/client.hpp
        include/telnetpp/options/mccp/codec.hpp
        include/telnetpp/options/mccp/server.hpp
        include/telnetpp/options/msdp/client.hpp
        include/telnetpp/options/msdp/server.hpp
        include/telnetpp/options/msdp/variable.hpp
        include/telnetpp/options/naws/client.hpp
        include/telnetpp/options/naws/server.hpp
        include/telnetpp/options/new_environ/client.hpp
        include/telnetpp/options/new_environ/server.hpp
        include/telnetpp/options/terminal_type/client.hpp
        include/telnetpp/options/suppress_ga/client.hpp
        include/telnetpp/options/suppress_ga/server.hpp

        include/telnetpp/detail/command_router.hpp
        include/telnetpp/detail/export.hpp
        include/telnetpp/detail/generate_helper.hpp
        include/telnetpp/detail/lambda_visitor.hpp
        include/telnetpp/detail/negotiation_router.hpp
        include/telnetpp/detail/registration.hpp
        include/telnetpp/detail/return_default.hpp
        include/telnetpp/detail/router.hpp
        include/telnetpp/detail/subnegotiation_router.hpp
        include/telnetpp/options/echo/detail/protocol.hpp
        include/telnetpp/options/mccp/detail/protocol.hpp
        include/telnetpp/options/msdp/detail/decoder.hpp
        include/telnetpp/options/msdp/detail/encoder.hpp
        include/telnetpp/options/msdp/detail/protocol.hpp
        include/telnetpp/options/naws/detail/protocol.hpp
        include/telnetpp/options/new_environ/detail/protocol.hpp
        include/telnetpp/options/new_environ/detail/for_each_request.hpp
        include/telnetpp/options/new_environ/detail/for_each_response.hpp
        include/telnetpp/options/new_environ/detail/request_parser_helper.hpp
        include/telnetpp/options/new_environ/detail/response_parser_helper.hpp
        include/telnetpp/options/new_environ/detail/stream.hpp
        include/telnetpp/options/suppress_ga/detail/protocol.hpp
    
        src/command.cpp
        src/element.cpp
        src/negotiation.cpp
        src/options/echo/client.cpp
        src/options/echo/server.cpp
        src/options/mccp/client.cpp
        src/options/mccp/codec.cpp
        src/options/mccp/server.cpp
        src/options/msdp/client.cpp
        src/options/msdp/server.cpp
        src/options/msdp/variable.cpp
        src/options/naws/client.cpp
        src/options/naws/server.cpp
        src/options/new_environ/client.cpp
        src/options/new_environ/server.cpp
        src/options/suppress_ga/client.cpp
        src/options/suppress_ga/server.cpp
        src/options/terminal_type/client.cpp
        src/session.cpp
        src/subnegotiation.cpp

        src/options/msdp/detail/decoder.cpp
        src/options/msdp/detail/encoder.cpp
        src/options/new_environ/detail/stream.cpp
        src/detail/registration.cpp
)

# The zlib compressors for MCCP should only be compiled into the library
# if zlib is available.
if (TELNETPP_WITH_ZLIB)
    target_sources(telnetpp
        PRIVATE
            include/telnetpp/options/mccp/zlib/compressor.hpp
            include/telnetpp/options/mccp/zlib/decompressor.hpp
            src/options/mccp/zlib/compressor.cpp
            src/options/mccp/zlib/decompressor.cpp
    )
endif()

target_link_libraries(telnetpp
    PUBLIC
        ${TELNETPP_PUBLIC_LIBRARIES}
    PRIVATE
        ${TELNETPP_PRIVATE_LIBRARIES}
)

set_target_properties(telnetpp
    PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VERSION ${TELNETPP_VERSION}
        SOVERSION ${TELNETPP_VERSION}
        DEBUG_POSTFIX "d"
)

target_compile_options(telnetpp
    PRIVATE
        # Do not generate warning C4251 (member needs dll linkage) on MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
        # Do not generate warning C4275 (type needs dll linkage) on MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/wd4275>
        # Any warning on MSVC is an error
        $<$<CXX_COMPILER_ID:MSVC>:/WX>

        # Add warnings on g++ and Clang
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wpedantic -Wsign-conversion -Werror>
)

target_include_directories(telnetpp
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/telnetpp-${TELNETPP_VERSION}>
)

generate_export_header(telnetpp
    EXPORT_FILE_NAME "${PROJECT_SOURCE_DIR}/include/telnetpp/detail/export.hpp"
)

configure_file(
    ${PROJECT_SOURCE_DIR}/include/telnetpp/version.hpp.in
    ${PROJECT_SOURCE_DIR}/include/telnetpp/version.hpp
    @ONLY)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/telnetpp-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/telnetpp-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/telnetpp
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/telnetpp-config-version.cmake"
    VERSION
        "${TELNETPP_VERSION}"
    COMPATIBILITY AnyNewerVersion
)

export(
    TARGETS 
        telnetpp
    NAMESPACE 
        KazDragon::
    FILE ${PROJECT_BINARY_DIR}/telnetpp-targets.cmake
)

install(
    TARGETS
        telnetpp
    EXPORT
        telnetpp-targets
    ARCHIVE DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/telnetpp-${TELNETPP_VERSION}
    RUNTIME DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/telnetpp-${TELNETPP_VERSION}
    LIBRARY DESTINATION
        ${CMAKE_INSTALL_BINDIR}/telnetpp-${TELNETPP_VERSION}
)

install(
    EXPORT
        telnetpp-targets
    NAMESPACE 
        KazDragon::
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/telnetpp-${TELNETPP_VERSION}
)

install(
    DIRECTORY
        include/
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}/telnetpp-${TELNETPP_VERSION}
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/telnetpp-config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/telnetpp-config-version.cmake"
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/telnetpp-${TELNETPP_VERSION}
)

if (TELNETPP_WITH_TESTS)
enable_testing()

add_executable(telnetpp_tester)

target_sources(telnetpp_tester
    PRIVATE
        test/client_option_test.cpp
        test/command_test.cpp
        test/command_router_test.cpp
        test/echo_client_test.cpp
        test/echo_server_test.cpp
        test/element_test.cpp
        test/generator_test.cpp
        test/in_memory_session_test.cpp
        test/mccp_client_test.cpp
        test/mccp_server_test.cpp
        test/msdp_client_test.cpp
        test/msdp_server_test.cpp
        test/naws_client_test.cpp
        test/naws_server_test.cpp
        test/negotiation_test.cpp
        test/new_environ_client_test.cpp
        test/new_environ_server_test.cpp
        test/negotiation_router_test.cpp
        test/parser_test.cpp
        test/q_method_test.cpp
        test/server_option_test.cpp
        test/session_test.cpp
        test/subnegotiation_test.cpp
        test/subnegotiation_router_test.cpp
        test/suppress_ga_client_test.cpp
        test/suppress_ga_server_test.cpp
        test/terminal_type_client_test.cpp
)

# The tests for the zlib compressors for MCCP should only be compiled into
# the executable if zlib is available.
if (TELNETPP_WITH_ZLIB)
    target_sources(telnetpp_tester
        PRIVATE
            test/mccp_zlib_compressor_test.cpp
            test/mccp_zlib_decompressor_test.cpp
    )
endif()

target_compile_options(telnetpp_tester
    PRIVATE
        # Do not generate warning C4251 (member needs dll linkage) on MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/wd4251>
        # Do not generate warning C4275 (type needs dll linkage) on MSVC
        $<$<CXX_COMPILER_ID:MSVC>:/wd4275>
        # Any warning on MSVC is an error
        $<$<CXX_COMPILER_ID:MSVC>:/WX>
)

target_link_libraries(telnetpp_tester
    PRIVATE
        ${TELNETPP_TESTER_LIBRARIES}
)

add_test(telnetpp_test telnetpp_tester)
endif()

# Add customizations for packaging
set(CPACK_PACKAGE_NAME "Telnet++")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Telnet++")
set(CPACK_PACKAGE_VENDOR "Matthew Chaplain")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${TELNETPP_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${TELNETPP_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${TELNETPP_VERSION_PATCH})
include(CPack)
