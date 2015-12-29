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

TEST(new_environ_client_test, receiving_request_with_no_content_and_one_cached_user_variable_returns_all_variables)
{
    telnetpp::options::new_environ::client client;
    client.set_user_variable("TEST", "VALUE");
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x00,
                  0x03, 'T', 'E', 'S', 'T',
                  0x01, 'V', 'A', 'L', 'U', 'E'
                })
        },
        client.subnegotiate({0x01}));
}

TEST(new_environ_client_test, receiving_request_with_no_content_and_one_cached_variable_with_empty_value_returns_all_variables)
{
    telnetpp::options::new_environ::client client;
    client.set_variable("USER", "");
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x00, 
                  0x00, 'U', 'S', 'E', 'R',
                  0x01
                })
        },
        client.subnegotiate({0x01}));
}

TEST(new_environ_client_test, receiving_request_with_no_content_and_one_deleted_variable_returns_no_variables)
{
    telnetpp::options::new_environ::client client;
    client.set_variable("USER", "TEST");
    client.delete_variable("USER");
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x00, 
                })
        },
        client.subnegotiate({0x01}));
}

TEST(new_environ_client_test, receiving_request_with_no_content_and_one_deleted_uservariable_returns_no_variables)
{
    telnetpp::options::new_environ::client client;
    client.set_user_variable("TTYPE", "XTERM");
    client.delete_user_variable("TTYPE");
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x00, 
                })
        },
        client.subnegotiate({0x01}));
}

TEST(new_environ_client_test, receiving_request_with_no_content_and_multiple_cached_variables_returns_all_variables)
{
    telnetpp::options::new_environ::client client;
    client.set_variable("PRINTER", "LPT1");
    client.set_variable("USER", "TEST");
    client.set_user_variable("LOCALE", "EN_UK");
    client.set_user_variable("TTYPE", "XTERM");
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x00,
                  0x00, 'P', 'R', 'I', 'N', 'T', 'E', 'R',
                  0x01, 'L', 'P', 'T', '1',
                  0x00, 'U', 'S', 'E', 'R',
                  0x01, 'T', 'E', 'S', 'T',
                  0x03, 'L', 'O', 'C', 'A', 'L', 'E',
                  0x01, 'E', 'N', '_', 'U', 'K',
                  0x03, 'T', 'T', 'Y', 'P', 'E',
                  0x01, 'X', 'T', 'E', 'R', 'M'
                })
        },
        client.subnegotiate({0x01}));
}