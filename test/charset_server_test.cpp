#include "telnet_option_fixture.hpp"

#include <gtest/gtest.h>
#include <telnetpp/options/charset/server.hpp>

using namespace telnetpp::literals;  // NOLINT

namespace {
using a_charset_server = a_telnet_option<telnetpp::options::charset::server>;
}

TEST_F(a_charset_server, is_a_charset_server)
{
    ASSERT_EQ(42, option_.option_code());
}

namespace {

class an_active_charset_server : public a_charset_server
{
protected:
    an_active_charset_server()
    {
        option_.negotiate(telnetpp::do_);
        assert(option_.active());
        channel_.written_.clear();
    }
};

}  // namespace

TEST_F(
    an_active_charset_server,
    requesting_supported_charsets_sends_charset_request_subnegotiation)
{
    option_.request_charsets();

    telnetpp::byte_storage const expected_content = {
        telnetpp::iac,
        telnetpp::sb,
        option_.option_code(),
        0x01,
        ';',
        telnetpp::iac,
        telnetpp::se};

    ASSERT_EQ(expected_content, channel_.written_);
}

TEST_F(
    an_active_charset_server,
    receiving_charset_offer_without_utf8_records_offer_order_and_leaves_charset_empty)
{
    static auto const content = "\x01;US-ASCII;CP437"_tb;

    option_.subnegotiate(content);

    std::vector<telnetpp::byte_storage> const expected_charsets = {
        "US-ASCII"_tb,
        "CP437"_tb};

    ASSERT_EQ(expected_charsets, option_.advertised_charsets());
    ASSERT_FALSE(option_.negotiated_charset().has_value());
    ASSERT_TRUE(channel_.written_.empty());
}
