#include "telnetpp/options/msdp/server.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(msdp_server_test, send_with_empty_list_sends_nothing)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {},
        server.send({}));
}

TEST(msdp_server_test, send_with_variable_sends_simple_variable)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        {
            telnetpp::subnegotiation{
                telnetpp::options::msdp::option,
                { 1, 'v', 'a', 'r', 2, 'v', 'a', 'l' }
            }
        },
        server.send({ { "var", "val" } }));
}

TEST(msdp_server_test, send_with_array_sends_array_variable)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    auto variable = telnetpp::options::msdp::variable{
        "var",
        { "val0", "val1" }
    };

    expect_elements(
        {
            telnetpp::subnegotiation{
                telnetpp::options::msdp::option,
                { 1, 'v', 'a', 'r',
                  2,
                      5,
                          2, 'v', 'a', 'l', '0',
                          2, 'v', 'a', 'l', '1',
                      6
                }
            }
        },
        server.send({ variable }));
}

TEST(msdp_server_test, send_with_table_sends_table_variable)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    auto variable = telnetpp::options::msdp::variable{
        "var",
        {{
            "tbl",
            { "val0", "val1" }
        }}
    };

    expect_elements(
        {
            telnetpp::subnegotiation{
                telnetpp::options::msdp::option,
                { 1, 'v', 'a', 'r',
                  2,  3,
                          1, 't', 'b', 'l',
                          2, 5,
                                  2, 'v', 'a', 'l', '0',
                                  2, 'v', 'a', 'l', '1',
                             6,
                      4
                }
            }
        },
        server.send({ variable }));
}

TEST(msdp_server_test, send_with_many_items_sends_many_items)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    auto variable0 = telnetpp::options::msdp::variable{"svar", "sval"};

    auto variable1 = telnetpp::options::msdp::variable{
        "avar", { "aval0", "aval1" }
    };

    auto variable2 = telnetpp::options::msdp::variable{
        "var",
        {{
            "tbl",
            { "val0", "val1" }
        }}
    };

    expect_elements(
        {
            telnetpp::subnegotiation{
                telnetpp::options::msdp::option,
                { 1, 's', 'v', 'a', 'r',
                  2, 's', 'v', 'a', 'l',
                  1, 'a', 'v', 'a', 'r',
                  2, 5, 2, 'a', 'v', 'a', 'l', '0',
                        2, 'a', 'v', 'a', 'l', '1',
                     6,
                  1, 'v', 'a', 'r',
                  2,  3,
                          1, 't', 'b', 'l',
                          2, 5,
                                  2, 'v', 'a', 'l', '0',
                                  2, 'v', 'a', 'l', '1',
                             6,
                      4
                }
            }
        },
        server.send({ variable0, variable1, variable2 }));
}

TEST(msdp_server_test, receiving_no_variables_does_nothing)
{
    telnetpp::options::msdp::server server;

    bool called = false;
    server.on_receive.connect(
        [&called](auto const &) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    server.activate();
    server.negotiate(telnetpp::do_);

    server.subnegotiate({});

    ASSERT_FALSE(called);
}

TEST(msdp_server_test, receiving_a_variable_reports_an_array_of_one_variable)
{
    telnetpp::options::msdp::server server;

    std::vector<telnetpp::options::msdp::variable> variables;
    server.on_receive.connect(
        [&variables](auto const &vars) -> std::vector<telnetpp::token>
        {
            variables = vars;
            return {};
        });

    server.activate();
    server.negotiate(telnetpp::do_);

    server.subnegotiate({
        1, 'v', 'a', 'r',
        2, 'v', 'a', 'l'
    });

    telnetpp::options::msdp::variable expected{"var", "val"};

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_test, receiving_two_variables_reports_an_array_of_two_variable)
{
    telnetpp::options::msdp::server server;

    std::vector<telnetpp::options::msdp::variable> variables;
    server.on_receive.connect(
        [&variables](auto const &vars) -> std::vector<telnetpp::token>
        {
            variables = vars;
            return {};
        });

    server.activate();
    server.negotiate(telnetpp::do_);

    server.subnegotiate({
        1, 'v', 'a', 'r', '0',
        2, 'v', 'a', 'l', '0',
        1, 'v', 'a', 'r', '1',
        2, 'v', 'a', 'l', '1',
    });

    telnetpp::options::msdp::variable expected0{"var0", "val0"};
    telnetpp::options::msdp::variable expected1{"var1", "val1"};

    ASSERT_EQ(size_t{2}, variables.size());
    ASSERT_EQ(expected0, variables[0]);
    ASSERT_EQ(expected1, variables[1]);
}
