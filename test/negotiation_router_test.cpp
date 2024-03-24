#include <gtest/gtest.h>
#include <telnetpp/detail/negotiation_router.hpp>

TEST(
    negotiation_router_test,
    message_with_registered_key_goes_to_registered_function)
{
    telnetpp::detail::negotiation_router router;

    telnetpp::negotiation expected(telnetpp::dont, 0x01);
    telnetpp::negotiation neg(telnetpp::will, 0x00);
    router.register_route(
        expected, [&neg](telnetpp::negotiation new_negotiation) {
            neg = new_negotiation;
        });

    bool unregistered_route_called = false;
    router.set_unregistered_route(
        [&unregistered_route_called](telnetpp::negotiation) {
            unregistered_route_called = true;
        });

    router(expected);

    ASSERT_EQ(expected, neg);
    ASSERT_FALSE(unregistered_route_called);
}

TEST(
    negotiation_router_test,
    message_with_unregistered_key_goes_to_unregistered_function)
{
    telnetpp::detail::negotiation_router router;

    bool registered_route_called = false;
    telnetpp::negotiation unexpected(telnetpp::wont, 0x02);
    router.register_route(
        unexpected, [&registered_route_called](telnetpp::negotiation neg) {
            registered_route_called = true;
        });

    telnetpp::negotiation neg(telnetpp::will, 0x00);
    router.set_unregistered_route(
        [&neg](telnetpp::negotiation new_negotiation) {
            neg = new_negotiation;
        });

    telnetpp::negotiation expected(telnetpp::will, 0x02);
    router(expected);

    ASSERT_FALSE(registered_route_called);
    ASSERT_EQ(expected, neg);
}
