#include "telnetpp/options/mccp/server.hpp"
#include "telnetpp/options/mccp/mccp.hpp"
#include "telnetpp/protocol.hpp"
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

TEST(mccp_server_test, completing_activation_returns_resume_compressed_token)
{
    telnetpp::options::mccp::server server;
    server.activate();

    auto tokens = server.negotiate(telnetpp::do_);

    ASSERT_EQ(1, tokens.size());

    auto *any = boost::get<boost::any>(&tokens[0]);
    ASSERT_TRUE(any != nullptr);

    auto *token =
        boost::any_cast<telnetpp::options::mccp::resume_compressed_token>(any);
    ASSERT_TRUE(token != nullptr);
}

TEST(mccp_server_test, rejecting_activation_returns_resume_uncompressed_token)
{
    telnetpp::options::mccp::server server;
    server.activate();

    auto tokens = server.negotiate(telnetpp::dont);

    ASSERT_EQ(1, tokens.size());

    auto *any = boost::get<boost::any>(&tokens[0]);
    ASSERT_TRUE(any != nullptr);

    auto *token =
        boost::any_cast<telnetpp::options::mccp::resume_uncompressed_token>(any);
    ASSERT_TRUE(token != nullptr);
}

TEST(mccp_server_test, deactivating_returns_block_token)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    auto tokens = server.deactivate();

    // In a very specific order, the first token will be the (still compressed)
    // DONT negotiation.
    // The second will be a block token, ensuring that the very next message
    // will not be sent until the result of the negotiation is discovered.
    ASSERT_EQ(2, tokens.size());

    auto *any = boost::get<boost::any>(&tokens[1]);
    ASSERT_TRUE(any != nullptr);

    auto *token = boost::any_cast<telnetpp::options::mccp::block_token>(any);
    ASSERT_TRUE(token != nullptr);
}

TEST(mccp_server_test, activated_remotely_returns_resume_compressed_token)
{
    telnetpp::options::mccp::server server;
    server.set_activatable();

    auto tokens = server.negotiate(telnetpp::do_);

    // Very specifically, the first token should be the response.
    // The second token is resume_compressed.  This ordering means that the
    // response is uncompressed and everything that follows is compressed.
    ASSERT_EQ(2, tokens.size());

    auto *any = boost::get<boost::any>(&tokens[1]);
    ASSERT_TRUE(any != nullptr);

    auto *token =
        boost::any_cast<telnetpp::options::mccp::resume_compressed_token>(any);
    ASSERT_TRUE(token != nullptr);
}

TEST(mccp_server_test, deactivated_remotely_returns_resume_uncompressed_token)
{
    telnetpp::options::mccp::server server;
    server.set_activatable();
    server.negotiate(telnetpp::do_);

    auto tokens = server.negotiate(telnetpp::dont);

    // Very specifically, the first token should be the response.
    // The second token is resume_uncompressed.  This ordering means that the
    // response is compressed and everything that follows is uncompressed.
    ASSERT_EQ(2, tokens.size());

    auto *any = boost::get<boost::any>(&tokens[1]);
    ASSERT_TRUE(any != nullptr);

    auto *token =
        boost::any_cast<telnetpp::options::mccp::resume_uncompressed_token>(any);
    ASSERT_TRUE(token != nullptr);
}