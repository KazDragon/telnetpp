#include "telnetpp/options/naws/server.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_tokens.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class naws_server_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(naws_server_test);
        // Test the deactivated state
        CPPUNIT_TEST(deactivated_negotiate_do_responds_with_wont);
        CPPUNIT_TEST(deactivated_negotiate_dont_responds_with_wont);
        CPPUNIT_TEST(deactivated_activate_responds_with_will);
        CPPUNIT_TEST(deactivated_deactivate_responds_with_nothing);
        
        // Test the activating state
        CPPUNIT_TEST(activating_negotiate_do_responds_with_nothing_is_active);
        CPPUNIT_TEST(activating_negotiate_dont_responds_with_nothing_is_inactive);
        CPPUNIT_TEST(activating_activate_responds_with_nothing);
        // TODO: CPPUNIT_TEST(activating_deactivate_responds_with_???);

        // Test the active state
    CPPUNIT_TEST_SUITE_END();
    
private :
    void deactivated_negotiate_do_responds_with_wont();
    void deactivated_negotiate_dont_responds_with_wont();
    void deactivated_activate_responds_with_will();
    void deactivated_deactivate_responds_with_nothing();
    
    void activating_negotiate_do_responds_with_nothing_is_active();
    void activating_negotiate_dont_responds_with_nothing_is_inactive();
    void activating_activate_responds_with_nothing();
};

CPPUNIT_TEST_SUITE_REGISTRATION(naws_server_test);

void naws_server_test::deactivated_negotiate_do_responds_with_wont()
{
    telnetpp::options::naws::server server;
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::options::naws::option, telnetpp::wont) },
        server.negotiate(telnetpp::do_));
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}


void naws_server_test::deactivated_negotiate_dont_responds_with_wont()
{
    telnetpp::options::naws::server server;
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::options::naws::option, telnetpp::wont) },
        server.negotiate(telnetpp::dont));
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}

void naws_server_test::deactivated_activate_responds_with_will()
{
    telnetpp::options::naws::server server;
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::options::naws::option, telnetpp::will) },
        server.activate());
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}

void naws_server_test::deactivated_deactivate_responds_with_nothing()
{
    telnetpp::options::naws::server server;
    
    expect_tokens({}, server.deactivate());
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}

void naws_server_test::activating_negotiate_do_responds_with_nothing_is_active()
{
    telnetpp::options::naws::server server;
    server.activate();
    
    expect_tokens({}, server.negotiate(telnetpp::do_));
    CPPUNIT_ASSERT_EQUAL(true, server.is_active());
}

void naws_server_test::activating_negotiate_dont_responds_with_nothing_is_inactive()
{
    telnetpp::options::naws::server server;
    server.activate();
    
    expect_tokens({}, server.negotiate(telnetpp::dont));
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}

void naws_server_test::activating_activate_responds_with_nothing()
{
    telnetpp::options::naws::server server;
    server.activate();
    
    expect_tokens({}, server.activate());
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
}
