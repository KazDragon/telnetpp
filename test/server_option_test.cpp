#include "telnetpp/server_option.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_tokens.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class server_option_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(server_option_test);
        CPPUNIT_TEST(option_returns_option);
        
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
        
        CPPUNIT_TEST(inactive_subnegotiation_is_ignored);
        CPPUNIT_TEST(active_subnegotiation_is_handled);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void option_returns_option();
    
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
    
    void inactive_subnegotiation_is_ignored();
    void active_subnegotiation_is_handled();
};

CPPUNIT_TEST_SUITE_REGISTRATION(server_option_test);

namespace {
    
class fake_server_option : public telnetpp::server_option
{
public :  
    fake_server_option(telnetpp::u8 option)
      : telnetpp::server_option(option)
    {
    }
        
    boost::signals2::signal<
        std::vector<telnetpp::token> (std::vector<telnetpp::u8> const &content),
        telnetpp::token_combiner
    > on_subnegotiation;
        
private :
    std::vector<telnetpp::token> handle_subnegotiation(
        std::vector<telnetpp::u8> const &content)
    {
        return on_subnegotiation(content);
    }
};

}

void server_option_test::option_returns_option()
{
    fake_server_option server(21);
    CPPUNIT_ASSERT_EQUAL(telnetpp::u8(21), server.option());
}

void server_option_test::deactivated_negotiate_do_responds_with_wont_no_signal()
{
    fake_server_option server(0xA5);

    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::wont, 0xA5) },
        server.negotiate(telnetpp::do_));
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}


void server_option_test::deactivated_negotiate_dont_responds_with_wont_no_signal()
{
    fake_server_option server(0xA5);
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::wont, 0xA5) },
        server.negotiate(telnetpp::dont));
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void server_option_test::deactivated_activate_responds_with_will_no_signal()
{
    fake_server_option server(0xA5);
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::will, 0xA5) },
        server.activate());
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void server_option_test::deactivated_deactivate_responds_with_nothing_with_signal()
{
    fake_server_option server(0xA5);

    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens({}, server.deactivate());
    
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void server_option_test::activating_negotiate_do_responds_with_nothing_is_active_with_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens({}, server.negotiate(telnetpp::do_));
    CPPUNIT_ASSERT_EQUAL(true, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void server_option_test::activating_negotiate_dont_responds_with_nothing_is_inactive_with_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.negotiate(telnetpp::dont));
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void server_option_test::activating_activate_responds_with_nothing_no_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens({}, server.activate());
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void server_option_test::activated_negotiate_do_responds_with_will_is_active_no_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);

    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::will, 0xA5) },
        server.negotiate(telnetpp::do_));
    CPPUNIT_ASSERT_EQUAL(true, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void server_option_test::activated_negotiate_dont_responds_with_wont_is_inactive_no_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::negotiation(telnetpp::wont, 0xA5) },
        server.negotiate(telnetpp::dont));
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void server_option_test::activated_activate_responds_with_nothing_is_active_with_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.activate());
    CPPUNIT_ASSERT_EQUAL(true, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void server_option_test::activated_deactive_responds_with_wont_is_inactive_no_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens(
        { telnetpp::negotiation(telnetpp::wont, 0xA5) },
        server.deactivate());
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void server_option_test::deactivating_negotiate_do_responds_with_nothing_is_active_with_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    server.deactivate();
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens({}, server.negotiate(telnetpp::do_));
    CPPUNIT_ASSERT_EQUAL(true, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void server_option_test::deactivating_negotiate_dont_responds_with_nothing_is_inactive_with_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    server.deactivate();
    
    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens({}, server.negotiate(telnetpp::dont));
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void server_option_test::deactivating_deactivate_responds_with_nothing_is_inactive_no_signal()
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    server.deactivate();

    bool called = false;
    server.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_tokens({}, server.deactivate());
    CPPUNIT_ASSERT_EQUAL(false, server.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void server_option_test::inactive_subnegotiation_is_ignored()
{
    fake_server_option server(0xA5);
    
    bool called = false;
    server.on_subnegotiation.connect(
        [&called](auto) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    server.subnegotiate({0x01, 0x02, 0x03});
    
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void server_option_test::active_subnegotiation_is_handled()
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    
    bool called = false;
    std::vector<telnetpp::u8> content;
    
    server.on_subnegotiation.connect(
        [&called, &content](std::vector<telnetpp::u8> const &new_content)
            -> std::vector<telnetpp::token>
        {
            called = true;
            content = new_content;
            return {};
        });
    
    server.subnegotiate({0x01, 0x02, 0x03});
    
    CPPUNIT_ASSERT_EQUAL(true, called);
    CPPUNIT_ASSERT_EQUAL(size_t(3), content.size());
}
