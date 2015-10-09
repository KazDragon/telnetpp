#include "telnetpp/options/terminal_type/client.hpp"
#include "telnetpp/options/terminal_type.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_tokens.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class terminal_type_client_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(terminal_type_client_test);
        CPPUNIT_TEST(option_is_terminal_type);
        CPPUNIT_TEST(requesting_terminal_type_sends_terminal_type_request);
        CPPUNIT_TEST(receiving_terminal_type_reports_terminal_type);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void option_is_terminal_type();
    void requesting_terminal_type_sends_terminal_type_request();
    void receiving_terminal_type_reports_terminal_type();
};

CPPUNIT_TEST_SUITE_REGISTRATION(terminal_type_client_test);

void terminal_type_client_test::option_is_terminal_type()
{
    telnetpp::options::terminal_type::client client;
    CPPUNIT_ASSERT_EQUAL(
        telnetpp::options::terminal_type::option, 
        client.option());
}

void terminal_type_client_test::requesting_terminal_type_sends_terminal_type_request()
{
    telnetpp::options::terminal_type::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    expect_tokens(
        { 
            telnetpp::subnegotiation(
                telnetpp::options::terminal_type::option,
                { telnetpp::options::terminal_type::send })
        },
        client.request_terminal_type());
}

void terminal_type_client_test::receiving_terminal_type_reports_terminal_type()
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
            telnetpp::subnegotiation(
                telnetpp::options::terminal_type::option,
                { telnetpp::options::terminal_type::send })
        },
        client.subnegotiate({
            telnetpp::options::terminal_type::is,
            telnetpp::u8(expected[0]),
            telnetpp::u8(expected[1]),
            telnetpp::u8(expected[2])}));
    
    CPPUNIT_ASSERT_EQUAL(expected, terminal_type);
}
