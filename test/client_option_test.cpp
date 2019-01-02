#include "telnetpp/client_option.hpp"
#include <gtest/gtest.h>

namespace {

class fake_client_option : public telnetpp::client_option
{
public:
    explicit fake_client_option(telnetpp::option_type option)
      : telnetpp::client_option(option)
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

TEST(client_option_test, code_returns_option_code)
{
    fake_client_option client{21};
    ASSERT_EQ(telnetpp::option_type{21}, client.code());
}

TEST(client_option_test, deactivated_negotiate_will_sends_do_with_signal)
{
    fake_client_option client(0xA5);

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::element{telnetpp::negotiation{telnetpp::do_, 0xA5}}
    };

    std::vector<telnetpp::element> received_elements;
    client.negotiate(
        telnetpp::will, 
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_TRUE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, deactivated_negotiate_wont_sends_dont_no_signal)
{
    fake_client_option client(0xA5);

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::dont, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    client.negotiate(
        telnetpp::wont, 
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_FALSE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, deactivated_activate_sends_do_no_signal)
{
    fake_client_option client(0xA5);

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::do_, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    client.activate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_FALSE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, deactivated_deactivate_sends_nothing_with_signal)
{
    fake_client_option client(0xA5);

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.deactivate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, activating_negotiate_will_sends_nothing_is_active_with_signal)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.negotiate(
        telnetpp::will,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_TRUE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, activating_negotiate_wont_sends_nothing_is_inactive_with_signal)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.negotiate(
        telnetpp::wont,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, activating_activate_sends_nothing_no_signal)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.activate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_FALSE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, activated_negotiate_will_sends_do_is_active_no_signal)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::do_, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    client.negotiate(
        telnetpp::will,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_TRUE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, activated_negotiate_wont_sends_dont_is_inactive_with_signal)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::dont, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    client.negotiate(
        telnetpp::wont,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, activated_activate_sends_nothing_is_active_with_signal)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.activate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_TRUE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, activated_deactivate_sends_dont_is_inactive_with_signal)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::negotiation{telnetpp::dont, 0xA5}
    };

    std::vector<telnetpp::element> received_elements;
    client.deactivate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, deactivating_negotiate_will_sends_nothing_is_active_with_signal)
{
    // Note: this transition is explicitly disallowed in the protocol 
    // specification.  However, it is included here to specify what our
    // behaviour is in the case that the remote is not acting according to 
    // spec.  The implementation currently chooses to be lenient and allow the
    // remote to cancel an option's deactivation.
    fake_client_option client(0xA5);
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});
    client.deactivate([](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.negotiate(
        telnetpp::will,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_TRUE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, deactivating_negotiate_wont_sends_nothing_is_inactive_with_signal)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});
    client.deactivate([](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.negotiate(
        telnetpp::wont,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(state_changed);
    ASSERT_FALSE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, deactivating_deactivate_sends_nothing_is_inactive_no_signal)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});
    client.deactivate([](auto &&){});

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed](auto &&)
        { 
            state_changed = true; 
        });

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.deactivate(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(state_changed);
    ASSERT_FALSE(client.active());
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, inactive_subnegotiation_is_ignored)
{
    fake_client_option client(0xA5);

    bool subnegotiation_called = false;
    client.on_subnegotiation.connect(
        [&subnegotiation_called](auto &&)
        { 
            subnegotiation_called = true; 
        });

    constexpr telnetpp::byte const content[] = { 0x01, 0x02, 0x03 };

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.subnegotiate(
        content,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_FALSE(subnegotiation_called);
    ASSERT_EQ(expected_elements, received_elements);
}

TEST(client_option_test, active_subnegotiation_is_handled)
{
    fake_client_option client(0xA5);
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});

    bool subnegotiation_called = false;
    std::vector<telnetpp::byte> received_content;
    client.on_subnegotiation.connect(
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
    client.subnegotiate(
        content,
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_TRUE(subnegotiation_called);
    ASSERT_EQ(expected_elements, received_elements);
    ASSERT_EQ(expected_content, received_content);
}
