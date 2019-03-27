#include "telnetpp/options/echo/client.hpp"
#include <gtest/gtest.h>

using namespace telnetpp::literals;

TEST(echo_client_test, option_is_echo)
{
    telnetpp::options::echo::client client;
    ASSERT_EQ(1, client.option_code());
}

TEST(echo_client_test, subnegotiation_returns_nothing)
{
    telnetpp::options::echo::client client;
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});
    assert(client.active());

    static auto const content = "\x00"_tb;

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    client.subnegotiate(
        content, 
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_EQ(expected_elements, received_elements);
}