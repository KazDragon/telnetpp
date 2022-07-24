#include "telnetpp/options/new_environ/client.hpp"
#include "telnet_option_fixture.hpp"
#include <gtest/gtest.h>
#include <utility>
#include <cassert>

using namespace telnetpp::literals;

namespace {
using a_new_environ_client = a_telnet_option<telnetpp::options::new_environ::client>;
}

TEST_F(a_new_environ_client, is_a_new_environ_client)
{
    ASSERT_EQ(39, option_.option_code());
}

namespace {

class an_active_new_environ_client : public a_new_environ_client
{
protected:
    an_active_new_environ_client()
    {
        option_.negotiate(telnetpp::will);
        assert(option_.active());
        channel_.written_.clear();

        option_.on_variable_changed.connect(
            [this](telnetpp::options::new_environ::response const &rsp)
            {
                responses_.push_back(rsp);
            });
    }

    std::vector<telnetpp::options::new_environ::response> responses_;
};

}

TEST_F(an_active_new_environ_client, requesting_no_variables_requests_empty_list)
{
    // According to the specification, if no variables are specified in a
    // request, then it is equivalent to requesting all the variables.
    option_.request_variables({});

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x01, // SEND
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_result, channel_.written_);
}


TEST_F(an_active_new_environ_client, requesting_one_var_variable_requests_user_variable)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TEST"_tb
        }
    };

    option_.request_variables(requests);

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x01, // SEND
            0x00, 'T', 'E', 'S', 'T', // VAR TEST
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_result, channel_.written_);
}

TEST_F(an_active_new_environ_client, requesting_one_user_variable_requests_user_variable)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::uservar, 
            "TEST"_tb
        }
    };

    option_.request_variables(requests);

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x01, // SEND
            0x03, 'T', 'E', 'S', 'T', // USERVAR TEST
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_result, channel_.written_);
}


TEST_F(an_active_new_environ_client, requesting_two_variables_requests_two_variables)
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

    option_.request_variables(requests);

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x01, // SEND
            0x00, 'T', 'E', 'S', 'T', '0', // VAR TEST
            0x03, 'T', 'E', 'S', 'T', '1', // USERVAR TEST
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_result, channel_.written_);
}

TEST_F(an_active_new_environ_client, requesting_var_with_var_in_name_is_escaped)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TE" "\x00" "ST"_tb
        }
    };

    option_.request_variables(requests);

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x01, // SEND
            0x00, 'T', 'E', 0x02, 0x00, 'S', 'T', // VAR TE\0ST
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_result, channel_.written_);
}

TEST_F(an_active_new_environ_client, requesting_var_with_value_in_name_is_escaped)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TE" "\x01" "ST"_tb
        }
    };

    option_.request_variables(requests);

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x01, // SEND
            0x00, 'T', 'E', 0x02, 0x01, 'S', 'T', // VAR TE\1ST
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_result, channel_.written_);
}

TEST_F(an_active_new_environ_client, requesting_var_with_esc_in_name_is_escaped)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TE" "\x02" "ST"_tb
        }
    };

    option_.request_variables(requests);

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x01, // SEND
            0x00, 'T', 'E', 0x02, 0x02, 'S', 'T', // VAR TE\2ST
        telnetpp::iac, telnetpp::se
    };
}

TEST_F(an_active_new_environ_client, requesting_var_with_uservar_in_name_is_escaped)
{
    static telnetpp::options::new_environ::request const requests[] =
    {
        telnetpp::options::new_environ::request{ 
            telnetpp::options::new_environ::variable_type::var, 
            "TE" "\x03" "ST"_tb
        }
    };

    option_.request_variables(requests);

    telnetpp::byte_storage const expected_result = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x01, // SEND
            0x00, 'T', 'E', 0x02, 0x03, 'S', 'T', // VAR TE\3ST
        telnetpp::iac, telnetpp::se
    };
}

TEST_F(an_active_new_environ_client, receiving_empty_response_does_nothing)
{
    option_.subnegotiate({});
    ASSERT_TRUE(responses_.empty());
}

TEST_F(an_active_new_environ_client, receiving_empty_list_does_nothing)
{
    static auto const content = "\x00"_tb;
    option_.subnegotiate(content);
    ASSERT_TRUE(responses_.empty());
}

TEST_F(an_active_new_environ_client, receiving_var_and_no_value_reports_undefined_var)
{
    static auto const subnegotiation_content =
        "\x00" // IS
        "\x00" "USER"_tb; // VAR USER

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(size_t{1}, responses_.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, responses_[0].type);
    ASSERT_EQ("USER"_tb, responses_[0].name);
    ASSERT_FALSE(responses_[0].value.has_value());
}

TEST_F(an_active_new_environ_client, receiving_uservar_and_no_value_reports_undefined_uservar)
{
    static auto const subnegotiation_content =
        "\x00" // IS
        "\x03" "TEST"_tb; // USERVAR TEST

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(size_t{1}, responses_.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[0].type);
    ASSERT_EQ("TEST"_tb, responses_[0].name);
    ASSERT_FALSE(responses_[0].value.has_value());
}

