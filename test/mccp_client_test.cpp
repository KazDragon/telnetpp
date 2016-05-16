#include "expect_elements.hpp"
#include <telnetpp/protocol.hpp>
#include <telnetpp/options/mccp/client.hpp>
#include <telnetpp/options/mccp/detail/protocol.hpp>
#include <gtest/gtest.h>

TEST(mccp_client_test, client_is_mccp)
{
    telnetpp::options::mccp::client client;

    ASSERT_EQ(86, client.option());
}

TEST(mccp_client_test, empty_subnegotiation_returns_begin_decompression)
{
    telnetpp::options::mccp::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    auto const expected = std::vector<telnetpp::token> {
        boost::any(telnetpp::options::mccp::detail::begin_decompression{})
    };

    // When an empty subnegotiation is received, we should send a
    // begin_decompression tag so that the lower layers know to start treating
    // the very next byte received as the start of a compression stream.
    expect_tokens(expected, client.subnegotiate({}));
}

TEST(mccp_client_test, deactivating_client_locally_does_not_immediately_end_compression)
{
    telnetpp::options::mccp::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    client.subnegotiate({});

    // If we deactivate locally, then we should send a DONT packet as normal.
    // However, because other input received from the remote side may still
    // be compressed between now and the reception of the WONT packet, we
    // must not end the decompression.
    auto const &result = client.deactivate();

    // Only one element - the DONT packet.
    ASSERT_EQ(size_t{1}, result.size());
}

TEST(mccp_client_test, deactivated_client_locally_returns_end_decompression)
{
    telnetpp::options::mccp::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    client.subnegotiate({});
    client.deactivate();

    // Upon reception of the WONT packet, the client will be fully deactivated.
    // This means that the very next byte received will no longer be
    // compressed.  Therefore, we should end decompression.
    expect_tokens(
        {
            boost::any(telnetpp::options::mccp::detail::end_decompression{})
        },
        client.negotiate(telnetpp::wont));
}

TEST(mccp_client_test, deactivating_client_remotely_returns_end_decompression)
{
    telnetpp::options::mccp::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    client.subnegotiate({});

    // If we deactivate remotely, then we should send a DONT packet as normal,
    // and also an end_decompression to tell the lower layers to stop treating
    // input as compressed since the option is now completely off.
    // Furthermore, because the remote side has sent a WONT, and the only
    // legal response is DONT, it can assume that we will immediately
    // deactivate.
    auto const &result = client.negotiate(telnetpp::wont);

    ASSERT_EQ(size_t{2}, result.size());
    auto const &any = boost::get<boost::any>(result[1]);

    ASSERT_TRUE(
        typeid(telnetpp::options::mccp::detail::end_decompression) == any.type());
}
