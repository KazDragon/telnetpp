#include "telnetpp/options/suppress_ga/client.hpp"
#include "telnet_option_fixture.hpp"
#include <gtest/gtest.h>

namespace {
using a_suppress_ga_client = a_telnet_option<telnetpp::options::suppress_ga::client>;
}

TEST_F(a_suppress_ga_client, is_a_suppress_ga_client)
{
    ASSERT_EQ(3, option_.option_code());
}