TEST_F(an_active_new_environ_client, receiving_var_and_empty_value_reports_empty_var)
{
    static auto const subnegotiation_content =
        "\x00"  // IS
        "\x00" "USER" // VAR USER
        "\x01"_tb; // VAL

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(size_t{1}, responses_.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, responses_[0].type);
    ASSERT_EQ("USER"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ(""_tb, *responses_[0].value);
}

TEST_F(an_active_new_environ_client, receiving_uservar_and_empty_value_reports_empty_var)
{
    static auto const subnegotiation_content =
        "\x00"  // IS
        "\x03" "TEST" // USERVAR TEST
        "\x01"_tb; // VAL

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(size_t{1}, responses_.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[0].type);
    ASSERT_EQ("TEST"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ(""_tb, *responses_[0].value);
}

TEST_F(an_active_new_environ_client, receiving_var_and_value_reports_var_and_value)
{
    static auto const subnegotiation_content =
        "\x00"
        "\x00" "USER"
        "\x01" "BOB"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(size_t{1}, responses_.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, responses_[0].type);
    ASSERT_EQ("USER"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("BOB"_tb, *responses_[0].value);
}

TEST_F(an_active_new_environ_client, receiving_uservar_and_value_reports_var_and_value)
{
    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TEST"
        "\x01" "VALUE"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(size_t{1}, responses_.size());
    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[0].type);
    ASSERT_EQ("TEST"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("VALUE"_tb, *responses_[0].value);
}

TEST_F(an_active_new_environ_client, receiving_multiple_variables_reports_multiple_variables)
{
    static auto const subnegotiation_content =
        "\x00"
        "\x00" "USER"
        "\x01" "BOB"
        "\x03" "TEST0"
        "\x03" "TEST1"
        "\x01" "VALUE"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(std::size_t{3}, responses_.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, responses_[0].type);
    ASSERT_EQ("USER"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("BOB"_tb, *responses_[0].value);

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[1].type);
    ASSERT_EQ("TEST0"_tb, responses_[1].name);
    ASSERT_FALSE(responses_[1].value.has_value());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[2].type);
    ASSERT_EQ("TEST1"_tb, responses_[2].name);
    ASSERT_TRUE(responses_[2].value.has_value());
    ASSERT_EQ("VALUE"_tb, *responses_[2].value);
}

TEST_F(an_active_new_environ_client, receiving_info_for_multiple_variables_reports_multiple_variables)
{
    static auto const subnegotiation_content =
        "\x02"
        "\x00" "USER"
        "\x01" "BOB"
        "\x03" "TEST0"
        "\x03" "TEST1"
        "\x01" "VALUE"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(std::size_t{3}, responses_.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::var, responses_[0].type);
    ASSERT_EQ("USER"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("BOB"_tb, *responses_[0].value);

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[1].type);
    ASSERT_EQ("TEST0"_tb, responses_[1].name);
    ASSERT_FALSE(responses_[1].value.has_value());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[2].type);
    ASSERT_EQ("TEST1"_tb, responses_[2].name);
    ASSERT_TRUE(responses_[2].value.has_value());
    ASSERT_EQ("VALUE"_tb, *responses_[2].value);
}

TEST_F(an_active_new_environ_client, receiving_uservar_with_escaped_var_in_name_reports_variable_correctly)
{
    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TE" "\x02\x00" "ST"
        "\x01" "VALUE"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(std::size_t{1}, responses_.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[0].type);
    ASSERT_EQ("TE\x00ST"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("VALUE"_tb, *responses_[0].value);
}

TEST_F(an_active_new_environ_client, receiving_uservar_with_escaped_uservar_in_name_reports_variable_correctly)
{
    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TE" "\x02\x03" "ST"
        "\x01" "VALUE"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(std::size_t{1}, responses_.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[0].type);
    ASSERT_EQ("TE\x03ST"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("VALUE"_tb, *responses_[0].value);
}

TEST_F(an_active_new_environ_client, receiving_uservar_with_escaped_value_in_name_reports_variable_correctly)
{
    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TE" "\x02\x01" "ST"
        "\x01" "VALUE"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(std::size_t{1}, responses_.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[0].type);
    ASSERT_EQ("TE\x01ST"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("VALUE"_tb, *responses_[0].value);
}

TEST_F(an_active_new_environ_client, receiving_uservar_with_escaped_escape_in_name_reports_variable_correctly)
{
    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TE" "\x02\x02" "ST"
        "\x01" "VALUE"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(std::size_t{1}, responses_.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[0].type);
    ASSERT_EQ("TE\x02ST"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("VALUE"_tb, *responses_[0].value);
}

TEST_F(an_active_new_environ_client, receiving_uservar_with_escaped_var_in_value_reports_variable_correctly)
{
    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TEST"
        "\x01" "VAL" "\x02\x00" "UE"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(std::size_t{1}, responses_.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[0].type);
    ASSERT_EQ("TEST"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("VAL\x00UE"_tb, *responses_[0].value);
}

TEST_F(an_active_new_environ_client, receiving_uservar_with_escaped_uservar_in_value_reports_variable_correctly)
{
    static auto const subnegotiation_content =
        "\x00"
        "\x03" "TEST"
        "\x01" "VAL" "\x02\x03" "UE"_tb;

    option_.subnegotiate(subnegotiation_content);

    ASSERT_EQ(std::size_t{1}, responses_.size());

    ASSERT_EQ(telnetpp::options::new_environ::variable_type::uservar, responses_[0].type);
    ASSERT_EQ("TEST"_tb, responses_[0].name);
    ASSERT_TRUE(responses_[0].value.has_value());
    ASSERT_EQ("VAL\x03UE"_tb, *responses_[0].value);
}
