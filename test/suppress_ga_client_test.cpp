#include "telnetpp/options/suppress_ga/client.hpp"
#include "telnetpp/options/suppress_ga.hpp"
#include <gtest/gtest.h>

TEST(suppress_ga_client_test, option_is_suppress_ga)
{
    telnetpp::options::suppress_ga::client client;
    ASSERT_EQ(telnetpp::options::suppress_ga::option, client.option());
}
