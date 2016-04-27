#include "expect_elements.hpp"
#include <telnetpp/options/mccp/server.hpp>
#include <telnetpp/options/mccp/mccp.hpp>
#include <telnetpp/protocol.hpp>
#include <gtest/gtest.h>

TEST(mccp_server_test, server_is_mccp)
{
    telnetpp::options::mccp::server server;

    ASSERT_EQ(86, server.option());
}

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
            server.option(),
            { server.option() })),
        boost::any(telnetpp::options::mccp::begin_compression{})
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
            server.option(),
            { server.option() })),
        boost::any(telnetpp::options::mccp::begin_compression{})
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
            server.option(),
            { server.option() })),
        boost::any(telnetpp::options::mccp::begin_compression{})
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
        boost::any(telnetpp::options::mccp::end_compression{})
    };
    
    auto const result = server.end_compression();
    
    expect_tokens(expected, result);
}
