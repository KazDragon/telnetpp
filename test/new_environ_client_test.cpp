#include "telnetpp/options/new_environ/client.hpp"
#include <gtest/gtest.h>
#include <utility>
#include <cassert>

using namespace telnetpp::literals;

namespace {

class in_an_active_new_environ_client : public testing::Test
{
public:
    in_an_active_new_environ_client()
    {
        client_.negotiate(telnetpp::will, [](auto &&){});
        assert(client_.active());
    }

protected:
    telnetpp::options::new_environ::client client_;
};

}

TEST(new_environ_client_test, option_is_new_environ)
{
    telnetpp::options::new_environ::client client;
    ASSERT_EQ(39, client.code());
}

TEST_F(in_an_active_new_environ_client, requesting_no_variables_requests_empty_list)
{
    // According to the specification, if no variables are specified in a
    // request, then it is equivalent to requesting all the variables.

    std::vector<telnetpp::elements> results;
    client_.request_variables(
        {},
        [&](telnetpp::element const &elem)
        {
            static constexpr telnetpp::byte const expected_content[] = {
                0x01
            };

            telnetpp::element const expected_result{
                telnetpp::subnegotiation{client_.code(), expected_content}
            };

            ASSERT_EQ(expected_result, elem);

            results.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, results.size());
}

TEST_F(in_an_active_new_environ_client, requesting_one_var_variable_requests_user_variable)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TEST"_tb
        }
    };

    std::vector<telnetpp::elements> results;
    client_.request_variables(
        requests,
        [&](telnetpp::element const &elem)
        {
            static auto const expected_content = 
                "\x01"
                "\x00" "TEST"_tb
            ;

            telnetpp::element const expected_result{
                telnetpp::subnegotiation{client_.code(), expected_content}
            };

            ASSERT_EQ(expected_result, elem);

            results.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, results.size());
}

TEST_F(in_an_active_new_environ_client, requesting_one_user_variable_requests_user_variable)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::uservar, 
            "TEST"_tb
        }
    };

    std::vector<telnetpp::elements> results;
    client_.request_variables(
        requests,
        [&](telnetpp::element const &elem)
        {
            static auto const expected_content =
                "\x01"
                "\x03" "TEST"_tb
            ;

            telnetpp::element const expected_result{
                telnetpp::subnegotiation{client_.code(), expected_content}
            };

            ASSERT_EQ(expected_result, elem);

            results.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, results.size());
}


TEST_F(in_an_active_new_environ_client, requesting_two_variables_requests_two_variables)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TEST0"_tb
        },

        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::uservar, 
            "TEST1"_tb
        }
    };

    std::vector<telnetpp::elements> results;
    client_.request_variables(
        requests,
        [&](telnetpp::element const &elem)
        {
            static auto const expected_content =
                "\x01"
                "\x00" "TEST0"
                "\x03" "TEST1"_tb
            ;

            telnetpp::element const expected_result{
                telnetpp::subnegotiation{client_.code(), expected_content}
            };

            ASSERT_EQ(expected_result, elem);

            results.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, results.size());
}

TEST_F(in_an_active_new_environ_client, requesting_uservar_with_var_in_name_is_escaped)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TE" "\x00" "ST"_tb
        }
    };

    std::vector<telnetpp::elements> results;
    client_.request_variables(
        requests,
        [&](telnetpp::element const &elem)
        {
            static auto const expected_content = 
                "\x01"
                "\x00" "TE" "\x02\x00" "ST"_tb
            ;

            telnetpp::element const expected_result{
                telnetpp::subnegotiation{client_.code(), expected_content}
            };

            ASSERT_EQ(expected_result, elem);

            results.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, results.size());
}

TEST_F(in_an_active_new_environ_client, requesting_uservar_with_value_in_name_is_escaped)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TE" "\x01" "ST"_tb
        }
    };

    std::vector<telnetpp::elements> results;
    client_.request_variables(
        requests,
        [&](telnetpp::element const &elem)
        {
            static auto const expected_content = 
                "\x01"
                "\x00" "TE" "\x02\x01" "ST"_tb
            ;

            telnetpp::element const expected_result{
                telnetpp::subnegotiation{client_.code(), expected_content}
            };

            ASSERT_EQ(expected_result, elem);

            results.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, results.size());
}

