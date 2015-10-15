#include "telnetpp/client_option.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class client_option_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(client_option_test);
        CPPUNIT_TEST(option_returns_option);
        
        // Test the deactivated state
        CPPUNIT_TEST(deactivated_negotiate_will_responds_with_dont_no_signal);
        CPPUNIT_TEST(deactivated_negotiate_wont_responds_with_dont_no_signal);
        CPPUNIT_TEST(deactivated_activate_responds_with_do_no_signal);
        CPPUNIT_TEST(deactivated_deactivate_responds_with_nothing_with_signal);
        
        CPPUNIT_TEST(activatable_deactivated_negotiate_will_responds_with_do_with_signal);
        CPPUNIT_TEST(activatable_deactivated_negotiate_wont_responds_with_dont_no_signal);
        
        // Test the activating state
        CPPUNIT_TEST(activating_negotiate_will_responds_with_nothing_is_active_with_signal);
        CPPUNIT_TEST(activating_negotiate_wont_responds_with_nothing_is_inactive_with_signal);
        CPPUNIT_TEST(activating_activate_responds_with_nothing_no_signal);
        // TODO: CPPUNIT_TEST(activating_deactivate_responds_with_???);

        // Test the active state
        CPPUNIT_TEST(activated_negotiate_will_responds_with_do_is_active_no_signal);
        CPPUNIT_TEST(activated_negotiate_wont_responds_with_dont_is_inactive_no_signal);
        CPPUNIT_TEST(activated_activate_responds_with_nothing_is_active_with_signal);
        CPPUNIT_TEST(activated_deactive_responds_with_dont_is_inactive_no_signal);
        
        CPPUNIT_TEST(deactivating_negotiate_will_responds_with_nothing_is_active_with_signal);
        CPPUNIT_TEST(deactivating_negotiate_wont_responds_with_nothing_is_inactive_with_signal);
        //TODO: CPPUNIT_TEST(deactivating_activate_responds_with_???);
        CPPUNIT_TEST(deactivating_deactivate_responds_with_nothing_is_inactive_no_signal);
        
        CPPUNIT_TEST(inactive_subnegotiation_is_ignored);
        CPPUNIT_TEST(active_subnegotiation_is_handled);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void option_returns_option();
    
    void deactivated_negotiate_will_responds_with_dont_no_signal();
    void deactivated_negotiate_wont_responds_with_dont_no_signal();
    void deactivated_activate_responds_with_do_no_signal();
    void deactivated_deactivate_responds_with_nothing_with_signal();
    
    void activatable_deactivated_negotiate_will_responds_with_do_with_signal();
    void activatable_deactivated_negotiate_wont_responds_with_dont_no_signal();
    
    void activating_negotiate_will_responds_with_nothing_is_active_with_signal();
    void activating_negotiate_wont_responds_with_nothing_is_inactive_with_signal();
    void activating_activate_responds_with_nothing_no_signal();
    
    void activated_negotiate_will_responds_with_do_is_active_no_signal();
    void activated_negotiate_wont_responds_with_dont_is_inactive_no_signal();
    void activated_activate_responds_with_nothing_is_active_with_signal();
    void activated_deactive_responds_with_dont_is_inactive_no_signal();
    
    void deactivating_negotiate_will_responds_with_nothing_is_active_with_signal();
    void deactivating_negotiate_wont_responds_with_nothing_is_inactive_with_signal();
    void deactivating_deactivate_responds_with_nothing_is_inactive_no_signal();
    
    void inactive_subnegotiation_is_ignored();
    void active_subnegotiation_is_handled();
};

CPPUNIT_TEST_SUITE_REGISTRATION(client_option_test);

namespace {
    
class fake_client_option : public telnetpp::client_option
{
public :  
    fake_client_option(telnetpp::u8 option)
      : telnetpp::client_option(option)
    {
    }
        
    boost::signals2::signal<
        std::vector<telnetpp::token> (telnetpp::u8stream const &content),
        telnetpp::token_combiner
    > on_subnegotiation;
        
private :
    std::vector<telnetpp::token> handle_subnegotiation(
        telnetpp::u8stream const &content)
    {
        return on_subnegotiation(content);
    }
};

}

void client_option_test::option_returns_option()
{
    fake_client_option client(21);
    CPPUNIT_ASSERT_EQUAL(telnetpp::u8(21), client.option());
}

