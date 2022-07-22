#include "telnetpp/options/new_environ/server.hpp"
#include "telnet_option_fixture.hpp"
#include <gtest/gtest.h>

using namespace telnetpp::literals;

namespace {
using a_new_environ_server = a_telnet_option<telnetpp::options::new_environ::server>;
}

TEST_F(a_new_environ_server, is_a_new_environ_server)
{
    ASSERT_EQ(39, option_.option_code());
}

namespace {
class an_active_new_environ_server : public a_new_environ_server
{
protected:
    an_active_new_environ_server()
    {
        option_.negotiate(telnetpp::do_);
        assert(option_.active());
        channel_.written_.clear();
    }
};

}
TEST_F(an_active_new_environ_server, receiving_request_with_no_content_and_no_cached_variables_sends_empty_list)
{
    static auto const request_subnegotiation_content = "\x01"_tb;
    option_.subnegotiate(request_subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);

}

TEST_F(an_active_new_environ_server, receiving_request_with_no_content_and_one_cached_variable_sends_variable)
{
    static auto const name = "USER"_tb;
    static auto const value = "TEST"_tb;

    option_.set_variable(name, value);
    channel_.written_.clear();

    static auto const request_subnegotiation_content = "\x01"_tb;
    option_.subnegotiate(request_subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
            0x00, 'U', 'S', 'E', 'R', // VAR USER
            0x01, 'T', 'E', 'S', 'T', // VAL TEST
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, receiving_request_with_no_content_and_one_cached_user_variable_sends_all_variables)
{
    static auto const name = "USER"_tb;
    static auto const value = "TEST"_tb;

    option_.set_user_variable(name, value);
    channel_.written_.clear();

    static auto const request_subnegotiation_content = "\x01"_tb;
    option_.subnegotiate(request_subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
            0x03, 'U', 'S', 'E', 'R', // USERVAR USER
            0x01, 'T', 'E', 'S', 'T', // VAL TEST
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, receiving_request_with_no_content_and_one_cached_variable_with_empty_value_sends_all_variables)
{
    static auto const name = "USER"_tb;
    static auto const value = ""_tb;

    option_.set_variable(name, value);
    channel_.written_.clear();

    static auto const expected_subnegotiation_content = 
        "\x00"
        "\x00" "USER"
        "\x01"_tb;

    static auto const request_subnegotiation_content = "\x01"_tb;
    option_.subnegotiate(request_subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
            0x00, 'U', 'S', 'E', 'R', // VAR USER
            0x01,                     // VAL
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, receiving_request_with_no_content_and_one_deleted_variable_sends_no_variables)
{
    static auto const name = "USER"_tb;
    static auto const value = "TEST"_tb;

    option_.set_variable(name, value);
    option_.delete_variable(name);
    channel_.written_.clear();

    static auto const request_subnegotiation_content = "\x01"_tb;
    option_.subnegotiate(request_subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, receiving_request_with_no_content_and_one_deleted_uservariable_sends_no_variables)
{
    static auto const name = "TTYPE"_tb;
    static auto const value = "XTERM"_tb;

    option_.set_user_variable(name, value);
    option_.delete_user_variable(name);
    channel_.written_.clear();

    static auto const request_subnegotiation_content = "\x01"_tb;
    option_.subnegotiate(request_subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, receiving_request_with_no_content_and_multiple_cached_variables_sends_all_variables)
{
    static auto const name0 = "PRINTER"_tb;
    static auto const value0 = "LPT1"_tb;
    static auto const name1 = "USER"_tb;
    static auto const value1 = "TEST"_tb;
    static auto const name2 = "LOCALE"_tb;
    static auto const value2 = "EN_GB"_tb;
    static auto const name3 = "TTYPE"_tb;
    static auto const value3 = "XTERM"_tb;

    option_.set_variable(name0, value0);
    option_.set_variable(name1, value1);
    option_.set_user_variable(name2, value2);
    option_.set_user_variable(name3, value3);
    channel_.written_.clear();

    static auto const request_subnegotiation_content = "\x01"_tb;
    option_.subnegotiate(request_subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
        0x00, 'P', 'R', 'I', 'N', 'T', 'E', 'R', 0x01, 'L', 'P', 'T', '1',
        0x00, 'U', 'S', 'E', 'R', 0x01, 'T', 'E', 'S', 'T',
        0x03, 'L', 'O', 'C', 'A', 'L', 'E', 0x01, 'E', 'N', '_', 'G', 'B',
        0x03, 'T', 'T', 'Y', 'P', 'E', 0x01, 'X', 'T', 'E', 'R', 'M',
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, modifying_a_variable_sends_variable_modification)
{
    static auto const name = "USER"_tb;
    static auto const old_value = "FRED"_tb;

    option_.set_variable(name, old_value);
    channel_.written_.clear();

    static auto const new_value = "BOB"_tb;
    option_.set_variable(name, new_value);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x02, // INFO
        0x00, 'U', 'S', 'E', 'R', 0x01, 'B', 'O', 'B',
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, deleting_a_variable_sends_variable_deletion)
{
    static auto const name = "USER"_tb;
    static auto const value = "FRED"_tb;

    option_.set_variable(name, value);
    channel_.written_.clear();

    option_.delete_variable(name);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x02, // INFO
        0x00, 'U', 'S', 'E', 'R',
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, modifying_a_user_variable_sends_variable_modification)
{
    static auto const name = "TEST"_tb;
    static auto const old_value = "VALUE"_tb;

    option_.set_user_variable(name, old_value);
    channel_.written_.clear();

    static auto const new_value = "RESULT"_tb;
    option_.set_user_variable(name, new_value);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x02, // INFO
        0x03, 'T', 'E', 'S', 'T', 0x01, 'R', 'E', 'S', 'U', 'L', 'T',
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, deleting_a_user_variable_sends_variable_deletion)
{
    static auto const name = "TEST"_tb;
    static auto const value = "VALUE"_tb;

    option_.set_user_variable(name, value);
    channel_.written_.clear();

    option_.delete_user_variable(name);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x02, // INFO
        0x03, 'T', 'E', 'S', 'T',
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, requesting_nonexistent_variable_returns_empty_result)
{
    static auto const name = "TEST"_tb;
    static auto const value = "VALUE"_tb;

    option_.set_user_variable(name, value);
    channel_.written_.clear();

    static auto const subnegotiation_content = 
        "\x01\x03" "FAIL"_tb;
    option_.subnegotiate(subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, requesting_existent_variables_sends_variables)
{
    static auto const name0 = "PRINTER"_tb;
    static auto const value0 = "LPT1"_tb;
    static auto const name1 = "USER"_tb;
    static auto const value1 = "TEST"_tb;
    static auto const name2 = "LOCALE"_tb;
    static auto const value2 = "EN_GB"_tb;
    static auto const name3 = "TTYPE"_tb;
    static auto const value3 = "XTERM"_tb;

    option_.set_variable(name0, value0);
    option_.set_variable(name1, value1);
    option_.set_user_variable(name2, value2);
    option_.set_user_variable(name3, value3);
    channel_.written_.clear();

    static auto const subnegotiation_content = 
        "\x01"
        "\x03" "TTYPE"
        "\x00" "USER"_tb;
    option_.subnegotiate(subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
        0x03, 'T', 'T', 'Y', 'P', 'E', 0x01, 'X', 'T', 'E', 'R', 'M',
        0x00, 'U', 'S', 'E', 'R', 0x01, 'T', 'E', 'S', 'T',
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, names_and_values_with_control_characters_are_escaped_properly)
{
    static auto const name0 = "USER"_tb;
    static auto const value0 = "\x01" "ABC"_tb;
    static auto const name1 = "\x00" "DEF"_tb;
    static auto const value1 = "\x02" "GHI"_tb;

    option_.set_variable(name0, value0);
    option_.set_user_variable(name1, value1);
    channel_.written_.clear();

    static auto const subnegotiation_content = "\x01"_tb;
    option_.subnegotiate(subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
        0x00, 'U', 'S', 'E', 'R', 0x01, 0x02, 0x01, 'A', 'B', 'C',
        0x03, 0x02, 0x00, 'D', 'E', 'F', 0x01, 0x02, 0x02, 'G', 'H', 'I',
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}

TEST_F(an_active_new_environ_server, requests_with_escaped_control_characters_are_handled_correctly)
{
    static auto const name = "\x01\x02\x03\x04" "GOOD"_tb;
    static auto const value = "VALUE"_tb;

    option_.set_user_variable(name, value);
    channel_.written_.clear();

    static auto const subnegotiation_content = 
        "\x01\x03" "\x02\x01\x02\x02\x02\x03\x04" "GOOD"_tb;
    option_.subnegotiate(subnegotiation_content);

    static telnetpp::byte_storage const expected_response = {
        telnetpp::iac, telnetpp::sb, option_.option_code(),
        0x00, // IS
        0x03, 0x02, 0x01, 0x02, 0x02, 0x02, 0x03, 0x04, 'G', 'O', 'O', 'D',
        0x01, 'V', 'A', 'L', 'U', 'E',
        telnetpp::iac, telnetpp::se
    };

    ASSERT_EQ(expected_response, channel_.written_);
}