TEST_F(in_an_active_new_environ_client, requesting_uservar_with_esc_in_name_is_escaped)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TE" "\x02" "ST"_tb
        }
    };

    std::vector<telnetpp::elements> results;
    client_.request_variables(
        requests,
        [&](telnetpp::element const &elem)
        {
            static auto const expected_content = 
                "\x01"
                "\x00" "TE" "\x02\x02" "ST"_tb
            ;

            telnetpp::element const expected_result{
                telnetpp::subnegotiation{client_.code(), expected_content}
            };

            ASSERT_EQ(expected_result, elem);

            results.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, results.size());
}

TEST_F(in_an_active_new_environ_client, requesting_uservar_with_uservar_in_name_is_escaped)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TE" "\x03" "ST"_tb
        }
    };

    std::vector<telnetpp::elements> results;
    client_.request_variables(
        requests,
        [&](telnetpp::element const &elem)
        {
            static auto const expected_content = 
                "\x01"
                "\x00" "TE" "\x02\x03" "ST"_tb
            ;

            telnetpp::element const expected_result{
                telnetpp::subnegotiation{client_.code(), expected_content}
            };

            ASSERT_EQ(expected_result, elem);

            results.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, results.size());
}

TEST_F(in_an_active_new_environ_client, receiving_empty_response_does_nothing)
{
    bool called = false;
    client_.on_variable_changed.connect(
        [&called](auto &&, auto &&)
        {
            called = true;
        });

    client_.subnegotiate({}, [](auto &&){});

    ASSERT_FALSE(called);
}


TEST_F(in_an_active_new_environ_client, receiving_empty_list_does_nothing)
{
    bool called = false;
    client_.on_variable_changed.connect(
        [&called](auto &&, auto &&)
        {
            called = true;
        });

    static auto const content = "\x00"_tb;
    client_.subnegotiate(content, [](auto &&){});

    ASSERT_FALSE(called);
}

TEST_F(in_an_active_new_environ_client, receiving_var_and_no_value_reports_undefined_var)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x00" "USER"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(size_t{1}, received_responses.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, received_responses[0].type);
    ASSERT_EQ("USER"_tb, received_responses[0].name);
    ASSERT_FALSE(received_responses[0].value.is_initialized());
}

TEST_F(in_an_active_new_environ_client, receiving_uservar_and_no_value_reports_undefined_uservar)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TEST"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(size_t{1}, received_responses.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[0].type);
    ASSERT_EQ("TEST"_tb, received_responses[0].name);
    ASSERT_FALSE(received_responses[0].value.is_initialized());
}

TEST_F(in_an_active_new_environ_client, receiving_var_and_empty_value_reports_empty_var)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x00" "USER"
        "\x01"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(size_t{1}, received_responses.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, received_responses[0].type);
    ASSERT_EQ("USER"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ(""_tb, *received_responses[0].value);
}

TEST_F(in_an_active_new_environ_client, receiving_uservar_and_empty_value_reports_empty_var)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TEST"
        "\x01"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(size_t{1}, received_responses.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[0].type);
    ASSERT_EQ("TEST"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ(""_tb, *received_responses[0].value);
}

TEST_F(in_an_active_new_environ_client, receiving_var_and_value_reports_var_and_value)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x00" "USER"
        "\x01" "BOB"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(size_t{1}, received_responses.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, received_responses[0].type);
    ASSERT_EQ("USER"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("BOB"_tb, *received_responses[0].value);
}

TEST_F(in_an_active_new_environ_client, receiving_uservar_and_value_reports_var_and_value)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TEST"
        "\x01" "VALUE"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(size_t{1}, received_responses.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[0].type);
    ASSERT_EQ("TEST"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("VALUE"_tb, *received_responses[0].value);
}

