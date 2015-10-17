#include "telnetpp/detail/negotiation_router.hpp"
#include "telnetpp/options/echo/server.hpp"
#include "telnetpp/options/echo.hpp"
#include "telnetpp/protocol.hpp"
#include "telnetpp/detail/registration.hpp"
#include "expect_elements.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class negotiation_router_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(negotiation_router_test);
        CPPUNIT_TEST(when_nothing_is_registered_router_sinks_data);
        CPPUNIT_TEST(message_with_registered_key_goes_to_registered_function);
        CPPUNIT_TEST(message_with_unregistered_key_goes_to_unregistered_function);
        CPPUNIT_TEST(activating_option_returns_activation_sequence);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void when_nothing_is_registered_router_sinks_data();
    void message_with_registered_key_goes_to_registered_function();
    void message_with_unregistered_key_goes_to_unregistered_function();
    void activating_option_returns_activation_sequence();
};

CPPUNIT_TEST_SUITE_REGISTRATION(negotiation_router_test);

void negotiation_router_test::when_nothing_is_registered_router_sinks_data()
{
    telnetpp::detail::negotiation_router router;
    router(telnetpp::negotiation(telnetpp::will, 0x00));
}

void negotiation_router_test::message_with_registered_key_goes_to_registered_function()
{
    telnetpp::detail::negotiation_router router;
    
    telnetpp::negotiation neg(0x00, 0x00);
    telnetpp::negotiation expected(telnetpp::dont, 0x01);
    bool unregistered_route_called = false;
    
    router.register_route(expected, 
        [&neg](auto &&new_negotiation) -> std::vector<telnetpp::token>
        { 
            neg = new_negotiation;
            return {};
        });
    
    router.set_unregistered_route(
        [&unregistered_route_called](auto &&) -> std::vector<telnetpp::token>
        {
            unregistered_route_called = true;
            return {};
        });
    
    
    router(expected);

    CPPUNIT_ASSERT_EQUAL(expected, neg);
    CPPUNIT_ASSERT_EQUAL(false, unregistered_route_called);
}

void negotiation_router_test::message_with_unregistered_key_goes_to_unregistered_function()
{
    telnetpp::detail::negotiation_router router;
    
    telnetpp::negotiation neg(0x00, 0x00);
    telnetpp::negotiation expected(telnetpp::will, 0x02);
    telnetpp::negotiation unexpected(telnetpp::wont, 0x02);
    
    bool registered_route_called = false;
    
    router.register_route(unexpected, 
        [&registered_route_called](auto &&) -> std::vector<telnetpp::token>
        {
            registered_route_called = true;
            return {};
        });
    
    router.set_unregistered_route(
        [&neg](auto &&new_negotiation) -> std::vector<telnetpp::token>
        {
            neg = new_negotiation;
            return {};
        });
    
    
    router(expected);
    
    CPPUNIT_ASSERT_EQUAL(false, registered_route_called);
    CPPUNIT_ASSERT_EQUAL(expected, neg);
}

void negotiation_router_test::activating_option_returns_activation_sequence()
{
    telnetpp::detail::negotiation_router router;
    telnetpp::options::echo::server server;
    telnetpp::negotiation expected(
        telnetpp::dont, 
        telnetpp::options::echo::option);
    
    telnetpp::detail::register_route_from_negotiation_to_option(
        router, telnetpp::dont, server);

    expect_elements({
        telnetpp::negotiation(
            telnetpp::wont, telnetpp::options::echo::option)
        },
        router(telnetpp::negotiation(
            telnetpp::dont, telnetpp::options::echo::option)));
}
