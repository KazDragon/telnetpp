#include "telnet_option_fixture.hpp"

#include <gtest/gtest.h>
#include <telnetpp/options/naws/client.hpp>

namespace {
using a_naws_client = a_telnet_option<telnetpp::options::naws::client>;
}

TEST_F(a_naws_client, is_a_naws_client)
{
    ASSERT_EQ(31, option_.option_code());
}

namespace {

class an_active_naws_client : public a_naws_client
{
protected:
    an_active_naws_client()
    {
        option_.negotiate(telnetpp::will);
        assert(option_.active());
    }
};

}  // namespace

TEST_F(
    an_active_naws_client,
    remote_window_size_change_signals_local_window_size_change)
{
    telnetpp::options::naws::client::window_dimension width = 0;
    telnetpp::options::naws::client::window_dimension height = 0;

    option_.on_window_size_changed.connect(
        [&width, &height](auto new_width, auto new_height) {
            width = new_width;
            height = new_height;
        });

    static constexpr telnetpp::byte const content[] = {0x01, 0x02, 0x03, 0x04};

    option_.subnegotiate(content);

    telnetpp::options::naws::client::window_dimension const expected_width =
        0x01 << 8 | 0x02;
    telnetpp::options::naws::client::window_dimension const expected_height =
        0x03 << 8 | 0x04;

    ASSERT_EQ(expected_width, width);
    ASSERT_EQ(expected_height, height);
}

TEST_F(an_active_naws_client, short_subnegotiation_is_ignored)
{
    bool called = false;
    option_.on_window_size_changed.connect(
        [&called](auto &&, auto &&) { called = true; });

    static constexpr telnetpp::byte const content[] = {0x01, 0x02, 0x03};

    option_.subnegotiate(content);

    ASSERT_EQ(false, called);
}
