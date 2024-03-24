#include <gtest/gtest.h>
#include <telnetpp/detail/command_router.hpp>

TEST(
    command_router_test,
    message_with_registered_key_goes_to_registered_function)
{
    telnetpp::detail::command_router router;

    telnetpp::command cmd(0x00);
    router.register_route(telnetpp::ayt, [&cmd](telnetpp::command new_command) {
        cmd = new_command;
    });

    bool unregistered_route_called = false;
    router.set_unregistered_route(
        [&unregistered_route_called](telnetpp::command) {
            unregistered_route_called = true;
        });

    router(telnetpp::command{telnetpp::ayt});

    telnetpp::command const expected(telnetpp::ayt);
    ASSERT_EQ(expected, cmd);
    ASSERT_FALSE(unregistered_route_called);
}

TEST(
    command_router_test,
    message_with_unregistered_key_goes_to_unregistered_function)
{
    telnetpp::detail::command_router router;

    telnetpp::command cmd(0x00);
    router.register_route(telnetpp::ayt, [&cmd](telnetpp::command new_command) {
        cmd = new_command;
    });

    bool unregistered_route_called = false;
    router.set_unregistered_route(
        [&unregistered_route_called](telnetpp::command) {
            unregistered_route_called = true;
        });

    router(telnetpp::command{telnetpp::dm});

    telnetpp::command const expected(0x00);
    ASSERT_EQ(expected, cmd);
    ASSERT_TRUE(unregistered_route_called);
}
