#include <telnetpp/client_option.hpp>
#include <telnetpp/server_option.hpp>
#include <gtest/gtest.h>

// The Q Method (RFC 1153) is a specification of how to use TELNET negotiations
// in such a way that client-server negotiation loops are avoided.  The
// document dictates a set of rules, and this code tests that they are adhered
// to.

// Some of these tests may overlap with client/server tests, but this is done
// so that the tests survive any later refactorings.

// Rule 1: a client/server must remember whether it is being disabled (in
// addition to whether it is being enabled.)  It must not send any further
// negotiations until the previous negotiation is complete.
template <class Option, telnetpp::negotiation_type PositiveResponse>
class an_option_being_deactivated : public testing::Test
{
public :
    an_option_being_deactivated()
    {
        option_.activate();
        option_.negotiate(PositiveResponse);
        option_.deactivate();
    }

protected :
    class option : public Option
    {
    public :
        option()
          : Option(0)
        {
        }

        std::vector<telnetpp::token> handle_subnegotiation(
            telnetpp::byte_stream const &content) override
        {
            return {};
        }
    };

    option option_;
};

using a_client_option_being_deactivated =
    an_option_being_deactivated<telnetpp::client_option, telnetpp::will>;
using a_server_option_being_deactivated =
    an_option_being_deactivated<telnetpp::server_option, telnetpp::do_>;

TEST_F(a_client_option_being_deactivated, ignores_activate_requests)
{
    auto result = option_.activate();
    ASSERT_EQ(0u, result.size());
}

TEST_F(a_client_option_being_deactivated, ignores_deactivate_requests)
{
    auto result = option_.deactivate();
    ASSERT_EQ(0u, result.size());
}

TEST_F(a_server_option_being_deactivated, ignores_activate_requests)
{
    auto result = option_.activate();
    ASSERT_EQ(0u, result.size());
}

TEST_F(a_server_option_being_deactivated, ignores_deactivate_requests)
{
    auto result = option_.deactivate();
    ASSERT_EQ(0u, result.size());
}

template <class Option>
class an_option_being_activated : public testing::Test
{
public :
    an_option_being_activated()
    {
        option_.activate();
    }

protected :
    class option : public Option
    {
    public :
        option()
          : Option(0)
        {
        }

        std::vector<telnetpp::token> handle_subnegotiation(
            telnetpp::byte_stream const &content) override
        {
            return {};
        }
    };

    option option_;
};

using a_client_option_being_activated =
    an_option_being_activated<telnetpp::client_option>;
using a_server_option_being_activated =
    an_option_being_activated<telnetpp::server_option>;

TEST_F(a_client_option_being_activated, ignores_activate_requests)
{
    auto result = option_.activate();
    ASSERT_EQ(0u, result.size());
}

TEST_F(a_client_option_being_activated, ignores_deactivate_requests)
{
    auto result = option_.deactivate();
    ASSERT_EQ(0u, result.size());
}

TEST_F(a_server_option_being_activated, ignores_activate_requests)
{
    auto result = option_.activate();
    ASSERT_EQ(0u, result.size());
}

TEST_F(a_server_option_being_activated, ignores_deactivate_requests)
{
    auto result = option_.deactivate();
    ASSERT_EQ(0u, result.size());
}

// Rule 2: Prohibit new requests before completing old negotiation.
// This is covered above.  All requests are ignored if there is one underway.
// In this way, a user of the code may assume that the next response that
// arrives is destined for him.  This may involve some odd results, such as
// sending WONT and receiving DO (since it's actually the result of the
// previous WILL).
//
// I could implement a queue here, but I imagine it's a rare case.  For my
// use cases, there is only usually one initial request, and then nothing
// else changes.

// Rule 3: Separate WANTNO and WANTYET
// This is the activating/deactivating states.

