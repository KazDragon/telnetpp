#include "telnet_option_fixture.hpp"

#include <gtest/gtest.h>
#include <telnetpp/options/naws/server.hpp>

namespace {
using a_naws_server = a_telnet_option<telnetpp::options::naws::server>;
}

TEST_F(a_naws_server, is_a_naws_server)
{
    ASSERT_EQ(31, option_.option_code());
}

TEST_F(
    a_naws_server, on_activation_with_no_screen_size_does_not_send_screen_size)
{
    option_.activate();
    channel_.written_.clear();

    option_.negotiate(telnetpp::do_);
    assert(option_.active());

    telnetpp::byte_storage const expected_result = {};
    ASSERT_EQ(expected_result, channel_.written_);
}

TEST_F(a_naws_server, on_activation_with_screen_size_sends_screen_size)
{
    option_.set_window_size(80, 24);

    option_.activate();
    channel_.written_.clear();

    option_.negotiate(telnetpp::do_);
    assert(option_.active());

    telnetpp::byte_storage const expected_content = {
        telnetpp::iac,
        telnetpp::sb,
        option_.option_code(),
        0,
        80,
        0,
        24,
        telnetpp::iac,
        telnetpp::se};
    ASSERT_EQ(expected_content, channel_.written_);
}

TEST_F(a_naws_server, setting_screen_size_when_activated_sends_screen_size)
{
    option_.activate();
    channel_.written_.clear();

    option_.negotiate(telnetpp::do_);
    assert(option_.active());

    option_.set_window_size(80, 24);

    telnetpp::byte_storage const expected_content = {
        telnetpp::iac,
        telnetpp::sb,
        option_.option_code(),
        0,
        80,
        0,
        24,
        telnetpp::iac,
        telnetpp::se};
    ASSERT_EQ(expected_content, channel_.written_);
}
