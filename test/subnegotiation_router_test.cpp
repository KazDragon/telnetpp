#include <gtest/gtest.h>
#include <telnetpp/detail/subnegotiation_router.hpp>

TEST(
    subnegotiation_router_test,
    message_with_registered_key_goes_to_registered_function)
{
    telnetpp::detail::subnegotiation_router router;

    static constexpr telnetpp::byte const content[] = {0x02};

    telnetpp::subnegotiation sub(0x00, {});
    telnetpp::subnegotiation expected(0x01, content);
    bool unregistered_route_called = false;

    router.register_route(
        expected.option(),
        [&sub](telnetpp::subnegotiation const &new_subnegotiation) {
            sub = new_subnegotiation;
        });

    router.set_unregistered_route(
        [&unregistered_route_called](telnetpp::subnegotiation const &) {
            unregistered_route_called = true;
        });

    router(expected);

    ASSERT_EQ(expected, sub);
    ASSERT_EQ(false, unregistered_route_called);
}

TEST(
    subnegotiation_router_test,
    message_with_unregistered_key_goes_to_unregistered_function)
{
    telnetpp::detail::subnegotiation_router router;

    static constexpr telnetpp::byte const content[] = {0x02};

    telnetpp::subnegotiation sub(0x00, {});
    telnetpp::subnegotiation expected(0x01, content);
    telnetpp::subnegotiation unexpected(0x02, content);

    bool registered_route_called = false;

    router.register_route(
        unexpected.option(),
        [&registered_route_called](telnetpp::subnegotiation const &) {
            registered_route_called = true;
        });

    router.set_unregistered_route(
        [&sub](telnetpp::subnegotiation const &new_subnegotiation) {
            sub = new_subnegotiation;
        });

    router(expected);

    ASSERT_EQ(false, registered_route_called);
    ASSERT_EQ(expected, sub);
}
