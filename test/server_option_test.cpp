#include "telnetpp/server_option.hpp"
#include "fakes/fake_channel.hpp"
#include "fakes/fake_server_option.hpp"
#include <gtest/gtest.h>

using namespace telnetpp::literals;

TEST(server_option_test, code_returns_option_code)
{
    fake_channel channel;
    telnetpp::session session{channel};

    fake_server_option server{session, 21};
    ASSERT_EQ(telnetpp::option_type{21}, server.option_code());
}

namespace {

class a_server_option : public testing::Test
{
protected:
    a_server_option()
    {
        server_.on_state_changed.connect(
            [this]() { 
                state_changed_ = true; 
            });

        server_.on_subnegotiation.connect(
            [this](telnetpp::bytes data) {
                subnegotiation_content_.append(
                    data.begin(),
                    data.end());
            });
    }

    static constexpr telnetpp::option_type option_ = 0xA5;

    fake_channel channel_;
    telnetpp::session session_{channel_};
    fake_server_option server_{session_, option_};
    telnetpp::byte_storage subnegotiation_content_;

    bool state_changed_{false};
};

}

TEST_F(a_server_option, is_deactivated_by_default)
{
    ASSERT_FALSE(server_.active());
}

namespace {

class a_deactivated_server_option : public a_server_option
{
};

}

TEST_F(a_deactivated_server_option, on_do_responds_with_will_and_is_active)
{
    server_.negotiate(telnetpp::do_);

    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::will, option_
    };

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_TRUE(server_.active());
}

TEST_F(a_deactivated_server_option, on_dont_responds_with_wont_and_is_not_active)
{
    server_.negotiate(telnetpp::dont);

    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::wont, option_
    };

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(server_.active());
}

TEST_F(a_deactivated_server_option, on_activation_sends_will)
{
    server_.activate();

    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::will, option_
    };

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(server_.active());
}

TEST_F(a_deactivated_server_option, on_deactivate_sends_nothing)
{
    server_.deactivate();

    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_FALSE(server_.active());
}

TEST_F(a_deactivated_server_option, ignores_subnegotiations)
{
    server_.subnegotiate(telnetpp::byte_storage{ 0x01, 0x02 });
    
    telnetpp::byte_storage const expected_subnegotiation_content = {};
    ASSERT_EQ(expected_subnegotiation_content, subnegotiation_content_);
}

namespace {

class an_activating_server_option : public a_server_option
{
protected:
    an_activating_server_option()
    {
        server_.activate();
        channel_.written_.clear();
    }
};

}

TEST_F(an_activating_server_option, on_do_is_active)
{
    server_.negotiate(telnetpp::do_);

    telnetpp::byte_storage const expected_written = {};
    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_TRUE(server_.active());
}

TEST_F(an_activating_server_option, on_dont_is_inactive)
{
    server_.negotiate(telnetpp::dont);

    telnetpp::byte_storage const expected_written = {};
    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_FALSE(server_.active());
}

TEST_F(an_activating_server_option, on_activate_sends_nothing)
{
    server_.activate();

    telnetpp::byte_storage const expected_written = {};
    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(server_.active());
}

TEST_F(an_activating_server_option, ignores_subnegotiations)
{
    server_.subnegotiate(telnetpp::byte_storage{ 0x01, 0x02 });
    
    telnetpp::byte_storage const expected_subnegotiation_content = {};
    ASSERT_EQ(expected_subnegotiation_content, subnegotiation_content_);
}

namespace {

class an_active_server_option : public a_server_option
{
protected:
    an_active_server_option()
    {
        server_.negotiate(telnetpp::do_);
        assert(server_.active());

        channel_.written_.clear();
        state_changed_ = false;
    }
};

}

TEST_F(an_active_server_option, on_do_sends_will)
{
    server_.negotiate(telnetpp::do_);
    
    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::will, option_
    };

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_TRUE(server_.active());
}

TEST_F(an_active_server_option, on_dont_sends_wont_is_inactive)
{
    server_.negotiate(telnetpp::dont);
    
    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::wont, option_
    };

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_FALSE(server_.active());
}

TEST_F(an_active_server_option, on_activate_sends_nothing)
{
    server_.activate();
    
    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_TRUE(server_.active());
}

TEST_F(an_active_server_option, on_deactivate_sends_wont_is_inactive)
{
    server_.deactivate();
    
    telnetpp::byte_storage const expected_written = {
        telnetpp::iac, telnetpp::wont, option_
    };

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(server_.active());
}

TEST_F(an_active_server_option, handles_subnegotiations)
{
    server_.subnegotiate(telnetpp::byte_storage{ 0x01, 0x02 });
    
    telnetpp::byte_storage const expected_subnegotiation_content = {
        0x01, 0x02
    };
    ASSERT_EQ(expected_subnegotiation_content, subnegotiation_content_);
}

namespace {

class a_deactivating_server_option : public a_server_option
{
protected:
    a_deactivating_server_option()
    {
        server_.negotiate(telnetpp::do_);
        assert(server_.active());
        server_.deactivate();
        assert(!server_.active());

        channel_.written_.clear();
        state_changed_ = false;
    }
};

}

TEST_F(a_deactivating_server_option, on_do_sends_nothing_is_active)
{
    // Note: this transition is explicitly disallowed in the protocol 
    // specification.  However, it is included here to specify what our
    // behaviour is in the case that the remote is not acting according to 
    // spec.  The implementation currently chooses to be lenient and allow the
    // remote to cancel an option's deactivation.
    server_.negotiate(telnetpp::do_);
    
    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_TRUE(server_.active());
}

TEST_F(a_deactivating_server_option, on_dont_is_inactive)
{
    server_.negotiate(telnetpp::dont);

    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_TRUE(state_changed_);
    ASSERT_FALSE(server_.active());
}

TEST_F(a_deactivating_server_option, on_deactivate_sends_nothing)
{
    server_.deactivate();

    telnetpp::byte_storage const expected_written = {};

    ASSERT_EQ(expected_written, channel_.written_);
    ASSERT_FALSE(state_changed_);
    ASSERT_FALSE(server_.active());
}

TEST_F(a_deactivating_server_option, ignores_subnegotiations)
{
    server_.subnegotiate(telnetpp::byte_storage{ 0x01, 0x02 });
    
    telnetpp::byte_storage const expected_subnegotiation_content = {};
    ASSERT_EQ(expected_subnegotiation_content, subnegotiation_content_);
}
