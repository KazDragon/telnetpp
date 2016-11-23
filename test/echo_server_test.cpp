#include "telnetpp/options/echo/server.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(echo_server_test, option_is_echo)
{
    telnetpp::options::echo::server server;
    ASSERT_EQ(1, server.option());
}

TEST(echo_server_test, subnegotiation_returns_nothing)
{
    telnetpp::options::echo::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    assert(server.is_active());


    expect_elements({}, server.subnegotiate({ 0x00 }));
}