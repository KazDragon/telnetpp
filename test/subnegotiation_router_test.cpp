#include "telnetpp/detail/subnegotiation_router.hpp"
#include "telnetpp/detail/registration.hpp"
#include <gtest/gtest.h>

namespace {

class fake_client_option : public telnetpp::client_option
{
public:
    explicit fake_client_option(telnetpp::option_type option)
      : telnetpp::client_option(option)
    {
    }

    boost::signals2::signal<
        void (telnetpp::bytes data)
    > on_subnegotiation;

private:
    void handle_subnegotiation(
        telnetpp::bytes data,
        std::function<void (telnetpp::element const &)> const &cont) override
    {
        on_subnegotiation(data);
    }
};

}

TEST(subnegotiation_router_test, when_nothing_is_registered_router_sinks_data)
{
    telnetpp::detail::subnegotiation_router router;

    bool called = false;
    router(
        telnetpp::subnegotiation{0, {}},
        [&called](auto &&...)
        {
            called = true;
        });

    ASSERT_FALSE(called);
}

TEST(subnegotiation_router_test, message_with_registered_key_goes_to_registered_function)
{
    telnetpp::detail::subnegotiation_router router;

    static constexpr telnetpp::byte const content[] = {
        0x02
    };

    telnetpp::subnegotiation sub(0x00, {});
    telnetpp::subnegotiation expected(0x01, content);
    bool unregistered_route_called = false;

    router.register_route(
        expected.option(),
        [&sub](auto &&new_subnegotiation, auto &&cont)
        {
            sub = new_subnegotiation;
        });

    router.set_unregistered_route(
        [&unregistered_route_called](auto &&, auto &&cont)
        {
            unregistered_route_called = true;
        });

    router(expected, [](auto &&){});

    ASSERT_EQ(expected, sub);
    ASSERT_EQ(false, unregistered_route_called);
}

TEST(subnegotiation_router_test, message_with_unregistered_key_goes_to_unregistered_function)
{
    telnetpp::detail::subnegotiation_router router;

    static constexpr telnetpp::byte const content[] = {
        0x02
    };

    telnetpp::subnegotiation sub(0x00, {});
    telnetpp::subnegotiation expected(0x01, content);
    telnetpp::subnegotiation unexpected(0x02, content);

    bool registered_route_called = false;

    router.register_route(
        unexpected.option(),
        [&registered_route_called](auto &&, auto &&cont)
        {
            registered_route_called = true;
        });

    router.set_unregistered_route(
        [&sub](auto &&new_subnegotiation, auto &&cont)
        {
            sub = new_subnegotiation;
        });


    router(expected, [](auto &&){});

    ASSERT_EQ(false, registered_route_called);
    ASSERT_EQ(expected, sub);
}

TEST(subnegotiation_router_test, routing_subnegotiation_returns_subnegotiation_result)
{
    telnetpp::detail::subnegotiation_router router;
    fake_client_option client{0xC4};
    client.negotiate(telnetpp::will, [](auto &&){});
    assert(client.active());

    telnetpp::detail::register_route_from_subnegotiation_to_option(router, client);

    std::vector<telnetpp::byte> const expected_subnegotiation = {
        'T', 'E', 'S', 'T'
    };
    
    std::vector<telnetpp::byte> received_subnegotiation;
    client.on_subnegotiation.connect(
        [&](telnetpp::bytes data)
        {
            received_subnegotiation.insert(
                received_subnegotiation.end(),
                data.begin(),
                data.end());
        });

    router(telnetpp::subnegotiation{
        client.code(), 
        expected_subnegotiation},
        [](auto &&){});

    ASSERT_EQ(expected_subnegotiation, received_subnegotiation);
}
