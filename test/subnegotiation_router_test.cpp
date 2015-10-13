#include "telnetpp/subnegotiation_router.hpp"
#include "telnetpp/options/naws/client.hpp"
#include "telnetpp/options/naws.hpp"
#include "telnetpp/protocol.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class subnegotiation_router_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(subnegotiation_router_test);
        CPPUNIT_TEST(when_nothing_is_registered_router_sinks_data);
        CPPUNIT_TEST(message_with_registered_key_goes_to_registered_function);
        CPPUNIT_TEST(message_with_unregistered_key_goes_to_unregistered_function);
        CPPUNIT_TEST(routing_subnegotiation_returns_subnegotiation_result);
    CPPUNIT_TEST_SUITE_END();
    
private :
    void when_nothing_is_registered_router_sinks_data();
    void message_with_registered_key_goes_to_registered_function();
    void message_with_unregistered_key_goes_to_unregistered_function();
    void routing_subnegotiation_returns_subnegotiation_result();
};

CPPUNIT_TEST_SUITE_REGISTRATION(subnegotiation_router_test);

void subnegotiation_router_test::when_nothing_is_registered_router_sinks_data()
{
    telnetpp::subnegotiation_router router;
    router(telnetpp::subnegotiation(telnetpp::options::naws::option, {}));
}

void subnegotiation_router_test::message_with_registered_key_goes_to_registered_function()
{
    telnetpp::subnegotiation_router router;
    
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

    CPPUNIT_ASSERT_EQUAL(expected, sub);
    CPPUNIT_ASSERT_EQUAL(false, unregistered_route_called);
}

void subnegotiation_router_test::message_with_unregistered_key_goes_to_unregistered_function()
{
    telnetpp::subnegotiation_router router;
    
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
    
    CPPUNIT_ASSERT_EQUAL(false, registered_route_called);
    CPPUNIT_ASSERT_EQUAL(expected, sub);
}

void subnegotiation_router_test::routing_subnegotiation_returns_subnegotiation_result()
{
    telnetpp::subnegotiation_router router;
    telnetpp::options::naws::client client;
    client.activate();
    client.negotiate(telnetpp::will);
    
    register_route_from_subnegotiation_to_option(router, client);
    
    telnetpp::u16 width = 0;
    telnetpp::u16 height = 0;
    client.on_window_size_changed.connect(
        [&width, &height](auto new_width, auto new_height)
            -> std::vector<telnetpp::token>
        {
            width = new_width;
            height = new_height;
            return {};
        });
    
    router(telnetpp::subnegotiation(
        telnetpp::options::naws::option,
        { 0x00, 80, 0x00, 24 }));
    
    telnetpp::u16 expected_width = 80;
    telnetpp::u16 expected_height = 24;
    
    CPPUNIT_ASSERT_EQUAL(expected_width, width);
    CPPUNIT_ASSERT_EQUAL(expected_height, height);
}
