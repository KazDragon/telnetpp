#include "telnetpp/options/echo/client.hpp"
#include "telnetpp/options/echo.hpp"
#include <gtest/gtest.h>

TEST(echo_client_test, option_is_echo)
{
    telnetpp::options::echo::client client;
    ASSERT_EQ(telnetpp::options::echo::option, client.option());
}
