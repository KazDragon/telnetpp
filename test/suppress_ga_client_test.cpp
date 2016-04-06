#include "telnetpp/options/suppress_ga/client.hpp"
#include <gtest/gtest.h>

TEST(suppress_ga_client_test, option_is_suppress_ga)
{
    telnetpp::options::suppress_ga::client client;
    ASSERT_EQ(3, client.option());
}
