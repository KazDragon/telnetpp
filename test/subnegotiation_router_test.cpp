#include "telnetpp/detail/subnegotiation_router.hpp"
#include "telnetpp/options/naws/client.hpp"
#include "telnetpp/protocol.hpp"
#include "telnetpp/detail/registration.hpp"
#include <gtest/gtest.h>

TEST(subnegotiation_router_test, when_nothing_is_registered_router_sinks_data)
{
    telnetpp::detail::subnegotiation_router router;
    router(telnetpp::subnegotiation(0, {}));
}

TEST(subnegotiation_router_test, message_with_registered_key_goes_to_registered_function)
{
    telnetpp::detail::subnegotiation_router router;

    telnetpp::subnegotiation sub(0x00, {});
    telnetpp::subnegotiation expected(0x01, {0x02});
    bool unregistered_route_called = false;

    router.register_route(expected.option(),
        [&sub](auto &&new_subnegotiation) -> std::vector<telnetpp::token>
        {
            sub = new_subnegotiation;
            return {};
        });

    router.set_unregistered_route(
        [&unregistered_route_called](auto &&) -> std::vector<telnetpp::token>
        {
            unregistered_route_called = true;
            return {};
        });


    router(expected);

    ASSERT_EQ(expected, sub);
    ASSERT_EQ(false, unregistered_route_called);
}

TEST(subnegotiation_router_test, message_with_unregistered_key_goes_to_unregistered_function)
{
    telnetpp::detail::subnegotiation_router router;

    telnetpp::subnegotiation sub(0x00, {});
    telnetpp::subnegotiation expected(0x01, {0x02});
    telnetpp::subnegotiation unexpected(0x02, {0x02});

    bool registered_route_called = false;

    router.register_route(unexpected.option(),
        [&registered_route_called](auto &&) -> std::vector<telnetpp::token>
        {
            registered_route_called = true;
            return {};
        });

    router.set_unregistered_route(
        [&sub](auto &&new_subnegotiation) -> std::vector<telnetpp::token>
        {
            sub = new_subnegotiation;
            return {};
        });


    router(expected);

    ASSERT_EQ(false, registered_route_called);
    ASSERT_EQ(expected, sub);
}

TEST(subnegotiation_router_test, routing_subnegotiation_returns_subnegotiation_result)
{
    telnetpp::detail::subnegotiation_router router;
    telnetpp::options::naws::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::detail::register_route_from_subnegotiation_to_option(router, client);

    telnetpp::options::naws::client::window_dimension width = 0;
    telnetpp::options::naws::client::window_dimension height = 0;

    client.on_window_size_changed.connect(
        [&width, &height](auto new_width, auto new_height)
            -> std::vector<telnetpp::token>
        {
            width = new_width;
            height = new_height;
            return {};
        });

    router(telnetpp::subnegotiation(
        client.option(),
        { 0x00, 80, 0x00, 24 }));

    telnetpp::options::naws::client::window_dimension const expected_width = 80;
    telnetpp::options::naws::client::window_dimension const expected_height = 24;

    ASSERT_EQ(expected_width, width);
    ASSERT_EQ(expected_height, height);
}
