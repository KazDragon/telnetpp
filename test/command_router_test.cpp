#include "telnetpp/detail/command_router.hpp"
#include <gtest/gtest.h>

TEST(command_router_test, when_nothing_is_registered_router_sinks_data)
{
    telnetpp::detail::command_router router;

    bool called = false;
    router(
        telnetpp::command(telnetpp::ayt),
        [&called](auto &&...)
        {
            called = true;
        });

    ASSERT_FALSE(called);
}

TEST(command_router_test, message_with_registered_key_goes_to_registered_function)
{
    telnetpp::detail::command_router router;

    telnetpp::command cmd(0x00);
    telnetpp::command expected(telnetpp::ayt);
    bool unregistered_route_called = false;

    router.register_route(
        telnetpp::ayt,
        [&cmd](auto &&new_command, auto &&cont)
        {
            cmd = new_command;
        });

    router.set_unregistered_route(
        [&unregistered_route_called](auto &&, auto &&cont)
        {
            unregistered_route_called = true;
        });

    router(expected, [](auto &&...){});

    ASSERT_EQ(expected, cmd);
    ASSERT_EQ(false, unregistered_route_called);
}

TEST(command_router_test, message_with_unregistered_key_goes_to_unregistered_function)
{
    telnetpp::detail::command_router router;
    telnetpp::command cmd(0x00);
    telnetpp::command expected(telnetpp::dm);
    telnetpp::command unexpected(telnetpp::ao);

    bool registered_route_called = false;

    router.register_route(
        telnetpp::ao,
        [&registered_route_called](auto &&, auto &&)
        {
            registered_route_called = true;
        });

    router.set_unregistered_route(
        [&cmd](auto &&new_command, auto &&)
        {
            cmd = new_command;
        });

    router(expected, [](auto &&...){});

    ASSERT_EQ(false, registered_route_called);
    ASSERT_EQ(expected, cmd);
}
