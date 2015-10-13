#include "telnetpp/routing_visitor.hpp"
#include "telnetpp/options/naws/client.hpp"
#include "telnetpp/options/naws.hpp"
#include "telnetpp/protocol.hpp"
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class routing_visitor_test : public CppUnit::TestFixture
{
public :
    CPPUNIT_TEST_SUITE(routing_visitor_test);
        CPPUNIT_TEST(text_routes_to_text_function);
        CPPUNIT_TEST(text_does_not_route_to_null_function);
        CPPUNIT_TEST(command_routes_to_command_router);
        CPPUNIT_TEST(negotiation_routes_to_negotiation_router);
        CPPUNIT_TEST(subnegotiation_routes_to_subnegotiation_router);
        CPPUNIT_TEST(subnegotiation_accumulates_responses);
    CPPUNIT_TEST_SUITE_END();

private :
    void text_routes_to_text_function();
    void text_does_not_route_to_null_function();
    void command_routes_to_command_router();
    void negotiation_routes_to_negotiation_router();
    void subnegotiation_routes_to_subnegotiation_router();
    void subnegotiation_accumulates_responses();
};

CPPUNIT_TEST_SUITE_REGISTRATION(routing_visitor_test);

void routing_visitor_test::text_routes_to_text_function()
{
    telnetpp::command_router cmd_router;
    telnetpp::negotiation_router neg_router;
    telnetpp::subnegotiation_router sub_router;
    
    std::string text;
    std::string expected_text = "expected text";
    
    telnetpp::routing_visitor visitor(
        [&text](auto &&new_text) -> std::vector<telnetpp::token_pass>
        {
            text = new_text;
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);
        
    telnetpp::token text_token(expected_text);
    boost::apply_visitor(visitor, text_token);
    
    CPPUNIT_ASSERT_EQUAL(expected_text, text);
}

void routing_visitor_test::text_does_not_route_to_null_function()
{
    telnetpp::command_router cmd_router;
    telnetpp::negotiation_router neg_router;
    telnetpp::subnegotiation_router sub_router;
    
    telnetpp::routing_visitor visitor(
        nullptr,
        cmd_router,
        neg_router,
        sub_router);
        
    telnetpp::token text_token("text token");
    boost::apply_visitor(visitor, text_token);
}

void routing_visitor_test::command_routes_to_command_router()
{
    telnetpp::command_router cmd_router;
    telnetpp::negotiation_router neg_router;
    telnetpp::subnegotiation_router sub_router;
    
    telnetpp::command cmd(0x00);
    telnetpp::command expected_command(telnetpp::ayt);
    
    cmd_router.register_route(expected_command,
        [&cmd](auto &&new_cmd) -> std::vector<telnetpp::token_pass>
        {
            cmd = new_cmd;
            return {};
        });
        
    telnetpp::routing_visitor visitor(
        [](auto &&) -> std::vector<telnetpp::token_pass>
        {
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);
        
    telnetpp::token cmd_token(expected_command);
    boost::apply_visitor(visitor, cmd_token);
    
    CPPUNIT_ASSERT_EQUAL(expected_command, cmd);
}

void routing_visitor_test::negotiation_routes_to_negotiation_router()
{
    telnetpp::command_router cmd_router;
    telnetpp::negotiation_router neg_router;
    telnetpp::subnegotiation_router sub_router;
    telnetpp::options::naws::client client;
    client.activate();
    
    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed]() -> std::vector<telnetpp::token_pass>
        {
            state_changed = true;
            return {};
        });
    
    telnetpp::register_client_option(client, neg_router, sub_router);
        
    telnetpp::routing_visitor visitor(
        [](auto &&) -> std::vector<telnetpp::token_pass>
        {
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);

    telnetpp::token neg_token(
        telnetpp::negotiation(
            telnetpp::will, telnetpp::options::naws::option));

    boost::apply_visitor(visitor, neg_token);
    
    CPPUNIT_ASSERT_EQUAL(true, state_changed);
    CPPUNIT_ASSERT_EQUAL(true, client.is_active());
}

void routing_visitor_test::subnegotiation_routes_to_subnegotiation_router()
{
    telnetpp::command_router cmd_router;
    telnetpp::negotiation_router neg_router;
    telnetpp::subnegotiation_router sub_router;
    telnetpp::options::naws::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::u16 width = 0;
    telnetpp::u16 height = 0;
    client.on_window_size_changed.connect(
        [&width, &height](auto &&new_width, auto &&new_height)
            -> std::vector<telnetpp::token_pass>
        {
            width = new_width;
            height = new_height;
            return {};
        });
        
    telnetpp::register_route_from_subnegotiation_to_option(
        sub_router, client);
        
    telnetpp::routing_visitor visitor(
        [](auto &&) -> std::vector<telnetpp::token_pass>
        {
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);

    telnetpp::token sub_token(
        telnetpp::subnegotiation(
            telnetpp::options::naws::option,
            {0, 80, 0, 24}));

    boost::apply_visitor(visitor, sub_token);
    
    telnetpp::u16 expected_width = 80;
    telnetpp::u16 expected_height = 24;
    
    CPPUNIT_ASSERT_EQUAL(expected_width, width);
    CPPUNIT_ASSERT_EQUAL(expected_height, height);
}

void routing_visitor_test::subnegotiation_accumulates_responses()
{
    telnetpp::command_router cmd_router;
    telnetpp::negotiation_router neg_router;
    telnetpp::subnegotiation_router sub_router;
    telnetpp::options::naws::client client;
    
    telnetpp::register_client_option(client, neg_router, sub_router);
    
    client.on_window_size_changed.connect(
        [&client](auto &&, auto &&)
        {
            return client.deactivate();
        });
    
    telnetpp::routing_visitor visitor(
        [](auto &&) -> std::vector<telnetpp::token_pass>
        {
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);

    client.activate();
    telnetpp::token do_negotiation_token(
        telnetpp::negotiation(telnetpp::will, telnetpp::options::naws::option));
    
    CPPUNIT_ASSERT_EQUAL(
        size_t{0},
        boost::apply_visitor(visitor, do_negotiation_token).size());
    
    telnetpp::token sub_token(
        telnetpp::subnegotiation(
            telnetpp::options::naws::option,
            {0, 80, 0, 24}));

    auto result = boost::apply_visitor(visitor, sub_token);

    telnetpp::negotiation expected(
        telnetpp::dont, telnetpp::options::naws::option);
    
    CPPUNIT_ASSERT_EQUAL(size_t(1), result.size());
    CPPUNIT_ASSERT_EQUAL(
        expected, 
        boost::get<telnetpp::negotiation>(
            boost::get<telnetpp::token>(result[0])));
}
