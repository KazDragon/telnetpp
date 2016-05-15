#include "telnetpp/client_option.hpp"
#include "telnetpp/protocol.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

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

TEST(client_option_test, option_returns_option)
{
    fake_client_option client(21);
    ASSERT_EQ(telnetpp::u8(21), client.option());
}

TEST(client_option_test, deactivated_negotiate_will_responds_with_dont_no_signal)
{
    fake_client_option client(0xA5);

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::dont, 0xA5)) },
        client.negotiate(telnetpp::will));

    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(false, called);
}


TEST(client_option_test, deactivated_negotiate_wont_responds_with_dont_no_signal)
{
    fake_client_option client(0xA5);

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::dont, 0xA5)) },
        client.negotiate(telnetpp::wont));

    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(false, called);
}

TEST(client_option_test, deactivated_activate_responds_with_do_and_activating)
{
    fake_client_option client(0xA5);

    auto state = telnetpp::client_option::state::inactive;
    bool called = false;
    client.on_state_changed.connect(
        [&called, &state](telnetpp::client_option::state new_state)
            -> std::vector<telnetpp::token>
        {
            called = true;
            state = new_state;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::do_, 0xA5)) },
        client.activate());

    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(true, called);
    ASSERT_EQ(telnetpp::client_option::state::activating, state);
}

TEST(client_option_test, deactivated_deactivate_responds_with_nothing_with_signal)
{
    fake_client_option client(0xA5);

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, client.deactivate());

    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(true, called);
}

TEST(client_option_test, activatable_deactivated_negotiate_do_responds_with_will_with_signal)
{
    fake_client_option client(0xA5);
    client.set_activatable();

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::do_, 0xA5)) },
        client.negotiate(telnetpp::will));

    ASSERT_EQ(true, called);
    ASSERT_EQ(true, client.is_active());

}

TEST(client_option_test, activatable_deactivated_negotiate_dont_responds_with_wont_no_signal)
{
    fake_client_option client(0xA5);
    client.set_activatable();

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::dont, 0xA5)) },
        client.negotiate(telnetpp::wont));

    ASSERT_EQ(false, called);
    ASSERT_EQ(false, client.is_active());

}

TEST(client_option_test, activating_negotiate_will_responds_with_nothing_is_active_with_signal)
{
    fake_client_option client(0xA5);
    client.activate();

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, client.negotiate(telnetpp::will));
    ASSERT_EQ(true, client.is_active());
    ASSERT_EQ(true, called);
}

TEST(client_option_test, activating_negotiate_wont_responds_with_nothing_is_inactive_with_signal)
{
    fake_client_option client(0xA5);
    client.activate();

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, client.negotiate(telnetpp::wont));
    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(true, called);
}

TEST(client_option_test, activating_activate_responds_with_nothing_no_signal)
{
    fake_client_option client(0xA5);
    client.activate();

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, client.activate());
    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(false, called);
}

TEST(client_option_test, activated_negotiate_will_responds_with_do_is_active_no_signal)
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::do_, 0xA5)) },
        client.negotiate(telnetpp::will));
    ASSERT_EQ(true, client.is_active());
    ASSERT_EQ(false, called);
}

TEST(client_option_test, activated_negotiate_wont_responds_with_dont_is_inactive_with_signal)
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);

    auto state = telnetpp::client_option::state::inactive;
    bool called = false;
    client.on_state_changed.connect(
        [&called, &state](telnetpp::client_option::state new_state)
            -> std::vector<telnetpp::token>
        {
            called = true;
            state = new_state;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::dont, 0xA5)) },
        client.negotiate(telnetpp::wont));
    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(true, called);
    ASSERT_EQ(telnetpp::client_option::state::inactive, state);
}

TEST(client_option_test, activated_activate_responds_with_nothing_is_active_with_signal)
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, client.activate());
    ASSERT_EQ(true, client.is_active());
    ASSERT_EQ(true, called);
}

TEST(client_option_test, activated_deactive_responds_with_dont_is_inactive_with_signal)
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);

    auto state = telnetpp::client_option::state::inactive;
    bool called = false;
    client.on_state_changed.connect(
        [&called, &state](telnetpp::client_option::state new_state)
            -> std::vector<telnetpp::token>
        {
            called = true;
            state = new_state;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::dont, 0xA5)) },
        client.deactivate());
    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(true, called);
    ASSERT_EQ(telnetpp::client_option::state::deactivating, state);
}

TEST(client_option_test, deactivating_negotiate_will_responds_with_nothing_is_active_with_signal)
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    client.deactivate();

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, client.negotiate(telnetpp::will));
    ASSERT_EQ(true, client.is_active());
    ASSERT_EQ(true, called);
}

TEST(client_option_test, deactivating_negotiate_wont_responds_with_nothing_is_inactive_with_signal)
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    client.deactivate();

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, client.negotiate(telnetpp::wont));
    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(true, called);
}

TEST(client_option_test, deactivating_deactivate_responds_with_nothing_is_inactive_no_signal)
{
    fake_client_option client(0xA5);
    client.activate();
    client.negotiate(telnetpp::will);
    client.deactivate();

    bool called = false;
    client.on_state_changed.connect(
        [&called](telnetpp::client_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, client.deactivate());
    ASSERT_EQ(false, client.is_active());
    ASSERT_EQ(false, called);
}

TEST(client_option_test, inactive_subnegotiation_is_ignored)
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

    ASSERT_EQ(false, called);
}

TEST(client_option_test, active_subnegotiation_is_handled)
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

    ASSERT_EQ(true, called);
    ASSERT_EQ(size_t(3), content.size());
}
