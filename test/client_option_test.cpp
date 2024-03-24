#include "fakes/fake_channel.hpp"
#include "fakes/fake_client_option.hpp"

#include <gtest/gtest.h>
#include <telnetpp/client_option.hpp>

using namespace telnetpp::literals;  // NOLINT

TEST(client_option_test, code_returns_option_code)
{
    fake_channel channel;
    telnetpp::session session{channel};

    fake_client_option client{session, 21};
    ASSERT_EQ(telnetpp::option_type{21}, client.option_code());
}

namespace {

class a_client_option : public testing::Test
{
protected:
    a_client_option()
    {
        client_.on_state_changed.connect([this]() { state_changed_ = true; });

        client_.on_subnegotiation.connect([this](telnetpp::bytes data) {
            subnegotiation_content_.append(data.begin(), data.end());
        });
    }

    static constexpr telnetpp::option_type option = 0xA5;

    fake_channel channel_;
    telnetpp::session session_{channel_};
    fake_client_option client_{session_, option};
    telnetpp::byte_storage subnegotiation_content_;

    bool state_changed_{false};
};

}  // namespace

TEST_F(a_client_option, is_deactivated_by_default)
{
    ASSERT_FALSE(client_.active());
}

namespace {

class a_deactivated_client_option : public a_client_option
{
};

}  // namespace

TEST_F(a_deactivated_client_option, on_will_responds_with_do_and_is_active)
{
    client_.negotiate(telnetpp::will);

    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::do_, option};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_TRUE(client_.active());
}

TEST_F(
    a_deactivated_client_option, on_wont_responds_with_dont_and_is_not_active)
{
    client_.negotiate(telnetpp::wont);

    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::dont, option};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(client_.active());
}

TEST_F(a_deactivated_client_option, on_activation_sends_do)
{
    client_.activate();

    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::do_, option};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(client_.active());
}

TEST_F(a_deactivated_client_option, on_deactivate_sends_nothing)
{
    client_.deactivate();

    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_FALSE(client_.active());
}

TEST_F(a_deactivated_client_option, ignores_subnegotiations)
{
    client_.subnegotiate(telnetpp::byte_storage{0x01, 0x02});

    telnetpp::byte_storage const expected_subnegotiation_content = {};
    ASSERT_EQ(expected_subnegotiation_content, subnegotiation_content_);
}

namespace {

class an_activating_client_option : public a_client_option
{
protected:
    an_activating_client_option()
    {
        client_.activate();
        channel_.written_.clear();
    }
};

}  // namespace

TEST_F(an_activating_client_option, on_will_is_active)
{
    client_.negotiate(telnetpp::will);

    telnetpp::byte_storage const expected_written = {};
    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_TRUE(client_.active());
}

TEST_F(an_activating_client_option, on_wont_is_inactive)
{
    client_.negotiate(telnetpp::wont);

    telnetpp::byte_storage const expected_written = {};
    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_FALSE(client_.active());
}

TEST_F(an_activating_client_option, on_activate_sends_nothing)
{
    client_.activate();

    telnetpp::byte_storage const expected_written = {};
    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(client_.active());
}

TEST_F(an_activating_client_option, ignores_subnegotiations)
{
    client_.subnegotiate(telnetpp::byte_storage{0x01, 0x02});

    telnetpp::byte_storage const expected_subnegotiation_content = {};
    ASSERT_EQ(expected_subnegotiation_content, subnegotiation_content_);
}

namespace {

class an_active_client_option : public a_client_option
{
protected:
    an_active_client_option()
    {
        client_.negotiate(telnetpp::will);
        assert(client_.active());

        channel_.written_.clear();
        state_changed_ = false;
    }
};

}  // namespace

TEST_F(an_active_client_option, on_will_sends_do)
{
    client_.negotiate(telnetpp::will);

    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::do_, option};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_TRUE(client_.active());
}

TEST_F(an_active_client_option, on_wont_sends_dont_is_inactive)
{
    client_.negotiate(telnetpp::wont);

    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::dont, option};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_FALSE(client_.active());
}

TEST_F(an_active_client_option, on_activate_sends_nothing)
{
    client_.activate();

    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_TRUE(client_.active());
}

TEST_F(an_active_client_option, on_deactivate_sends_dont_is_inactive)
{
    client_.deactivate();

    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::dont, option};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(client_.active());
}

TEST_F(an_active_client_option, handles_subnegotiations)
{
    client_.subnegotiate(telnetpp::byte_storage{0x01, 0x02});

    telnetpp::byte_storage const expected_subnegotiation_content = {0x01, 0x02};
    ASSERT_EQ(expected_subnegotiation_content, subnegotiation_content_);
}

namespace {

class a_deactivating_client_option : public a_client_option
{
protected:
    a_deactivating_client_option()
    {
        client_.negotiate(telnetpp::will);
        assert(client_.active());
        client_.deactivate();
        assert(!client_.active());

        channel_.written_.clear();
        state_changed_ = false;
    }
};

}  // namespace

TEST_F(a_deactivating_client_option, on_will_sends_nothing_is_active)
{
    // Note: this transition is explicitly disallowed in the protocol
    // specification.  However, it is included here to specify what our
    // behaviour is in the case that the remote is not acting according to
    // spec.  The implementation currently chooses to be lenient and allow the
    // remote to cancel an option's deactivation.
    client_.negotiate(telnetpp::will);

    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_TRUE(client_.active());
}

TEST_F(a_deactivating_client_option, on_wont_is_inactive)
{
    client_.negotiate(telnetpp::wont);

    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_FALSE(client_.active());
}

TEST_F(a_deactivating_client_option, on_deactivate_sends_nothing)
{
    client_.deactivate();

    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(client_.active());
}

TEST_F(a_deactivating_client_option, ignores_subnegotiations)
{
    client_.subnegotiate(telnetpp::byte_storage{0x01, 0x02});

    telnetpp::byte_storage const expected_subnegotiation_content = {};
    ASSERT_EQ(expected_subnegotiation_content, subnegotiation_content_);
}
