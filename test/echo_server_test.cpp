#include "telnetpp/options/echo/server.hpp"
#include "telnetpp/options/echo.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class echo_server_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(echo_server_test);
        CPPUNIT_TEST(option_is_echo);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void option_is_echo();
    void valid_subnegotiation_signals_window_size_change();
};

CPPUNIT_TEST_SUITE_REGISTRATION(echo_server_test);

void echo_server_test::option_is_echo()
{
    telnetpp::options::echo::server server;
    CPPUNIT_ASSERT_EQUAL(telnetpp::options::echo::option, server.option());
}
