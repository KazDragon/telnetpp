#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(new_environ_server_test, option_is_new_environ)
{
    telnetpp::options::new_environ::server server;
    ASSERT_EQ(39, server.option());
}

TEST(new_environ_server_test, receiving_request_with_no_content_and_no_cached_variables_returns_empty_list)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(server.option(), { 0x00 })
        },
        server.subnegotiate({0x01}));
}

TEST(new_environ_server_test, receiving_request_with_no_content_and_one_cached_variable_returns_variable)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("USER", "TEST");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x00,
                  0x00, 'U', 'S', 'E', 'R',
                  0x01, 'T', 'E', 'S', 'T'
                })
        },
        server.subnegotiate({0x01}));
}

TEST(new_environ_server_test, receiving_request_with_no_content_and_one_cached_user_variable_returns_all_variables)
{
    telnetpp::options::new_environ::server server;
    server.set_user_variable("TEST", "VALUE");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x00,
                  0x03, 'T', 'E', 'S', 'T',
                  0x01, 'V', 'A', 'L', 'U', 'E'
                })
        },
        server.subnegotiate({0x01}));
}

TEST(new_environ_server_test, receiving_request_with_no_content_and_one_cached_variable_with_empty_value_returns_all_variables)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("USER", "");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x00,
                  0x00, 'U', 'S', 'E', 'R',
                  0x01
                })
        },
        server.subnegotiate({0x01}));
}

TEST(new_environ_server_test, receiving_request_with_no_content_and_one_deleted_variable_returns_no_variables)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("USER", "TEST");
    server.delete_variable("USER");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x00,
                })
        },
        server.subnegotiate({0x01}));
}

TEST(new_environ_server_test, receiving_request_with_no_content_and_one_deleted_uservariable_returns_no_variables)
{
    telnetpp::options::new_environ::server server;
    server.set_user_variable("TTYPE", "XTERM");
    server.delete_user_variable("TTYPE");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x00,
                })
        },
        server.subnegotiate({0x01}));
}

TEST(new_environ_server_test, receiving_request_with_no_content_and_multiple_cached_variables_returns_all_variables)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("PRINTER", "LPT1");
    server.set_variable("USER", "TEST");
    server.set_user_variable("LOCALE", "EN_UK");
    server.set_user_variable("TTYPE", "XTERM");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x00,
                  0x00, 'P', 'R', 'I', 'N', 'T', 'E', 'R',
                  0x01, 'L', 'P', 'T', '1',
                  0x00, 'U', 'S', 'E', 'R',
                  0x01, 'T', 'E', 'S', 'T',
                  0x03, 'L', 'O', 'C', 'A', 'L', 'E',
                  0x01, 'E', 'N', '_', 'U', 'K',
                  0x03, 'T', 'T', 'Y', 'P', 'E',
                  0x01, 'X', 'T', 'E', 'R', 'M'
                })
        },
        server.subnegotiate({0x01}));
}

TEST(new_environ_server_test, modifying_a_variable_with_active_server_returns_variable_modification)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("USER", "TEST");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x02,
                  0x00, 'U', 'S', 'E', 'R',
                  0x01, 'F', 'R', 'E', 'D'
                })
        },
        server.set_variable("USER", "FRED"));
}

TEST(new_environ_server_test, deleting_a_variable_with_active_server_returns_variable_deletion)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("USER", "TEST");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x02,
                  0x00, 'U', 'S', 'E', 'R'
                })
        },
        server.delete_variable("USER"));
}

TEST(new_environ_server_test, modifying_a_user_variable_with_active_server_returns_variable_modification)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("TEST", "VALUE");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x02,
                  0x03, 'T', 'E', 'S', 'T',
                  0x01, 'R', 'E', 'S', 'U', 'L', 'T'
                })
        },
        server.set_user_variable("TEST", "RESULT"));
}

TEST(new_environ_server_test, deleting_a_user_variable_with_active_server_returns_variable_deletion)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("TEST", "VALUE");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x02,
                  0x03, 'T', 'E', 'S', 'T'
                })
        },
        server.delete_user_variable("TEST"));
}

TEST(new_environ_server_test, requesting_nonexistent_variable_rturns_empty_result)
{
    telnetpp::options::new_environ::server server;
    server.set_user_variable("TEST", "VALUE");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x00
                })
        },
        server.subnegotiate({0x01, 0x03, 'F', 'A', 'I', 'L'}));
}

TEST(new_environ_server_test, requesting_existent_variables_returns_variables)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("USER", "TEST");
    server.set_variable("PRINTER", "LPT1");
    server.set_user_variable("TEST", "VALUE");
    server.set_user_variable("FOO", "BAR");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                { 0x00,
                  0x00, 'U', 'S', 'E', 'R',
                  0x01, 'T', 'E', 'S', 'T',
                  0x03, 'T', 'E', 'S', 'T',
                  0x01, 'V', 'A', 'L', 'U', 'E'
                })
        },
        server.subnegotiate(
            { 0x01,
              0x03, 'T', 'E', 'S', 'T',
              0x00, 'U', 'S', 'E', 'R'
            }));
}

TEST(new_environ_server_test, names_and_values_with_control_characters_are_escaped_properly)
{
    telnetpp::options::new_environ::server server;
    server.set_variable("USER", "\x01""ABC");
    server.set_user_variable(std::string("\x00""DEF", 4), "\x02""GHI");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                {
                    0x00,
                    0x00, 'U', 'S', 'E', 'R',
                    0x01, 0x02, 0x01, 'A', 'B', 'C',
                    0x03, 0x02, 0x00, 'D', 'E', 'F',
                    0x01, 0x02, 0x02, 'G', 'H', 'I'
                })
        },
        server.subnegotiate({0x01}));
}

TEST(new_environ_server_test, requests_with_escaped_control_characters_are_handled_correctly)
{
    telnetpp::options::new_environ::server server;
    server.set_user_variable(std::string("\x00\x01\x02\x03", 4), "GOOD");
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation(
                server.option(),
                {
                    0x00,
                    0x03, 0x02, 0x00, 0x02, 0x01, 0x02, 0x02, 0x02, 0x03,
                    0x01, 'G', 'O', 'O', 'D'
                })
        },
        server.subnegotiate(
            { 0x01,
              0x03, 0x02, 0x00, 0x02, 0x01, 0x02, 0x02, 0x02, 0x03
            }));

}