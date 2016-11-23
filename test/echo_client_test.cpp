#include "telnetpp/options/echo/client.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(echo_client_test, option_is_echo)
{
    telnetpp::options::echo::client client;
    ASSERT_EQ(1, client.option());
}

TEST(echo_client_test, subnegotiation_returns_nothing)
{
    telnetpp::options::echo::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    assert(client.is_active());


    expect_elements({}, client.subnegotiate({ 0x00 }));
}