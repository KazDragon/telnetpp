#include "telnetpp/options/naws/server.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_tokens.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class naws_server_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(naws_server_test);
        CPPUNIT_TEST(negotiate_do_responds_with_wont);
        CPPUNIT_TEST(negotiate_dont_responds_with_wont);
        CPPUNIT_TEST(activate_responds_with_will);
        CPPUNIT_TEST(deactivate_responds_with_nothing);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void negotiate_do_responds_with_wont();
    void negotiate_dont_responds_with_wont();
    void activate_responds_with_will();
    void deactivate_responds_with_nothing();
};

CPPUNIT_TEST_SUITE_REGISTRATION(naws_server_test);

void naws_server_test::negotiate_do_responds_with_wont()
{
    telnetpp::options::naws::server server;
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::options::naws::option, telnetpp::wont) },
        server.negotiate(telnetpp::do_));
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}


void naws_server_test::negotiate_dont_responds_with_wont()
{
    telnetpp::options::naws::server server;
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::options::naws::option, telnetpp::wont) },
        server.negotiate(telnetpp::dont));
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}

void naws_server_test::activate_responds_with_will()
{
    telnetpp::options::naws::server server;
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::options::naws::option, telnetpp::will) },
        server.activate());
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}

void naws_server_test::deactivate_responds_with_nothing()
{
    telnetpp::options::naws::server server;
    
    expect_tokens({}, server.deactivate());
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}
