#include "telnetpp/options/suppress_ga/server.hpp"
#include <gtest/gtest.h>

TEST(suppress_ga_server_test, option_is_suppress_ga)
{
    telnetpp::options::suppress_ga::server server;
    ASSERT_EQ(3, server.option());
}
