#include <telnetpp/options/mccp/server.hpp>
#include <gtest/gtest.h>

TEST(mccp_server_test, server_is_mccp)
{
    telnetpp::options::mccp::server server;

    ASSERT_EQ(86, server.option_code());
}

/*
TEST(mccp_server_test, deactivated_begin_compression_sends_nothing)
{
    telnetpp::options::mccp::server server;

    auto const expected = std::vector<telnetpp::token> {
    };

    auto const result = server.begin_compression();

    expect_tokens(expected, result);
}

TEST(mccp_server_test, deactivated_begin_compression_then_activate_locally_sends_begin_compression_sequence)
{
    telnetpp::options::mccp::server server;
    server.begin_compression();
    server.activate();

    auto const expected = std::vector<telnetpp::token> {
        telnetpp::element(telnetpp::subnegotiation(
            server.option(), {})),
        boost::any(telnetpp::options::mccp::detail::begin_compression{})
    };

    auto const result = server.negotiate(telnetpp::do_);

    expect_tokens(expected, result);
}

TEST(mccp_server_test, deactivated_begin_compression_then_activate_remotely_sends_begin_compression_sequence)
{
    telnetpp::options::mccp::server server;
    server.begin_compression();
    server.set_activatable();

    auto const expected = std::vector<telnetpp::token> {
        telnetpp::element(telnetpp::negotiation(
            telnetpp::will, server.option())),
        telnetpp::element(telnetpp::subnegotiation(
            server.option(), {})),
        boost::any(telnetpp::options::mccp::detail::begin_compression{})
    };

    auto const result = server.negotiate(telnetpp::do_);

    expect_tokens(expected, result);
}

TEST(mccp_server_test, deactivated_end_compression_sends_nothing)
{
    telnetpp::options::mccp::server server;

    auto const expected = std::vector<telnetpp::token> {
    };

    auto const result = server.end_compression();

    expect_tokens(expected, result);
}

TEST(mccp_server_test, deactivated_end_compression_then_activate_sends_nothing)
{
    telnetpp::options::mccp::server server;
    server.end_compression();

    auto const expected = std::vector<telnetpp::token> {
    };

    server.activate();
    auto const result = server.negotiate(telnetpp::do_);

    expect_tokens(expected, result);
}

TEST(mccp_server_test, deactivated_begin_compression_end_compression_then_activate_sends_nothing)
{
    telnetpp::options::mccp::server server;
    server.begin_compression();
    server.end_compression();

    auto const expected = std::vector<telnetpp::token> {
    };

    server.activate();
    auto const result = server.negotiate(telnetpp::do_);

    expect_tokens(expected, result);
}

TEST(mccp_server_test, activated_begin_compression_sends_begin_compression_sequence)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    assert(server.is_active());

    auto const expected = std::vector<telnetpp::token> {
        telnetpp::element(telnetpp::subnegotiation(
            server.option(), {})),
        boost::any(telnetpp::options::mccp::detail::begin_compression{})
    };

    auto const result = server.begin_compression();

    expect_tokens(expected, result);
}

TEST(mccp_server_test, activated_end_compression_sends_end_compression_token)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    assert(server.is_active());

    auto const expected = std::vector<telnetpp::token> {
        boost::any(telnetpp::options::mccp::detail::end_compression{})
    };

    auto const result = server.end_compression();

    expect_tokens(expected, result);
}

TEST(mccp_server_test, activated_compressed_deactivate_ends_compression)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    server.begin_compression();

    // If we end compression, we expect two tokens to be output.  The first
    // will be the (compressed) WONT MCCP negotiation.  Since the only valid
    // response to this is DONT MCCP by the client, the second token will
    // be an end_compression token, as we can assume that the client will
    // immediately end compression upon reception of the negotiation.
    auto const result = server.deactivate();

    ASSERT_EQ(size_t{2}, result.size());
    auto const &token = boost::get<boost::any>(result[1]);
    boost::any_cast<telnetpp::options::mccp::detail::end_compression>(token);
}

TEST(mccp_server_test, activated_compressed_negotiate_deactivation_ends_compression)
{
    telnetpp::options::mccp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    server.begin_compression();

    // If we end compression, we expect two tokens to be output.  The first
    // will be the (compressed) WONT MCCP negotiation.  Since the only valid
    // response to this is DONT MCCP by the client, the second token will
    // be an end_compression token, as we can assume that the client will
    // immediately end compression upon reception of the negotiation.
    auto const result = server.negotiate(telnetpp::dont);

    ASSERT_EQ(size_t{2}, result.size());
    auto const &token = boost::get<boost::any>(result[1]);
    boost::any_cast<telnetpp::options::mccp::detail::end_compression>(token);
}
*/
