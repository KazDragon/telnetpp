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

TEST(msdp_server_test, send_with_variable_sends_simple_variable)
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
        server.send({ { "var", "val" } }));
}

TEST(msdp_server_test, send_with_array_sends_array_variable)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    auto variable = telnetpp::options::msdp::variable{
        "var",
        { "val0", "val1" }
    };
    
    expect_elements(
        {
            telnetpp::subnegotiation{
                telnetpp::options::msdp::option,
                { 1, 'v', 'a', 'r', 
                  2, 
                      5, 
                          2, 'v', 'a', 'l', '0', 
                          2, 'v', 'a', 'l', '1',
                      6
                }
            }
        },
        server.send({ variable }));
}

TEST(msdp_server_test, send_with_table_sends_table_variable)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    auto variable = telnetpp::options::msdp::variable{
        "var",
        {{
            "tbl", 
            { "val0", "val1" }
        }}
    };
    
    expect_elements(
        {
            telnetpp::subnegotiation{
                telnetpp::options::msdp::option,
                { 1, 'v', 'a', 'r', 
                  2,  3,
                          1, 't', 'b', 'l',
                          2, 5,
                                  2, 'v', 'a', 'l', '0', 
                                  2, 'v', 'a', 'l', '1',
                             6,
                      4
                }
            }
        },
        server.send({ variable }));
}

TEST(msdp_server_test, send_with_many_items_sends_many_items)
{
    telnetpp::options::msdp::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    auto variable0 = telnetpp::options::msdp::variable{"svar", "sval"};

    auto variable1 = telnetpp::options::msdp::variable{
        "avar", { "aval0", "aval1" }
    };

    auto variable2 = telnetpp::options::msdp::variable{
        "var",
        {{
            "tbl", 
            { "val0", "val1" }
        }}
    };
 
    expect_elements(
        {
            telnetpp::subnegotiation{
                telnetpp::options::msdp::option,
                { 1, 's', 'v', 'a', 'r',
                  2, 's', 'v', 'a', 'l',
                  1, 'a', 'v', 'a', 'r',
                  2, 5, 2, 'a', 'v', 'a', 'l', '0',
                        2, 'a', 'v', 'a', 'l', '1',
                     6,
                  1, 'v', 'a', 'r', 
                  2,  3,
                          1, 't', 'b', 'l',
                          2, 5,
                                  2, 'v', 'a', 'l', '0', 
                                  2, 'v', 'a', 'l', '1',
                             6,
                      4
                }
            }
        },
        server.send({ variable0, variable1, variable2 }));
   
}