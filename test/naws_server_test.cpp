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
        CPPUNIT_TEST(deactivated_negotiate_do_responds_with_wont_no_signal);
        CPPUNIT_TEST(deactivated_negotiate_dont_responds_with_wont_no_signal);
        CPPUNIT_TEST(deactivated_activate_responds_with_will_no_signal);
        CPPUNIT_TEST(deactivated_deactivate_responds_with_nothing_with_signal);
        
        // Test the activating state
        CPPUNIT_TEST(activating_negotiate_do_responds_with_nothing_is_active_with_signal);
        CPPUNIT_TEST(activating_negotiate_dont_responds_with_nothing_is_inactive_with_signal);
        CPPUNIT_TEST(activating_activate_responds_with_nothing_no_signal);
        // TODO: CPPUNIT_TEST(activating_deactivate_responds_with_???);

        // Test the active state
        CPPUNIT_TEST(activated_negotiate_do_responds_with_will_is_active_no_signal);
        CPPUNIT_TEST(activated_negotiate_dont_responds_with_wont_is_inactive_no_signal);
        CPPUNIT_TEST(activated_activate_responds_with_nothing_is_active_with_signal);
        CPPUNIT_TEST(activated_deactive_responds_with_wont_is_inactive_no_signal);
        
        CPPUNIT_TEST(deactivating_negotiate_do_responds_with_nothing_is_active_with_signal);
        CPPUNIT_TEST(deactivating_negotiate_dont_responds_with_nothing_is_inactive_with_signal);
        //TODO: CPPUNIT_TEST(deactivating_activate_responds_with_???);
        CPPUNIT_TEST(deactivating_deactivate_responds_with_nothing_is_inactive_no_signal);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void deactivated_negotiate_do_responds_with_wont_no_signal();
    void deactivated_negotiate_dont_responds_with_wont_no_signal();
    void deactivated_activate_responds_with_will_no_signal();
    void deactivated_deactivate_responds_with_nothing_with_signal();
    
    void activating_negotiate_do_responds_with_nothing_is_active_with_signal();
    void activating_negotiate_dont_responds_with_nothing_is_inactive_with_signal();
    void activating_activate_responds_with_nothing_no_signal();
    
    void activated_negotiate_do_responds_with_will_is_active_no_signal();
    void activated_negotiate_dont_responds_with_wont_is_inactive_no_signal();
    void activated_activate_responds_with_nothing_is_active_with_signal();
    void activated_deactive_responds_with_wont_is_inactive_no_signal();
    
    void deactivating_negotiate_do_responds_with_nothing_is_active_with_signal();
    void deactivating_negotiate_dont_responds_with_nothing_is_inactive_with_signal();
    void deactivating_deactivate_responds_with_nothing_is_inactive_no_signal();
};

CPPUNIT_TEST_SUITE_REGISTRATION(naws_server_test);

void naws_server_test::deactivated_negotiate_do_responds_with_wont_no_signal()
{
    telnetpp::options::naws::server server;
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::wont, telnetpp::options::naws::option) },
        server.negotiate(telnetpp::do_));
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}


void naws_server_test::deactivated_negotiate_dont_responds_with_wont_no_signal()
{
    telnetpp::options::naws::server server;
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::wont, telnetpp::options::naws::option) },
        server.negotiate(telnetpp::dont));
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void naws_server_test::deactivated_activate_responds_with_will_no_signal()
{
    telnetpp::options::naws::server server;
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::will, telnetpp::options::naws::option) },
        server.activate());
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void naws_server_test::deactivated_deactivate_responds_with_nothing_with_signal()
{
    telnetpp::options::naws::server server;
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens({}, server.deactivate());
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void naws_server_test::activating_negotiate_do_responds_with_nothing_is_active_with_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens({}, server.negotiate(telnetpp::do_));
    CPPUNIT_ASSERT_EQUAL(true, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void naws_server_test::activating_negotiate_dont_responds_with_nothing_is_inactive_with_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });

    expect_tokens({}, server.negotiate(telnetpp::dont));
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void naws_server_test::activating_activate_responds_with_nothing_no_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens({}, server.activate());
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void naws_server_test::activated_negotiate_do_responds_with_will_is_active_no_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);

    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::will, telnetpp::options::naws::option) },
        server.negotiate(telnetpp::do_));
    CPPUNIT_ASSERT_EQUAL(true, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void naws_server_test::activated_negotiate_dont_responds_with_wont_is_inactive_no_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });

    expect_tokens(
        { telnetpp::negotiation(telnetpp::wont, telnetpp::options::naws::option) },
        server.negotiate(telnetpp::dont));
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void naws_server_test::activated_activate_responds_with_nothing_is_active_with_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });

    expect_tokens({}, server.activate());
    CPPUNIT_ASSERT_EQUAL(true, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void naws_server_test::activated_deactive_responds_with_wont_is_inactive_no_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::wont, telnetpp::options::naws::option) },
        server.deactivate());
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void naws_server_test::deactivating_negotiate_do_responds_with_nothing_is_active_with_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    server.deactivate();
    
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens({}, server.negotiate(telnetpp::do_));
    CPPUNIT_ASSERT_EQUAL(true, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void naws_server_test::deactivating_negotiate_dont_responds_with_nothing_is_inactive_with_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    server.deactivate();
    
    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens({}, server.negotiate(telnetpp::dont));
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void naws_server_test::deactivating_deactivate_responds_with_nothing_is_inactive_no_signal()
{
    telnetpp::options::naws::server server;
    server.activate();
    server.negotiate(telnetpp::do_);
    server.deactivate();

    bool called = false;
    server.on_state_changed.connect([&called]()
    {
        called = true;
    });
    
    expect_tokens({}, server.deactivate());
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}
