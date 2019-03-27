#include "telnetpp/detail/negotiation_router.hpp"
#include "telnetpp/options/echo/server.hpp"
#include "telnetpp/detail/registration.hpp"
#include <gtest/gtest.h>

TEST(negotiation_router_test, when_nothing_is_registered_router_sinks_data)
{
    telnetpp::detail::negotiation_router router;

    bool called = false;
    router(
        telnetpp::negotiation{telnetpp::will, 0x00},
        [&called](auto &&...)
        {
            called = true;
        });

    ASSERT_FALSE(called);
}

TEST(negotiation_router_test, message_with_registered_key_goes_to_registered_function)
{
    telnetpp::detail::negotiation_router router;

    telnetpp::negotiation neg(0x00, 0x00);
    telnetpp::negotiation expected(telnetpp::dont, 0x01);
    bool unregistered_route_called = false;

    router.register_route(
        expected,
        [&neg](auto &&new_negotiation, auto &&cont)
        {
            neg = new_negotiation;
        });

    router.set_unregistered_route(
        [&unregistered_route_called](auto &&neg, auto &&cont)
        {
            unregistered_route_called = true;
        });


    router(expected, [](auto &&){});

    ASSERT_EQ(expected, neg);
    ASSERT_EQ(false, unregistered_route_called);
}

TEST(negotiation_router_test, message_with_unregistered_key_goes_to_unregistered_function)
{
    telnetpp::detail::negotiation_router router;

    telnetpp::negotiation neg(0x00, 0x00);
    telnetpp::negotiation expected(telnetpp::will, 0x02);
    telnetpp::negotiation unexpected(telnetpp::wont, 0x02);

    bool registered_route_called = false;

    router.register_route(
        unexpected,
        [&registered_route_called](auto &&neg, auto &&cont)
        {
            registered_route_called = true;
        });

    router.set_unregistered_route(
        [&neg](auto &&new_negotiation, auto &&cont)
        {
            neg = new_negotiation;
        });


    router(expected, [](auto &&){});

    ASSERT_EQ(false, registered_route_called);
    ASSERT_EQ(expected, neg);
}

TEST(negotiation_router_test, activating_option_returns_activation_sequence)
{
    telnetpp::detail::negotiation_router router;
    telnetpp::options::echo::server server;

    telnetpp::detail::register_route_from_negotiation_to_option(
        router, telnetpp::dont, server);

    std::vector<telnetpp::element> const expected_result = {
        telnetpp::negotiation{telnetpp::wont, server.option_code()}
    };

    std::vector<telnetpp::element> received_result;
    router(
        telnetpp::negotiation{telnetpp::dont, server.option_code()},
        [&received_result](telnetpp::element const &elem)
        {
            received_result.push_back(elem);
        });
}
