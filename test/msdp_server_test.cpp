#include "telnetpp/options/msdp/server.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

namespace {

static void activate_msdp_server(telnetpp::options::msdp::server &server)
{
    server.activate();
    server.negotiate(telnetpp::do_);
}

static void register_msdp_server_variable_reception(
    telnetpp::options::msdp::server &server,
    std::vector<telnetpp::options::msdp::variable> &variables)
{
    server.on_receive.connect(
        [&variables](auto const &vars) -> std::vector<telnetpp::token>
        {
            variables = vars;
            return {};
        });
}

}

TEST(msdp_server_test, option_is_msdp)
{
    telnetpp::options::msdp::server server;

    ASSERT_EQ(69, server.option());
}

TEST(msdp_server_test, send_with_empty_list_sends_nothing)
{
    telnetpp::options::msdp::server server;
    activate_msdp_server(server);

    expect_elements(
        {},
        server.send({}));
}

TEST(msdp_server_test, send_with_variable_sends_simple_variable)
{
    telnetpp::options::msdp::server server;
    activate_msdp_server(server);

    expect_elements(
        {
            telnetpp::subnegotiation{
                server.option(),
                { 1, 'v', 'a', 'r', 2, 'v', 'a', 'l' }
            }
        },
        server.send({ { "var", "val" } }));
}

TEST(msdp_server_test, send_with_array_sends_array_variable)
{
    telnetpp::options::msdp::server server;
    activate_msdp_server(server);

    auto variable = telnetpp::options::msdp::variable{
        "var",
        { "val0", "val1" }
    };

    expect_elements(
        {
            telnetpp::subnegotiation{
                server.option(),
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
    activate_msdp_server(server);

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
                server.option(),
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
    activate_msdp_server(server);

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
                server.option(),
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

    activate_msdp_server(server);

    server.subnegotiate({});

    ASSERT_FALSE(called);
}

TEST(msdp_server_test, receiving_a_variable_reports_an_array_of_one_variable)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

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

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

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

TEST(msdp_server_test, receiving_empty_array_variable_reports_empty_array)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 'a', 'r', 'r',
        2, 5,
           6,
    });

    telnetpp::options::msdp::variable expected{
        "arr",
        std::vector<std::string>{}};

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_test, receiving_array_variable_with_one_element_reports_array)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 'a', 'r', 'r',
        2, 5,
              2, 'v', 'a', 'l',
           6,
    });

    telnetpp::options::msdp::variable expected{
        "arr",
        std::vector<std::string>{"val"}};

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_test, receiving_array_variable_with_two_elements_reports_array)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 'a', 'r', 'r',
        2, 5,
              2, 'v', 'a', 'l', '0',
              2, 'v', 'a', 'l', '1',
           6,
    });

    telnetpp::options::msdp::variable expected{
        "arr",
        std::vector<std::string>{"val0", "val1"}};

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_test, receiving_array_variable_then_string_reports_array_and_string)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 'a', 'r', 'r',
        2, 5,
              2, 'v', 'a', 'l', '0',
              2, 'v', 'a', 'l', '1',
           6,
        1, 'v', 'a', 'r',
        2, 'v', 'a', 'l',
    });

    telnetpp::options::msdp::variable expected0{
        "arr",
        std::vector<std::string>{"val0", "val1"}};
    telnetpp::options::msdp::variable expected1{"var", "val"};

    ASSERT_EQ(size_t{2}, variables.size());
    ASSERT_EQ(expected0, variables[0]);
    ASSERT_EQ(expected1, variables[1]);
}

TEST(msdp_server_test, receiving_empty_table_reports_empty_table)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 't', 'b', 'l',
        2, 3, 4
    });

    telnetpp::options::msdp::variable expected(
        "tbl",
        std::vector<telnetpp::options::msdp::variable>{});

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_test, receiving_table_with_one_value_returns_table_with_value)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 't', 'b', 'l',
        2, 3,
               1, 'v', 'a', 'r',
               2, 'v', 'a', 'l',
           4
    });

    telnetpp::options::msdp::variable expected(
        "tbl",
        std::vector<telnetpp::options::msdp::variable>{
            { "var", "val" }
        });

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_test, receiving_table_with_one_array_value_returns_table_with_value)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 't', 'b', 'l',
        2, 3,
               1, 'v', 'a', 'r',
               2, 5, 2, 'v', 'a', 'l', '0',
                     2, 'v', 'a', 'l', '1',
                  6,
           4
    });

    telnetpp::options::msdp::variable expected(
        "tbl",
        std::vector<telnetpp::options::msdp::variable>{
            { "var", std::vector<std::string>{ "val0", "val1" } }
        });

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_test, receiving_table_with_one_table_value_returns_table_with_value)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 't', 'b', 'l',
        2, 3,
               1, 'v', 'a', 'r',
               2, 3,
                     1, 'i', 'n',
                     2, 5, 2, 'v', 'a', 'l', '0',
                           2, 'v', 'a', 'l', '1',
                        6,
                  4,
           4
    });

    telnetpp::options::msdp::variable expected(
        "tbl",
        std::vector<telnetpp::options::msdp::variable>{
            { "var", std::vector<telnetpp::options::msdp::variable>{
                { "in", std::vector<std::string>{ "val0", "val1" } }
            }}
        });

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_test, receiving_table_with_many_values_returns_table_with_values)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    /*
     { "tbl" : {
         "var0" : {
             "in" : [ "val0", "val1" ],
             "var1" : "val1",
             "var2" : [ "arr0", "arr1" ]
         }
     }}
    */

    server.subnegotiate({
        1, 't', 'b', 'l',
        2, 3,
               1, 'v', 'a', 'r', '0',
               2, 3,
                     1, 'i', 'n',
                     2, 5, 2, 'v', 'a', 'l', '0',
                           2, 'v', 'a', 'l', '1',
                        6,
                  4,
               1, 'v', 'a', 'r', '1',
               2, 'v', 'a', 'l', '1',
               1, 'v', 'a', 'r', '2',
               2, 5, 2, 'a', 'r', 'r', '0',
                     2, 'a', 'r', 'r', '1',
                  6,
           4
    });

    telnetpp::options::msdp::variable expected(
        "tbl",
        std::vector<telnetpp::options::msdp::variable>{
            { "var0", std::vector<telnetpp::options::msdp::variable>{
                { "in", std::vector<std::string>{ "val0", "val1" } },
                { "var1", "val1" },
                { "var2", std::vector<std::string>{ "arr0", "arr1" }}
            }}
        });

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST(msdp_server_test, receiving_many_table_values_returns_many_tables)
{
    telnetpp::options::msdp::server server;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_server_variable_reception(server, variables);
    activate_msdp_server(server);

    server.subnegotiate({
        1, 't', 'b', 'l', '0',
        2, 3,
              1, 'v', 'a', 'r', '0',
              2, 'v', 'a', 'l', '0',
           4,
        1, 't', 'b', 'l', '1',
        2, 3,
              1, 'v', 'a', 'r', '1',
              2, 'v', 'a', 'l', '1',
           4
    });

    telnetpp::options::msdp::variable expected0(
        "tbl0",
        std::vector<telnetpp::options::msdp::variable>{
            { "var0", "val0" }
        });
    telnetpp::options::msdp::variable expected1(
        "tbl1",
        std::vector<telnetpp::options::msdp::variable>{
            { "var1", "val1" }
        });

    ASSERT_EQ(size_t{2}, variables.size());
    ASSERT_EQ(expected0, variables[0]);
    ASSERT_EQ(expected1, variables[1]);
}
