#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(new_environ_server_test, requesting_no_variables_requests_empty_list)
{
    // According to the specification, if no variables are specified in a
    // request, then it is equivalent to requesting all the variables.
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 0x01 })
        },
        server.request_variables({}));
}

TEST(new_environ_server_test, requesting_one_var_variable_requests_user_variable)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x00, 'T', 'E', 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::var,
                "TEST"
            }}));
}

TEST(new_environ_server_test, requesting_one_user_variable_requests_user_variable)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                "TEST"
            }}));
}

TEST(new_environ_server_test, requesting_two_variables_requests_two_variables)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x00, 'T', 'E', 'S', 'T', '0',
                    0x03, 'T', 'E', 'S', 'T', '1'
                })
        },
        server.request_variables(
            {
                {
                    telnetpp::options::new_environ::var,
                    "TEST0"
                },
                {
                    telnetpp::options::new_environ::uservar,
                    "TEST1"
                },
                
            }));
}

TEST(new_environ_server_test, requesting_uservar_with_var_in_name_is_escaped)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x00, 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x00ST", 5)
            }}));
}

TEST(new_environ_server_test, requesting_uservar_with_value_in_name_is_escaped)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x01, 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x01ST", 5)
            }}));
}

TEST(new_environ_server_test, requesting_uservar_with_esc_in_name_is_escaped)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x02, 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x02ST", 5)
            }}));
}

TEST(new_environ_server_test, requesting_uservar_with_uservar_in_name_is_escaped)
{
    telnetpp::options::new_environ::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    expect_elements(
        {
            telnetpp::subnegotiation(
                telnetpp::options::new_environ::option,
                { 
                    0x01,
                    0x03, 'T', 'E', 0x02, 0x03, 'S', 'T'
                })
        },
        server.request_variables(
            {{
                telnetpp::options::new_environ::uservar,
                std::string("TE\x03ST", 5)
            }}));
}
