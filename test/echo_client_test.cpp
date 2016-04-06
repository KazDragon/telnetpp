#include "telnetpp/options/echo/client.hpp"
#include <gtest/gtest.h>

TEST(echo_client_test, option_is_echo)
{
    telnetpp::options::echo::client client;
    ASSERT_EQ(1, client.option());
}
