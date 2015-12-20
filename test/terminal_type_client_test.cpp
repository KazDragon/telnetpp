#include "telnetpp/options/terminal_type/client.hpp"
#include "telnetpp/options/terminal_type.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(terminal_type_client_test, option_is_terminal_type)
{
    telnetpp::options::terminal_type::client client;
    ASSERT_EQ(
        telnetpp::options::terminal_type::option, 
        client.option());
}

TEST(terminal_type_client_test, requesting_terminal_type_sends_terminal_type_request)
{
    telnetpp::options::terminal_type::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_tokens(
        { 
            telnetpp::element(telnetpp::subnegotiation(
                telnetpp::options::terminal_type::option,
                { telnetpp::options::terminal_type::send }))
        },
        client.request_terminal_type());
}

TEST(terminal_type_client_test, receiving_terminal_type_reports_terminal_type)
{
    telnetpp::options::terminal_type::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    std::string terminal_type;
    client.on_terminal_type.connect(
        [&terminal_type, &client](std::string const &new_terminal_type)
            -> std::vector<telnetpp::token>
        {
            terminal_type = new_terminal_type;
            return client.request_terminal_type();
        });
    
    std::string expected("abc");
    
    expect_tokens(
        {
            telnetpp::element(telnetpp::subnegotiation(
                telnetpp::options::terminal_type::option,
                { telnetpp::options::terminal_type::send }))
        },
        client.subnegotiate({
            telnetpp::options::terminal_type::is,
            telnetpp::u8(expected[0]),
            telnetpp::u8(expected[1]),
            telnetpp::u8(expected[2])}));
    
    ASSERT_EQ(expected, terminal_type);
}
