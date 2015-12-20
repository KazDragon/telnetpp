#include "telnetpp/options/echo/server.hpp"
#include "telnetpp/options/echo.hpp"
#include <gtest/gtest.h>

TEST(echo_server_test, option_is_echo)
{
    telnetpp::options::echo::server server;
    ASSERT_EQ(telnetpp::options::echo::option, server.option());
}
