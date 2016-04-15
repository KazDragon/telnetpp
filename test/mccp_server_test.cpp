#include "telnetpp/options/mccp/server.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include <gtest/gtest.h>

TEST(mccp_server_test, server_is_mccp)
{
    telnetpp::options::mccp::server server;

    ASSERT_EQ(86, server.option());
}

TEST(mccp_server_test, requesting_activation_returns_block_token)
{
    telnetpp::options::mccp::server server;
    auto tokens = server.activate();

    // First token will be the WILL MCCP response.
    // Second token will be the block token to stop any further traffic until
    // compression has been accepted or rejected.
    ASSERT_EQ(2, tokens.size());

    auto *any = boost::get<boost::any>(&tokens[1]);
    ASSERT_TRUE(any != nullptr);

    auto *token = boost::any_cast<telnetpp::options::mccp::block_token>(any);
    ASSERT_TRUE(token != nullptr);
}