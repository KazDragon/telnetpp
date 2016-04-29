#include "expect_elements.hpp"
#include <telnetpp/protocol.hpp>
#include <telnetpp/options/mccp/client.hpp>
#include <telnetpp/options/mccp/mccp.hpp>
#include <gtest/gtest.h>

TEST(mccp_client_test, client_is_mccp)
{
    telnetpp::options::mccp::client client;

    ASSERT_EQ(86, client.option());
}

TEST(mccp_client_test, empty_subnegotiation_returns_begin_decompression)
{
    telnetpp::options::mccp::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    auto const expected = std::vector<telnetpp::token> {
        boost::any(telnetpp::options::mccp::begin_decompression{})
    };

    expect_tokens(expected, client.subnegotiate({}));
}