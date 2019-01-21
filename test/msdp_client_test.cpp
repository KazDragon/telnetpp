#include "telnetpp/options/msdp/client.hpp"
#include <gtest/gtest.h>

using namespace telnetpp::literals;

namespace {

/*
static void register_msdp_client_variable_reception(
    telnetpp::options::msdp::client &client,
    std::vector<telnetpp::options::msdp::variable> &variables)
{
    client.on_receive.connect(
        [&variables](auto const &vars) -> std::vector<telnetpp::token>
        {
            variables = vars;
            return {};
        });
}
*/

class in_an_activated_msdp_client : public testing::Test
{
protected:
    in_an_activated_msdp_client()
    {
        client_.negotiate(telnetpp::will, [](auto &&){});
        assert(client_.active());
    }
    
    telnetpp::options::msdp::client client_;
};

}

TEST(msdddp_client_test, option_is_msdp)
{
    telnetpp::options::msdp::client client;
    ASSERT_EQ(69, client.option_code());
}

TEST_F(in_an_activated_msdp_client, send_with_variable_sends_simple_variable)
{
    auto const var_name  = "var"_tb;
    auto const var_value = "val"_tb;
    telnetpp::options::msdp::variable const var{var_name, var_value};

    client_.send(
        var,
        [this](telnetpp::elements data)
        {
            ASSERT_EQ(size_t{1}, data.size());

            auto const expected_content = "\x01" "var" "\x02" "val"_tb;
            auto const expected_subnegotiation = telnetpp::subnegotiation{
                client_.option_code(),
                expected_content
            };

            ASSERT_EQ(telnetpp::element{expected_subnegotiation}, data[0]);
        });
}

/*
TEST_F(in_an_activated_msdp_client, send_with_array_sends_array_variable)
{
    telnetpp::options::msdp::client client;
    activate_msdp_client(client);

    auto variable = telnetpp::options::msdp::variable{
        "var",
        { "val0", "val1" }
    };

    expect_elements(
        {
            telnetpp::subnegotiation{
                client.option_code(),
                { 1, 'v', 'a', 'r',
                  2,
                      5,
                          2, 'v', 'a', 'l', '0',
                          2, 'v', 'a', 'l', '1',
                      6
                }
            }
        },
        client.send({ variable }));
}

TEST_F(in_an_activated_msdp_client, send_with_table_sends_table_variable)
{
    telnetpp::options::msdp::client client;
    activate_msdp_client(client);

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
                client.option_code(),
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
        client.send({ variable }));
}

TEST_F(in_an_activated_msdp_client, send_with_many_items_sends_many_items)
{
    telnetpp::options::msdp::client client;
    activate_msdp_client(client);

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
                client.option_code(),
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
        client.send({ variable0, variable1, variable2 }));
}

TEST_F(in_an_activated_msdp_client, receiving_no_variables_does_nothing)
{
    telnetpp::options::msdp::client client;

    bool called = false;
    client.on_receive.connect(
        [&called](auto const &) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    activate_msdp_client(client);

    client.subnegotiate({});

    ASSERT_FALSE(called);
}

TEST_F(in_an_activated_msdp_client, receiving_a_variable_reports_an_array_of_one_variable)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
        1, 'v', 'a', 'r',
        2, 'v', 'a', 'l'
    });

    telnetpp::options::msdp::variable expected{"var", "val"};

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST_F(in_an_activated_msdp_client, receiving_two_variables_reports_an_array_of_two_variable)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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

TEST_F(in_an_activated_msdp_client, receiving_empty_array_variable_reports_empty_array)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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

TEST_F(in_an_activated_msdp_client, receiving_array_variable_with_one_element_reports_array)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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

TEST_F(in_an_activated_msdp_client, receiving_array_variable_with_two_elements_reports_array)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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

TEST_F(in_an_activated_msdp_client, receiving_array_variable_then_string_reports_array_and_string)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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

TEST_F(in_an_activated_msdp_client, receiving_empty_table_reports_empty_table)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
        1, 't', 'b', 'l',
        2, 3, 4
    });

    telnetpp::options::msdp::variable expected(
        "tbl",
        std::vector<telnetpp::options::msdp::variable>{});

    ASSERT_EQ(size_t{1}, variables.size());
    ASSERT_EQ(expected, variables[0]);
}

TEST_F(in_an_activated_msdp_client, receiving_table_with_one_value_returns_table_with_value)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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

TEST_F(in_an_activated_msdp_client, receiving_table_with_one_array_value_returns_table_with_value)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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

TEST_F(in_an_activated_msdp_client, receiving_table_with_one_table_value_returns_table_with_value)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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

TEST_F(in_an_activated_msdp_client, receiving_table_with_many_values_returns_table_with_values)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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

TEST_F(in_an_activated_msdp_client, receiving_many_table_values_returns_many_tables)
{
    telnetpp::options::msdp::client client;
    std::vector<telnetpp::options::msdp::variable> variables;

    register_msdp_client_variable_reception(client, variables);
    activate_msdp_client(client);

    client.subnegotiate({
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
*/
