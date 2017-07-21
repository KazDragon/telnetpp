#include "telnetpp/server_option.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

namespace {

class fake_server_option : public telnetpp::server_option
{
public :
    explicit fake_server_option(telnetpp::option_type option)
      : telnetpp::server_option(option)
    {
    }

    boost::signals2::signal<
        std::vector<telnetpp::token> (telnetpp::byte_stream const &content),
        telnetpp::token_combiner
    > on_subnegotiation;

private :
    std::vector<telnetpp::token> handle_subnegotiation(
        telnetpp::byte_stream const &content)
    {
        return on_subnegotiation(content);
    }
};

}

TEST(server_option_test, option_returns_option)
{
    fake_server_option server(21);
    ASSERT_EQ(telnetpp::option_type(21), server.option());
}

TEST(server_option_test, deactivated_negotiate_do_responds_with_wont_no_signal)
{
    fake_server_option server(0xA5);

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::wont, 0xA5)) },
        server.negotiate(telnetpp::do_));

    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(false, called);
}


TEST(server_option_test, deactivated_negotiate_dont_responds_with_wont_no_signal)
{
    fake_server_option server(0xA5);

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::wont, 0xA5)) },
        server.negotiate(telnetpp::dont));

    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(false, called);
}

TEST(server_option_test, deactivated_activate_responds_with_will_and_activating)
{
    fake_server_option server(0xA5);

    auto state = telnetpp::server_option::state::inactive;
    bool called = false;
    server.on_state_changed.connect(
        [&called, &state](telnetpp::server_option::state new_state)
            -> std::vector<telnetpp::token>
        {
            called = true;
            state = new_state;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::will, 0xA5)) },
        server.activate());

    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(true, called);
    ASSERT_EQ(telnetpp::server_option::state::activating, state);
}

TEST(server_option_test, deactivated_deactivate_responds_with_nothing_with_signal)
{
    fake_server_option server(0xA5);

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.deactivate());

    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(true, called);
}

TEST(server_option_test, activatable_deactivated_negotiate_will_responds_with_do_with_signal)
{
    fake_server_option server(0xA5);
    server.set_activatable();

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::will, 0xA5)) },
        server.negotiate(telnetpp::do_));

    ASSERT_EQ(true, called);
    ASSERT_EQ(true, server.is_active());

}

TEST(server_option_test, activatable_deactivated_negotiate_wont_responds_with_dont_no_signal)
{
    fake_server_option server(0xA5);
    server.set_activatable();

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::wont, 0xA5)) },
        server.negotiate(telnetpp::dont));

    ASSERT_EQ(false, called);
    ASSERT_EQ(false, server.is_active());

}

TEST(server_option_test, activating_negotiate_do_responds_with_nothing_is_active_with_signal)
{
    fake_server_option server(0xA5);
    server.activate();

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.negotiate(telnetpp::do_));
    ASSERT_EQ(true, server.is_active());
    ASSERT_EQ(true, called);
}

TEST(server_option_test, activating_negotiate_dont_responds_with_nothing_is_inactive_with_signal)
{
    fake_server_option server(0xA5);
    server.activate();

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.negotiate(telnetpp::dont));
    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(true, called);
}

TEST(server_option_test, activating_activate_responds_with_nothing_no_signal)
{
    fake_server_option server(0xA5);
    server.activate();

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.activate());
    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(false, called);
}

TEST(server_option_test, activated_negotiate_do_responds_with_will_is_active_no_signal)
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::will, 0xA5)) },
        server.negotiate(telnetpp::do_));
    ASSERT_EQ(true, server.is_active());
    ASSERT_EQ(false, called);
}

TEST(server_option_test, activated_negotiate_dont_responds_with_wont_is_inactive_with_signal)
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);

    auto state = telnetpp::server_option::state::inactive;
    bool called = false;
    server.on_state_changed.connect(
        [&called, &state](telnetpp::server_option::state new_state)
            -> std::vector<telnetpp::token>
        {
            called = true;
            state = new_state;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::wont, 0xA5)) },
        server.negotiate(telnetpp::dont));
    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(true, called);
    ASSERT_EQ(telnetpp::server_option::state::inactive, state);
}

TEST(server_option_test, activated_activate_responds_with_nothing_is_active_with_signal)
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.activate());
    ASSERT_EQ(true, server.is_active());
    ASSERT_EQ(true, called);
}

TEST(server_option_test, activated_deactive_responds_with_wont_is_inactive_with_signal)
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);

    auto state = telnetpp::server_option::state::inactive;
    bool called = false;
    server.on_state_changed.connect(
        [&called, &state](telnetpp::server_option::state new_state)
            -> std::vector<telnetpp::token>
        {
            called = true;
            state = new_state;
            return {};
        });

    expect_tokens(
        { telnetpp::element(telnetpp::negotiation(telnetpp::wont, 0xA5)) },
        server.deactivate());
    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(true, called);
    ASSERT_EQ(telnetpp::server_option::state::deactivating, state);
}

TEST(server_option_test, deactivating_negotiate_do_responds_with_nothing_is_active_with_signal)
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    server.deactivate();

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.negotiate(telnetpp::do_));
    ASSERT_EQ(true, server.is_active());
    ASSERT_EQ(true, called);
}

TEST(server_option_test, deactivating_negotiate_dont_responds_with_nothing_is_inactive_with_signal)
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    server.deactivate();

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.negotiate(telnetpp::dont));
    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(true, called);
}

TEST(server_option_test, deactivating_deactivate_responds_with_nothing_is_inactive_no_signal)
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);
    server.deactivate();

    bool called = false;
    server.on_state_changed.connect(
        [&called](telnetpp::server_option::state) -> std::vector<telnetpp::token>
        {
            called = true;
            return {};
        });

    expect_tokens({}, server.deactivate());
    ASSERT_EQ(false, server.is_active());
    ASSERT_EQ(false, called);
}

TEST(server_option_test, inactive_subnegotiation_is_ignored)
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

    ASSERT_EQ(false, called);
}

TEST(server_option_test, active_subnegotiation_is_handled)
{
    fake_server_option server(0xA5);
    server.activate();
    server.negotiate(telnetpp::do_);

    bool called = false;
    telnetpp::byte_stream content;

    server.on_subnegotiation.connect(
        [&called, &content](telnetpp::byte_stream const &new_content)
            -> std::vector<telnetpp::token>
        {
            called = true;
            content = new_content;
            return {};
        });

    server.subnegotiate({0x01, 0x02, 0x03});

    ASSERT_EQ(true, called);
    ASSERT_EQ(size_t(3), content.size());
}
