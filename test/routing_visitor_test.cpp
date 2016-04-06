#include "telnetpp/detail/routing_visitor.hpp"
#include "telnetpp/options/naws/client.hpp"
#include "telnetpp/protocol.hpp"
#include "telnetpp/detail/registration.hpp"
#include "expect_elements.hpp"
#include <gtest/gtest.h>

TEST(routing_visitor_test, text_routes_to_text_function)
{
    telnetpp::detail::command_router cmd_router;
    telnetpp::detail::negotiation_router neg_router;
    telnetpp::detail::subnegotiation_router sub_router;

    std::string text;
    std::string expected_text = "expected text";

    telnetpp::detail::routing_visitor visitor(
        [&text](auto &&new_text) -> std::vector<telnetpp::token>
        {
            text = new_text;
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);

    telnetpp::element text_token(expected_text);
    boost::apply_visitor(visitor, text_token);

    ASSERT_EQ(expected_text, text);
}

TEST(routing_visitor_test, text_does_not_route_to_null_function)
{
    telnetpp::detail::command_router cmd_router;
    telnetpp::detail::negotiation_router neg_router;
    telnetpp::detail::subnegotiation_router sub_router;

    telnetpp::detail::routing_visitor visitor(
        nullptr,
        cmd_router,
        neg_router,
        sub_router);

    telnetpp::element text_token(telnetpp::element("text token"));
    boost::apply_visitor(visitor, text_token);
}

TEST(routing_visitor_test, command_routes_to_command_router)
{
    telnetpp::detail::command_router cmd_router;
    telnetpp::detail::negotiation_router neg_router;
    telnetpp::detail::subnegotiation_router sub_router;

    telnetpp::command cmd(0x00);
    telnetpp::command expected_command(telnetpp::ayt);

    cmd_router.register_route(expected_command,
        [&cmd](auto &&new_cmd) -> std::vector<telnetpp::token>
        {
            cmd = new_cmd;
            return {};
        });

    telnetpp::detail::routing_visitor visitor(
        [](auto &&) -> std::vector<telnetpp::token>
        {
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);

    telnetpp::element cmd_token(expected_command);
    boost::apply_visitor(visitor, cmd_token);

    ASSERT_EQ(expected_command, cmd);
}

TEST(routing_visitor_test, negotiation_routes_to_negotiation_router)
{
    telnetpp::detail::command_router cmd_router;
    telnetpp::detail::negotiation_router neg_router;
    telnetpp::detail::subnegotiation_router sub_router;
    telnetpp::options::naws::client client;
    client.activate();

    bool state_changed = false;
    client.on_state_changed.connect(
        [&state_changed]() -> std::vector<telnetpp::token>
        {
            state_changed = true;
            return {};
        });

    telnetpp::detail::register_client_option(client, neg_router, sub_router);

    telnetpp::detail::routing_visitor visitor(
        [](auto &&) -> std::vector<telnetpp::token>
        {
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);

    telnetpp::element neg_token(
        telnetpp::negotiation(telnetpp::will, client.option()));

    boost::apply_visitor(visitor, neg_token);

    ASSERT_EQ(true, state_changed);
    ASSERT_EQ(true, client.is_active());
}

TEST(routing_visitor_test, subnegotiation_routes_to_subnegotiation_router)
{
    telnetpp::detail::command_router cmd_router;
    telnetpp::detail::negotiation_router neg_router;
    telnetpp::detail::subnegotiation_router sub_router;
    telnetpp::options::naws::client client;
    client.activate();
    client.negotiate(telnetpp::will);

    telnetpp::u16 width = 0;
    telnetpp::u16 height = 0;
    client.on_window_size_changed.connect(
        [&width, &height](auto &&new_width, auto &&new_height)
            -> std::vector<telnetpp::token>
        {
            width = new_width;
            height = new_height;
            return {};
        });

    telnetpp::detail::register_route_from_subnegotiation_to_option(
        sub_router, client);

    telnetpp::detail::routing_visitor visitor(
        [](auto &&) -> std::vector<telnetpp::token>
        {
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);

    telnetpp::element sub_token(
        telnetpp::subnegotiation(client.option(), {0, 80, 0, 24}));

    boost::apply_visitor(visitor, sub_token);

    telnetpp::u16 expected_width = 80;
    telnetpp::u16 expected_height = 24;

    ASSERT_EQ(expected_width, width);
    ASSERT_EQ(expected_height, height);
}

TEST(routing_visitor_test, subnegotiation_accumulates_responses)
{
    telnetpp::detail::command_router cmd_router;
    telnetpp::detail::negotiation_router neg_router;
    telnetpp::detail::subnegotiation_router sub_router;
    telnetpp::options::naws::client client;

    telnetpp::detail::register_client_option(client, neg_router, sub_router);

    client.on_window_size_changed.connect(
        [&client](auto &&, auto &&)
        {
            return client.deactivate();
        });

    telnetpp::detail::routing_visitor visitor(
        [](auto &&) -> std::vector<telnetpp::token>
        {
            return {};
        },
        cmd_router,
        neg_router,
        sub_router);

    client.activate();
    telnetpp::element do_negotiation_token(
        telnetpp::negotiation(telnetpp::will, client.option()));

    expect_elements(
        {},
        boost::apply_visitor(visitor, do_negotiation_token));

    telnetpp::element sub_token(
        telnetpp::subnegotiation(client.option(), {0, 80, 0, 24}));

    expect_elements({
        telnetpp::negotiation(
            telnetpp::dont, client.option())
        },
        boost::apply_visitor(visitor, sub_token));
}
