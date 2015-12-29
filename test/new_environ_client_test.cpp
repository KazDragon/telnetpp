#include "telnetpp/options/new_environ/client.hpp"
#include "telnetpp/options/new_environ.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(new_environ_client_test, receiving_request_with_no_content_and_no_cached_variables_returns_empty_list)
{
    telnetpp::options::new_environ::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x00 })
        },
        client.subnegotiate({0x01}));
}

TEST(new_environ_client_test, receiving_request_with_no_content_and_one_cached_variable_returns_variable)
{
    telnetpp::options::new_environ::client client;
    client.set_variable("USER", "TEST");
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x00, 
                  0x00, 'U', 'S', 'E', 'R',
                  0x01, 'T', 'E', 'S', 'T' 
                })
        },
        client.subnegotiate({0x01}));
}