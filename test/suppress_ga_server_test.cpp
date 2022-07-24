#include "telnetpp/options/suppress_ga/server.hpp"
#include "telnet_option_fixture.hpp"
#include <gtest/gtest.h>

namespace {
using a_suppress_ga_server = a_telnet_option<telnetpp::options::suppress_ga::server>;
}

TEST_F(a_suppress_ga_server, is_a_suppress_ga_server)
{
    ASSERT_EQ(3, option_.option_code());
}
