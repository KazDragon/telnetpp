#include "telnetpp/options/echo/server.hpp"
#include <gtest/gtest.h>

TEST(echo_server_test, option_is_echo)
{
    telnetpp::options::echo::server server;
    ASSERT_EQ(1, server.option());
}
