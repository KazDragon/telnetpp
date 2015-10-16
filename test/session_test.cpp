#include "telnetpp/session.hpp"
#include "telnetpp/options/echo.hpp"
#include "telnetpp/options/echo/client.hpp"
#include "telnetpp/options/echo/server.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class session_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(session_test);
        CPPUNIT_TEST(reception_of_text_routes_to_user_supplied_function);
        CPPUNIT_TEST(reception_of_negotiation_routes_to_installed_client_option);
        CPPUNIT_TEST(reception_of_negotiation_routes_to_installed_server_option);
        CPPUNIT_TEST(reception_of_command_routes_to_installed_command_function);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void reception_of_text_routes_to_user_supplied_function();
    void reception_of_negotiation_routes_to_installed_client_option();
    void reception_of_negotiation_routes_to_installed_server_option();
    void reception_of_command_routes_to_installed_command_function();
};

CPPUNIT_TEST_SUITE_REGISTRATION(session_test);

void session_test::reception_of_text_routes_to_user_supplied_function()
{
    std::string result;
    telnetpp::session session(
        [&result](std::string const &text) -> std::vector<telnetpp::token>
        {
            result = text;
            return {};
        });
    
    std::string expected = "TEST STRING";
    session.receive(telnetpp::u8stream{expected.begin(), expected.end()});
    
    CPPUNIT_ASSERT_EQUAL(expected, result);
}

void session_test::reception_of_negotiation_routes_to_installed_client_option()
{
    telnetpp::session session(nullptr);
    telnetpp::options::echo::client echo_client;
    
    echo_client.set_activatable();
    session.install(echo_client);
    
    expect_tokens(
        { telnetpp::element(
            telnetpp::negotiation(
                telnetpp::do_, 
                telnetpp::options::echo::option)) 
        },
        session.receive({ 
            telnetpp::iac, 
            telnetpp::will, 
            telnetpp::options::echo::option 
        }));
}

void session_test::reception_of_negotiation_routes_to_installed_server_option()
{
    telnetpp::session session(nullptr);
    telnetpp::options::echo::server echo_server;
    
    echo_server.set_activatable();
    session.install(echo_server);
    
    expect_tokens(
        { telnetpp::element(
            telnetpp::negotiation(
                telnetpp::will, 
                telnetpp::options::echo::option)) },
        session.receive({
            telnetpp::iac, 
            telnetpp::do_, 
            telnetpp::options::echo::option
        }));
}

void session_test::reception_of_command_routes_to_installed_command_function()
{
    telnetpp::command expected(telnetpp::ayt);
    telnetpp::command result(0x00);
    telnetpp::session session(nullptr);

    session.install(expected, 
        [&result](telnetpp::command const &cmd) -> std::vector<telnetpp::token>
        {
            result = cmd;
            return {};
        });
        
    session.receive({
        telnetpp::iac,
        expected.value()
    });
    
    CPPUNIT_ASSERT_EQUAL(expected, result);
}
