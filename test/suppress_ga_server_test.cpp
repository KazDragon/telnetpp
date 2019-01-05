#include "telnetpp/options/suppress_ga/server.hpp"
#include <gtest/gtest.h>

TEST(suppress_ga_server_test, option_is_suppress_ga)
{
    telnetpp::options::suppress_ga::server server;
    ASSERT_EQ(3, server.option_code());
}

TEST(suppress_ga_server_test, subnegotiation_returns_nothing)
{
    telnetpp::options::suppress_ga::server server;
    server.activate([](auto &&){});
    server.negotiate(telnetpp::do_, [](auto &&){});
    assert(server.active());

    static constexpr telnetpp::byte const content[] = { 0x00 };

    std::vector<telnetpp::element> const expected_elements = {
    };

    std::vector<telnetpp::element> received_elements;
    server.subnegotiate(
        content, 
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_EQ(expected_elements, received_elements);
}