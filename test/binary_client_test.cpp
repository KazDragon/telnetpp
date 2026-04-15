#include "telnet_option_fixture.hpp"

#include <gtest/gtest.h>
#include <telnetpp/options/binary/client.hpp>

using namespace telnetpp::literals;  // NOLINT

namespace {

using an_binary_client = a_telnet_option<telnetpp::options::binary::client>;

}

TEST_F(an_binary_client, is_an_binary_client)
{
    ASSERT_EQ(0, option_.option_code());
}
