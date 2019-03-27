#include "telnetpp/options/naws/client.hpp"
#include <gtest/gtest.h>

TEST(naws_client_test, option_is_naws)
{
    telnetpp::options::naws::client client;
    ASSERT_EQ(31, client.option_code());
}

TEST(naws_client_test, valid_subnegotiation_signals_window_size_change)
{
    telnetpp::options::naws::client client;
    client.negotiate(telnetpp::will, [](auto &&){});
    assert(client.active());

    telnetpp::options::naws::client::window_dimension width = 0, height = 0;

    client.on_window_size_changed.connect(
        [&width, &height](auto new_width, auto new_height, auto &&cont)
        {
            width = new_width;
            height = new_height;
        });

    static constexpr telnetpp::byte const content[] = {
        0x01, 0x02, 0x03, 0x04
    };

    client.subnegotiate(content, [](auto &&){});

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
    client.negotiate(telnetpp::will, [](auto &&){});
    assert(client.active());

    bool called = false;
    client.on_window_size_changed.connect(
        [&called](auto &&, auto&&, auto &&)
        {
            called = true;
        });

    static constexpr telnetpp::byte const content[] = {
        0x01, 0x02, 0x03
    };

    client.subnegotiate(content, [](auto &&){});

   ASSERT_EQ(false, called);
}
