#include "telnet_option_fixture.hpp"

#include <gtest/gtest.h>
#include <telnetpp/options/binary/server.hpp>

using namespace telnetpp::literals;  // NOLINT

namespace {

using an_binary_server = a_telnet_option<telnetpp::options::binary::server>;

}

TEST_F(an_binary_server, is_an_binary_server)
{
    ASSERT_EQ(0, option_.option_code());
}
