#include "telnetpp/options/msdp/server.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(msdp_server_test, send_with_empty_list_sends_nothing)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {},
        server.send({}));
}

TEST(msdp_server_test, send_with_variable_sends_variable)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation{
                telnetpp::options::msdp::option,
                { 1, 'v', 'a', 'r', 2, 'v', 'a', 'l' }
            }
        },
        server.send({ { "var", "val" }}));
}