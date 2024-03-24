#include "telnet_option_fixture.hpp"

#include <gtest/gtest.h>
#include <telnetpp/options/terminal_type/client.hpp>

using namespace telnetpp::literals;  // NOLINT

namespace {
using a_terminal_type_client =
    a_telnet_option<telnetpp::options::terminal_type::client>;
}

TEST_F(a_terminal_type_client, is_a_terminal_type_client)
{
    ASSERT_EQ(24, option_.option_code());
}

namespace {

class an_active_terminal_type_client : public a_terminal_type_client
{
   protected:
    an_active_terminal_type_client()
    {
        option_.negotiate(telnetpp::will);
        assert(option_.active());
        channel_.written_.clear();

        option_.on_terminal_type.connect([this](telnetpp::bytes type) {
            received_types_.emplace_back(type.begin(), type.end());
        });
    }

    std::vector<telnetpp::byte_storage> received_types_;
};

}  // namespace

TEST_F(
    an_active_terminal_type_client,
    requesting_terminal_type_sends_terminal_type_request)
{
    option_.request_terminal_type();

    telnetpp::byte_storage const expected_content = {
        telnetpp::iac,
        telnetpp::sb,
        option_.option_code(),
        0x01,  // SEND
        telnetpp::iac,
        telnetpp::se};

    ASSERT_EQ(expected_content, channel_.written_);
}

TEST_F(
    an_active_terminal_type_client,
    receiving_terminal_type_reports_terminal_type)
{
    static constexpr telnetpp::byte const content[] = {0x00, 'a', 'b', 'c'};

    option_.subnegotiate(content);

    ASSERT_EQ(size_t{1U}, received_types_.size());
    ASSERT_EQ("abc"_tb, received_types_[0]);
}