void client_option_test::deactivated_negotiate_will_responds_with_dont_no_signal()
{
    fake_client_option client(0xA5);
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements(
        { telnetpp::negotiation(telnetpp::dont, 0xA5) },
        client.negotiate(telnetpp::will));
    
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void client_option_test::deactivated_negotiate_wont_responds_with_dont_no_signal()
{
    fake_client_option client(0xA5);

    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements(
        { telnetpp::negotiation(telnetpp::dont, 0xA5) },
        client.negotiate(telnetpp::wont));
    
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void client_option_test::deactivated_activate_responds_with_do_no_signal()
{
    fake_client_option client(0xA5);

    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements(
        { telnetpp::negotiation(telnetpp::do_, 0xA5) },
        client.activate());
    
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void client_option_test::deactivated_deactivate_responds_with_nothing_with_signal()
{
    fake_client_option client(0xA5);

    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements({}, client.deactivate());
    
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void client_option_test::activatable_deactivated_negotiate_will_responds_with_do_with_signal()
{
    fake_client_option client(0xA5);
    client.set_activatable();
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements(
        { telnetpp::negotiation(telnetpp::do_, 0xA5) },
        client.negotiate(telnetpp::will));
    
    CPPUNIT_ASSERT_EQUAL(true, called);
    CPPUNIT_ASSERT_EQUAL(true, client.is_active());
}

void client_option_test::activatable_deactivated_negotiate_wont_responds_with_dont_no_signal()
{
    fake_client_option client(0xA5);
    client.set_activatable();
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements(
        { telnetpp::negotiation(telnetpp::dont, 0xA5) }, 
        client.negotiate(telnetpp::wont));
    
    CPPUNIT_ASSERT_EQUAL(false, called);
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
}

void client_option_test::activating_negotiate_will_responds_with_nothing_is_active_with_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements({}, client.negotiate(telnetpp::will));
    CPPUNIT_ASSERT_EQUAL(true, client.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void client_option_test::activating_negotiate_wont_responds_with_nothing_is_inactive_with_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_elements({}, client.negotiate(telnetpp::wont));
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void client_option_test::activating_activate_responds_with_nothing_no_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements({}, client.activate());
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void client_option_test::activated_negotiate_will_responds_with_do_is_active_no_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);

    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements(
        { telnetpp::negotiation(telnetpp::do_, 0xA5) },
        client.negotiate(telnetpp::will));
    CPPUNIT_ASSERT_EQUAL(true, client.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void client_option_test::activated_negotiate_wont_responds_with_dont_is_inactive_no_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_elements(
        { telnetpp::negotiation(telnetpp::dont, 0xA5) },
        client.negotiate(telnetpp::wont));
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void client_option_test::activated_activate_responds_with_nothing_is_active_with_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_elements({}, client.activate());
    CPPUNIT_ASSERT_EQUAL(true, client.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void client_option_test::activated_deactive_responds_with_dont_is_inactive_no_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements(
        { telnetpp::negotiation(telnetpp::dont, 0xA5) },
        client.deactivate());
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void client_option_test::deactivating_negotiate_will_responds_with_nothing_is_active_with_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    client.deactivate();
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements({}, client.negotiate(telnetpp::will));
    CPPUNIT_ASSERT_EQUAL(true, client.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void client_option_test::deactivating_negotiate_wont_responds_with_nothing_is_inactive_with_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    client.deactivate();
    
    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements({}, client.negotiate(telnetpp::wont));
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(true, called);
}

void client_option_test::deactivating_deactivate_responds_with_nothing_is_inactive_no_signal()
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    client.deactivate();

    bool called = false;
    client.on_state_changed.connect(
        [&called]() -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    expect_elements({}, client.deactivate());
    CPPUNIT_ASSERT_EQUAL(false, client.is_active());
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void client_option_test::inactive_subnegotiation_is_ignored()
{
    fake_client_option client(0xA5);
    
    bool called = false;
    client.on_subnegotiation.connect(
        [&called](auto) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });
    
    client.subnegotiate({0x01, 0x02, 0x03});
    
    CPPUNIT_ASSERT_EQUAL(false, called);
}

void client_option_test::active_subnegotiation_is_handled()
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    
    bool called = false;
    telnetpp::u8stream content;
    
    client.on_subnegotiation.connect(
        [&called, &content](telnetpp::u8stream const &new_content)
            -> std::vector<telnetpp::token>
        {
            called = true;
            content = new_content;
            return {};
        });
    
    client.subnegotiate({0x01, 0x02, 0x03});
    
    CPPUNIT_ASSERT_EQUAL(true, called);
    CPPUNIT_ASSERT_EQUAL(size_t(3), content.size());
}
