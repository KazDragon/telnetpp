#include "telnetpp/options/echo/client.hpp"
#include "telnetpp/options/echo.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class echo_client_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(echo_client_test);
        CPPUNIT_TEST(option_is_echo);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void option_is_echo();
    void valid_subnegotiation_signals_window_size_change();
};

CPPUNIT_TEST_SUITE_REGISTRATION(echo_client_test);

void echo_client_test::option_is_echo()
{
    telnetpp::options::echo::client client;
    CPPUNIT_ASSERT_EQUAL(telnetpp::options::echo::option, client.option());
}