TEST_F(in_an_active_new_environ_client, receiving_multiple_variables_reports_multiple_variables)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x00" "USER"
        "\x01" "BOB"
        "\x03" "TEST0"
        "\x03" "TEST1"
        "\x01" "VALUE"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(std::size_t{3}, received_responses.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, received_responses[0].type);
    ASSERT_EQ("USER"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("BOB"_tb, *received_responses[0].value);

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[1].type);
    ASSERT_EQ("TEST0"_tb, received_responses[1].name);
    ASSERT_FALSE(received_responses[1].value.is_initialized());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[2].type);
    ASSERT_EQ("TEST1"_tb, received_responses[2].name);
    ASSERT_TRUE(received_responses[2].value.is_initialized());
    ASSERT_EQ("VALUE"_tb, *received_responses[2].value);
}

TEST_F(in_an_active_new_environ_client, receiving_info_for_multiple_variables_reports_multiple_variables)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x02"
        "\x00" "USER"
        "\x01" "BOB"
        "\x03" "TEST0"
        "\x03" "TEST1"
        "\x01" "VALUE"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(std::size_t{3}, received_responses.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, received_responses[0].type);
    ASSERT_EQ("USER"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("BOB"_tb, *received_responses[0].value);

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[1].type);
    ASSERT_EQ("TEST0"_tb, received_responses[1].name);
    ASSERT_FALSE(received_responses[1].value.is_initialized());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[2].type);
    ASSERT_EQ("TEST1"_tb, received_responses[2].name);
    ASSERT_TRUE(received_responses[2].value.is_initialized());
    ASSERT_EQ("VALUE"_tb, *received_responses[2].value);
}

TEST_F(in_an_active_new_environ_client, receiving_uservar_with_escaped_var_in_name_reports_variable_correctly)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TE" "\x02\x00" "ST"
        "\x01" "VALUE"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(std::size_t{1}, received_responses.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[0].type);
    ASSERT_EQ("TE\x00ST"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("VALUE"_tb, *received_responses[0].value);
}

TEST_F(in_an_active_new_environ_client, receiving_uservar_with_escaped_uservar_in_name_reports_variable_correctly)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TE" "\x02\x03" "ST"
        "\x01" "VALUE"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(std::size_t{1}, received_responses.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[0].type);
    ASSERT_EQ("TE\x03ST"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("VALUE"_tb, *received_responses[0].value);
}

TEST_F(in_an_active_new_environ_client, receiving_uservar_with_escaped_value_in_name_reports_variable_correctly)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TE" "\x02\x01" "ST"
        "\x01" "VALUE"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(std::size_t{1}, received_responses.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[0].type);
    ASSERT_EQ("TE\x01ST"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("VALUE"_tb, *received_responses[0].value);
}

TEST_F(in_an_active_new_environ_client, receiving_uservar_with_escaped_escape_in_name_reports_variable_correctly)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TE" "\x02\x02" "ST"
        "\x01" "VALUE"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(std::size_t{1}, received_responses.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[0].type);
    ASSERT_EQ("TE\x02ST"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("VALUE"_tb, *received_responses[0].value);
}

TEST_F(in_an_active_new_environ_client, receiving_uservar_with_escaped_var_in_value_reports_variable_correctly)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TEST"
        "\x01" "VAL" "\x02\x00" "UE"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(std::size_t{1}, received_responses.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[0].type);
    ASSERT_EQ("TEST"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("VAL\x00UE"_tb, *received_responses[0].value);
}

TEST_F(in_an_active_new_environ_client, receiving_uservar_with_escaped_uservar_in_value_reports_variable_correctly)
{
    std::vector<telnetpp::options::new_environ::response> received_responses;    
    client_.on_variable_changed.connect(
        [&](telnetpp::options::new_environ::response const &rsp,
            auto &&cont)
        {
            received_responses.push_back(rsp);
        });

    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TEST"
        "\x01" "VAL" "\x02\x03" "UE"_tb;

    client_.subnegotiate(subnegotiation_content, [](auto &&){});

    ASSERT_EQ(std::size_t{1}, received_responses.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, received_responses[0].type);
    ASSERT_EQ("TEST"_tb, received_responses[0].name);
    ASSERT_TRUE(received_responses[0].value.is_initialized());
    ASSERT_EQ("VAL\x03UE"_tb, *received_responses[0].value);
}
