#include "telnetpp/options/new_environ/server.hpp"
#include <gtest/gtest.h>

using namespace telnetpp::literals;

namespace {

class in_an_active_new_environ_server : public testing::Test
{
public:
    in_an_active_new_environ_server()
    {
        server_.negotiate(telnetpp::do_, [](auto &&){});
        assert(server_.active());
    }

protected:
    telnetpp::options::new_environ::server server_;
};

}

TEST(a_new_environ_server, has_option_code_new_environ)
{
    telnetpp::options::new_environ::server server;
    ASSERT_EQ(39, server.option_code());
}

TEST_F(in_an_active_new_environ_server, receiving_request_with_no_content_and_no_cached_variables_sends_empty_list)
{
    static auto const expected_subnegotiation_content = "\x00"_tb;
    static auto const expected_responses = std::vector<telnetpp::element> {
        telnetpp::subnegotiation{
            server_.option_code(), 
            expected_subnegotiation_content
        }
    };

    static auto const request_subnegotiation_content = "\x01"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        request_subnegotiation_content,
        [&](telnetpp::element const &elem)
        {
            received_responses.push_back(elem);

            ASSERT_EQ(expected_responses[0], elem);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, receiving_request_with_no_content_and_one_cached_variable_sends_variable)
{
    static auto const name = "USER"_tb;
    static auto const value = "TEST"_tb;

    server_.set_variable(name, value, [](auto &&){});

    static auto const expected_subnegotiation_content = 
        "\x00"
        "\x00" "USER"
        "\x01" "TEST"_tb;

    static auto const expected_responses = std::vector<telnetpp::element> {
        telnetpp::subnegotiation{
            server_.option_code(), 
            expected_subnegotiation_content
        }
    };

    static auto const request_subnegotiation_content = "\x01"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        request_subnegotiation_content,
        [&](telnetpp::element const &elem)
        {
            received_responses.push_back(elem);

            ASSERT_EQ(expected_responses[0], elem);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, receiving_request_with_no_content_and_one_cached_user_variable_sends_all_variables)
{
    static auto const name = "USER"_tb;
    static auto const value = "TEST"_tb;

    server_.set_user_variable(name, value, [](auto &&){});

    static auto const expected_subnegotiation_content = 
        "\x00"
        "\x03" "USER"
        "\x01" "TEST"_tb;

    static auto const expected_responses = std::vector<telnetpp::element> {
        telnetpp::subnegotiation{
            server_.option_code(), 
            expected_subnegotiation_content
        }
    };

    static auto const request_subnegotiation_content = "\x01"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        request_subnegotiation_content,
        [&](telnetpp::element const &elem)
        {
            received_responses.push_back(elem);

            ASSERT_EQ(expected_responses[0], elem);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, receiving_request_with_no_content_and_one_cached_variable_with_empty_value_sends_all_variables)
{
    static auto const name = "USER"_tb;
    static auto const value = ""_tb;

    server_.set_variable(name, value, [](auto &&){});

    static auto const expected_subnegotiation_content = 
        "\x00"
        "\x00" "USER"
        "\x01"_tb;

    static auto const expected_responses = std::vector<telnetpp::element> {
        telnetpp::subnegotiation{
            server_.option_code(), 
            expected_subnegotiation_content
        }
    };

    static auto const request_subnegotiation_content = "\x01"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        request_subnegotiation_content,
        [&](telnetpp::element const &elem)
        {
            received_responses.push_back(elem);

            ASSERT_EQ(expected_responses[0], elem);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, receiving_request_with_no_content_and_one_deleted_variable_sends_no_variables)
{
    static auto const name = "USER"_tb;
    static auto const value = "TEST"_tb;

    server_.set_variable(name, value, [](auto &&){});
    server_.delete_variable(name, [](auto &&){});

    static auto const expected_subnegotiation_content = "\x00"_tb;
    static auto const expected_responses = std::vector<telnetpp::element> {
        telnetpp::subnegotiation{
            server_.option_code(), 
            expected_subnegotiation_content
        }
    };

    static auto const request_subnegotiation_content = "\x01"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        request_subnegotiation_content,
        [&](telnetpp::element const &elem)
        {
            received_responses.push_back(elem);

            ASSERT_EQ(expected_responses[0], elem);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, receiving_request_with_no_content_and_one_deleted_uservariable_sends_no_variables)
{
    static auto const name = "TTYPE"_tb;
    static auto const value = "XTERM"_tb;

    server_.set_user_variable(name, value, [](auto &&){});
    server_.delete_user_variable(name, [](auto &&){});

    static auto const expected_subnegotiation_content = "\x00"_tb;
    static auto const expected_responses = std::vector<telnetpp::element> {
        telnetpp::subnegotiation{
            server_.option_code(), 
            expected_subnegotiation_content
        }
    };

    static auto const request_subnegotiation_content = "\x01"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        request_subnegotiation_content,
        [&](telnetpp::element const &elem)
        {
            received_responses.push_back(elem);

            ASSERT_EQ(expected_responses[0], elem);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, receiving_request_with_no_content_and_multiple_cached_variables_sends_all_variables)
{
    static auto const name0 = "PRINTER"_tb;
    static auto const value0 = "LPT1"_tb;
    static auto const name1 = "USER"_tb;
    static auto const value1 = "TEST"_tb;
    static auto const name2 = "LOCALE"_tb;
    static auto const value2 = "EN_GB"_tb;
    static auto const name3 = "TTYPE"_tb;
    static auto const value3 = "XTERM"_tb;

    server_.set_variable(name0, value0, [](auto &&){});
    server_.set_variable(name1, value1, [](auto &&){});
    server_.set_user_variable(name2, value2, [](auto &&){});
    server_.set_user_variable(name3, value3, [](auto &&){});

    static auto const expected_subnegotiation_content = 
        "\x00"
        "\x00" "PRINTER" "\x01" "LPT1"
        "\x00" "USER"    "\x01" "TEST"
        "\x03" "LOCALE"  "\x01" "EN_GB"
        "\x03" "TTYPE"   "\x01" "XTERM"_tb;

    static auto const expected_responses = std::vector<telnetpp::element> {
        telnetpp::subnegotiation{
            server_.option_code(), 
            expected_subnegotiation_content
        }
    };

    static auto const request_subnegotiation_content = "\x01"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        request_subnegotiation_content,
        [&](telnetpp::element const &elem)
        {
            received_responses.push_back(elem);

            ASSERT_EQ(expected_responses[0], elem);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, modifying_a_variable_sends_variable_modification)
{
    static auto const name = "USER"_tb;
    static auto const old_value = "FRED"_tb;
    static auto const new_value = "BOB"_tb;

    server_.set_variable(name, old_value, [](auto &&){});

    std::vector<telnetpp::element> received_responses;
    server_.set_variable(
        name,
        new_value,
        [&](telnetpp::element const &received_element)
        {
            static auto const expected_subnegotiation_content = 
                "\x02"
                "\x00" "USER" "\x01" "BOB"_tb;

            static telnetpp::element const expected_element = {
                telnetpp::subnegotiation{
                    server_.option_code(),
                    expected_subnegotiation_content
                }
            };

            ASSERT_EQ(expected_element, received_element);

            received_responses.push_back(received_element);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, deleting_a_variable_sends_variable_deletion)
{
    static auto const name = "USER"_tb;
    static auto const value = "FRED"_tb;

    server_.set_variable(name, value, [](auto &&){});

    std::vector<telnetpp::element> received_responses;
    server_.delete_variable(
        name,
        [&](telnetpp::element const &received_element)
        {
            static auto const expected_subnegotiation_content = 
                "\x02"
                "\x00" "USER"_tb;

            static telnetpp::element const expected_element = {
                telnetpp::subnegotiation{
                    server_.option_code(),
                    expected_subnegotiation_content
                }
            };

            ASSERT_EQ(expected_element, received_element);

            received_responses.push_back(received_element);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, modifying_a_user_variable_sends_variable_modification)
{
    static auto const name = "TEST"_tb;
    static auto const old_value = "VALUE"_tb;
    static auto const new_value = "RESULT"_tb;

    server_.set_user_variable(name, old_value, [](auto &&){});

    std::vector<telnetpp::element> received_responses;
    server_.set_user_variable(
        name,
        new_value,
        [&](telnetpp::element const &received_element)
        {
            static auto const expected_subnegotiation_content = 
                "\x02"
                "\x03" "TEST" "\x01" "RESULT"_tb;

            static telnetpp::element const expected_element = {
                telnetpp::subnegotiation{
                    server_.option_code(),
                    expected_subnegotiation_content
                }
            };

            ASSERT_EQ(expected_element, received_element);

            received_responses.push_back(received_element);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, deleting_a_user_variable_sends_variable_deletion)
{
    static auto const name = "TEST"_tb;
    static auto const value = "VALUE"_tb;

    server_.set_user_variable(name, value, [](auto &&){});

    std::vector<telnetpp::element> received_responses;
    server_.delete_user_variable(
        name,
        [&](telnetpp::element const &received_element)
        {
            static auto const expected_subnegotiation_content = 
                "\x02"
                "\x03" "TEST"_tb;

            static telnetpp::element const expected_element = {
                telnetpp::subnegotiation{
                    server_.option_code(),
                    expected_subnegotiation_content
                }
            };

            ASSERT_EQ(expected_element, received_element);

            received_responses.push_back(received_element);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, requesting_nonexistent_variable_rturns_empty_result)
{
    static auto const name = "TEST"_tb;
    static auto const value = "VALUE"_tb;

    server_.set_user_variable(name, value, [](auto &&){});

    static auto const subnegotiation_content = 
        "\x01\x03" "FAIL"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        subnegotiation_content,
        [&](telnetpp::element const &received_element)
        {
            static auto const expected_subnegotiation_content = 
                "\x00"_tb;

            static telnetpp::element const expected_element = {
                telnetpp::subnegotiation{
                    server_.option_code(),
                    expected_subnegotiation_content
                }
            };

            ASSERT_EQ(expected_element, received_element);

            received_responses.push_back(received_element);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, requesting_existent_variables_sends_variables)
{
    static auto const name0 = "PRINTER"_tb;
    static auto const value0 = "LPT1"_tb;
    static auto const name1 = "USER"_tb;
    static auto const value1 = "TEST"_tb;
    static auto const name2 = "LOCALE"_tb;
    static auto const value2 = "EN_GB"_tb;
    static auto const name3 = "TTYPE"_tb;
    static auto const value3 = "XTERM"_tb;

    server_.set_variable(name0, value0, [](auto &&){});
    server_.set_variable(name1, value1, [](auto &&){});
    server_.set_user_variable(name2, value2, [](auto &&){});
    server_.set_user_variable(name3, value3, [](auto &&){});

    static auto const subnegotiation_content = 
        "\x01"
        "\x03" "TTYPE"
        "\x00" "USER"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        subnegotiation_content,
        [&](telnetpp::element const &received_element)
        {
            static auto const expected_subnegotiation_content = 
                "\x00"
                "\x03" "TTYPE" "\x01" "XTERM"
                "\x00" "USER"  "\x01" "TEST"_tb;

            static telnetpp::element const expected_element = {
                telnetpp::subnegotiation{
                    server_.option_code(),
                    expected_subnegotiation_content
                }
            };

            ASSERT_EQ(expected_element, received_element);

            received_responses.push_back(received_element);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, names_and_values_with_control_characters_are_escaped_properly)
{
    static auto const name0 = "USER"_tb;
    static auto const value0 = "\x01" "ABC"_tb;
    static auto const name1 = "\x00" "DEF"_tb;
    static auto const value1 = "\x02" "GHI"_tb;

    server_.set_variable(name0, value0, [](auto &&){});
    server_.set_user_variable(name1, value1, [](auto &&){});

    static auto const subnegotiation_content = 
        "\x01"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        subnegotiation_content,
        [&](telnetpp::element const &received_element)
        {
            static auto const expected_subnegotiation_content = 
                "\x00"
                "\x00" "USER" "\x01" "\x02\x01" "ABC"
                "\x03" "\x02\x00" "DEF" "\x01" "\x02\x02" "GHI"_tb;

            static telnetpp::element const expected_element = {
                telnetpp::subnegotiation{
                    server_.option_code(),
                    expected_subnegotiation_content
                }
            };

            ASSERT_EQ(expected_element, received_element);

            received_responses.push_back(received_element);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}

TEST_F(in_an_active_new_environ_server, requests_with_escaped_control_characters_are_handled_correctly)
{
    static auto const name = "\x01\x02\x03\x04" "GOOD"_tb;
    static auto const value = "VALUE"_tb;

    server_.set_user_variable(name, value, [](auto &&){});

    static auto const subnegotiation_content = 
        "\x01\x03" "\x02\x01\x02\x02\x02\x03\x04" "GOOD"_tb;

    std::vector<telnetpp::element> received_responses;
    server_.subnegotiate(
        subnegotiation_content,
        [&](telnetpp::element const &received_element)
        {
            static auto const expected_subnegotiation_content = 
                "\x00"
                "\x03" "\x02\x01\x02\x02\x02\x03\x04" "GOOD"
                "\x01" "VALUE"_tb;

            static telnetpp::element const expected_element = {
                telnetpp::subnegotiation{
                    server_.option_code(),
                    expected_subnegotiation_content
                }
            };

            ASSERT_EQ(expected_element, received_element);

            received_responses.push_back(received_element);
        });

    ASSERT_EQ(size_t{1}, received_responses.size());
}
