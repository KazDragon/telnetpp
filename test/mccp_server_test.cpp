#include "telnetpp/options/mccp/server.hpp"
#include <gtest/gtest.h>

TEST(mccp_server_test, server_is_mccp)
{
    telnetpp::options::mccp::server server;

    ASSERT_EQ(86, server.option());
}
