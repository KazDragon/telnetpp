#include "telnetpp/options/naws/server.hpp"
#include "telnetpp/options/naws.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(naws_server_test, option_is_naws)
{
    telnetpp::options::naws::server server;
    ASSERT_EQ(telnetpp::options::naws::option, server.option());
}

TEST(naws_server_test, activation_with_no_screen_size_sends_nothing)
{
    telnetpp::options::naws::server server;
    server.activate();
    auto result = server.negotiate(telnetpp::do_);
    
    ASSERT_EQ(size_t{0}, result.size());
}

TEST(naws_server_test, setting_screen_size_when_not_activated_sends_nothing)
{
    telnetpp::options::naws::server server;
    auto result = server.set_window_size(80, 24);
    
    ASSERT_EQ(size_t{0}, result.size());
}

TEST(naws_server_test, activation_with_screen_size_sends_screen_size)
{
    telnetpp::options::naws::server server;
    server.set_window_size(80, 24);
    server.activate();

    expect_elements(
        { telnetpp::subnegotiation(
            telnetpp::options::naws::option,
            { 0, 80, 0, 24 })
        },
        server.negotiate(telnetpp::do_));
}

TEST(naws_server_test, setting_screen_size_when_activated_sends_screen_size)
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    expect_elements(
        { telnetpp::subnegotiation(
            telnetpp::options::naws::option,
            { 0, 80, 0, 24 })
        },
        server.set_window_size(80, 24));
}
