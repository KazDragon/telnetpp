#include "telnetpp/options/echo/client.hpp"
#include "telnet_option_fixture.hpp"
#include <gtest/gtest.h>

using namespace telnetpp::literals;

namespace {

using an_echo_client = a_telnet_option<telnetpp::options::echo::client>;

}

TEST_F(an_echo_client, is_an_echo_client)
{
    ASSERT_EQ(1, option_.option_code());
}
