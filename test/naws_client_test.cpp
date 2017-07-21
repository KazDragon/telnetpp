#include "telnetpp/options/naws/client.hpp"
#include "telnetpp/protocol.hpp"
#include <gtest/gtest.h>

TEST(naws_client_test, option_is_naws)
{
    telnetpp::options::naws::client client;
    ASSERT_EQ(31, client.option());
}

TEST(naws_client_test, valid_subnegotiation_signals_window_size_change)
{
    telnetpp::options::naws::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::options::naws::client::window_dimension width = 0, height = 0;

    client.on_window_size_changed.connect(
        [&width, &height](auto new_width, auto new_height)
            -> std::vector<telnetpp::token>
        {
            width = new_width;
            height = new_height;
            return {};
        });

    client.subnegotiate({0x01, 0x02, 0x03, 0x04});

    telnetpp::options::naws::client::window_dimension const expected_width =
        0x01 << 8 | 0x02;
    telnetpp::options::naws::client::window_dimension const expected_height =
        0x03 << 8 | 0x04;

    ASSERT_EQ(expected_width, width);
    ASSERT_EQ(expected_height, height);
}

TEST(naws_client_test, short_subnegotiation_is_ignored)
{
    telnetpp::options::naws::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    bool called = false;
    client.on_window_size_changed.connect(
        [&called](auto &&, auto&&) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

   client.subnegotiate({0x01, 0x02, 0x03});

   ASSERT_EQ(false, called);
}
