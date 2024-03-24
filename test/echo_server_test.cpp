#include "telnet_option_fixture.hpp"

#include <gtest/gtest.h>
#include <telnetpp/options/echo/server.hpp>

using namespace telnetpp::literals;  // NOLINT

namespace {

using an_echo_server = a_telnet_option<telnetpp::options::echo::server>;

}

TEST_F(an_echo_server, is_an_echo_server)
{
    ASSERT_EQ(1, option_.option_code());
}
