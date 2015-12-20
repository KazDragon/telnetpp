#include "telnetpp/detail/command_router.hpp"
#include "telnetpp/protocol.hpp"
#include <gtest/gtest.h>

TEST(command_router_test, when_nothing_is_registered_router_sinks_data)
{
    telnetpp::detail::command_router router;
    router(telnetpp::command(telnetpp::ayt));
}

TEST(command_router_test, message_with_registered_key_goes_to_registered_function)
{
    telnetpp::detail::command_router router;
    
    telnetpp::command cmd(0x00);
    telnetpp::command expected(telnetpp::ayt);
    bool unregistered_route_called = false;
    
    router.register_route(expected,
        [&cmd](auto &&new_command) -> std::vector<telnetpp::token>
        {
            cmd = new_command;
            return {};
        });
    
    router.set_unregistered_route(
        [&unregistered_route_called](auto &&) -> std::vector<telnetpp::token>
        {
            unregistered_route_called = true;
            return {};
        });
    
    router(expected);
    
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
    
    router.register_route(unexpected,
        [&registered_route_called](auto &&) -> std::vector<telnetpp::token>
        {
            registered_route_called = true;
            return {};
        });
    
    router.set_unregistered_route(
        [&cmd](auto &&new_command) -> std::vector<telnetpp::token>
        {
            cmd = new_command;
            return {};
        });
    
    router(expected);
    
    ASSERT_EQ(false, registered_route_called);
    ASSERT_EQ(expected, cmd);
}
