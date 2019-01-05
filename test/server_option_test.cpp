#include "telnetpp/server_option.hpp"
#include <gtest/gtest.h>

namespace {

class fake_server_option : public telnetpp::server_option
{
public:
    explicit fake_server_option(telnetpp::option_type option)
      : telnetpp::server_option(option)
    {
    }

    boost::signals2::signal<
        void (telnetpp::bytes data)
    > on_subnegotiation;

private:
    void handle_subnegotiation(
        telnetpp::bytes data,
        std::function<void (telnetpp::element const &)> const &cont) override
    {
        on_subnegotiation(data);
    }
};

}

TEST(server_option_test, code_returns_option_code)
{
    fake_server_option server{21};
    ASSERT_EQ(telnetpp::option_type{21}, server.option_code());
}

TEST(server_option_test, deactivated_negotiate_do_sends_will_with_signal)
{
    fake_server_option server(0xA5);

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::element{telnetpp::negotiation{telnetpp::will, 0xA5}}
    };

    std::vector<telnetpp::element> received_elements;
    server.negotiate(
        telnetpp::do_, 
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_TRUE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, deactivated_negotiate_dont_sends_wont_no_signal)
{
    fake_server_option server(0xA5);

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::wont, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    server.negotiate(
        telnetpp::dont, 
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_FALSE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, deactivated_activate_sends_will_no_signal)
{
    fake_server_option server(0xA5);

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::will, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    server.activate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_FALSE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, deactivated_deactivate_sends_nothing_with_signal)
{
    fake_server_option server(0xA5);

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.deactivate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, activating_negotiate_do_sends_nothing_is_active_with_signal)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.negotiate(
        telnetpp::do_,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_TRUE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, activating_negotiate_dont_sends_nothing_is_inactive_with_signal)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.negotiate(
        telnetpp::dont,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, activating_activate_sends_nothing_no_signal)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.activate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_FALSE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, activated_negotiate_do_sends_will_is_active_no_signal)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});
    server.negotiate(telnetpp::do_, [](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::will, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    server.negotiate(
        telnetpp::do_,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_TRUE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, activated_negotiate_dont_sends_wont_is_inactive_with_signal)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});
    server.negotiate(telnetpp::do_, [](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::wont, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    server.negotiate(
        telnetpp::dont,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, activated_activate_sends_nothing_is_active_with_signal)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});
    server.negotiate(telnetpp::do_, [](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.activate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_TRUE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, activated_deactivate_sends_wont_is_inactive_with_signal)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});
    server.negotiate(telnetpp::do_, [](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::wont, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    server.deactivate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, deactivating_negotiate_do_sends_nothing_is_active_with_signal)
{
    // Note: this transition is explicitly disallowed in the protocol 
    // specification.  However, it is included here to specify what our
    // behaviour is in the case that the remote is not acting according to 
    // spec.  The implementation currently chooses to be lenient and allow the
    // remote to cancel an option's deactivation.
    fake_server_option server(0xA5);
    server.activate([](auto &&){});
    server.negotiate(telnetpp::do_, [](auto &&){});
    server.deactivate([](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.negotiate(
        telnetpp::do_,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_TRUE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, deactivating_negotiate_dont_sends_nothing_is_inactive_with_signal)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});
    server.negotiate(telnetpp::do_, [](auto &&){});
    server.deactivate([](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.negotiate(
        telnetpp::dont,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, deactivating_deactivate_sends_nothing_is_inactive_no_signal)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});
    server.negotiate(telnetpp::do_, [](auto &&){});
    server.deactivate([](auto &&){});

    bool state_changed = false;
    server.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.deactivate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_FALSE(server.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, inactive_subnegotiation_is_ignored)
{
    fake_server_option server(0xA5);

    bool subnegotiation_called = false;
    server.on_subnegotiation.connect(
        [&subnegotiation_called](auto &&)
        { 
            subnegotiation_called = true; 
        });

    constexpr telnetpp::byte const content[] = { 0x01, 0x02, 0x03 };

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.subnegotiate(
        content,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(subnegotiation_called);
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(server_option_test, active_subnegotiation_is_handled)
{
    fake_server_option server(0xA5);
    server.activate([](auto &&){});
    server.negotiate(telnetpp::do_, [](auto &&){});

    bool subnegotiation_called = false;
    std::vector<telnetpp::byte> received_content;
    server.on_subnegotiation.connect(
        [&subnegotiation_called, &received_content](auto &&content)
        { 
            subnegotiation_called = true; 
            received_content.insert(
                received_content.end(),
                content.begin(),
                content.end());
        });

    constexpr telnetpp::byte const content[] = {
        0x01, 0x02, 0x03 
    };

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::byte> const expected_content = {
        0x01, 0x02, 0x03
    };

    std::vector<telnetpp::element> received_elements;
    server.subnegotiate(
        content,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(subnegotiation_called);
    ASSERT_EQ(expected_elements, received_elements);
    ASSERT_EQ(expected_content, received_content);
}
