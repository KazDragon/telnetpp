#include "telnetpp/options/naws/server.hpp"
#include <gtest/gtest.h>

TEST(naws_server_test, option_is_naws)
{
    telnetpp::options::naws::server server;
    ASSERT_EQ(31, server.option_code());
}

TEST(naws_server_test, activation_with_no_screen_size_sends_nothing)
{
    telnetpp::options::naws::server server;

    std::vector<telnetpp::element> const expected_result = {
    };

    std::vector<telnetpp::element> received_result;
    server.activate([&](auto &&){});
    server.negotiate(
        telnetpp::do_,
        [&](telnetpp::element const &elem)
        {
            received_result.push_back(elem);
        });
    assert(server.active());

    ASSERT_EQ(expected_result, received_result);
}

TEST(naws_server_test, setting_screen_size_when_not_activated_sends_nothing)
{
    telnetpp::options::naws::server server;

    std::vector<telnetpp::element> const expected_result = {
    };

    std::vector<telnetpp::element> received_result;

    server.set_window_size(
        80, 24, 
        [&](telnetpp::element const &elem)
        {
            received_result.push_back(elem);
        });

    ASSERT_EQ(expected_result, received_result);
}

TEST(naws_server_test, activation_with_screen_size_sends_screen_size)
{
    telnetpp::options::naws::server server;
    server.set_window_size(80, 24, [&](auto &&){});

    telnetpp::byte const expected_content[] = {
        0, 80, 0, 24
    };

    std::vector<telnetpp::element> const expected_result = {
        telnetpp::subnegotiation{server.option_code(), expected_content}
    };

    std::vector<telnetpp::element> received_result;
    server.activate([&](auto &&){});
    server.negotiate(
        telnetpp::do_,
        [&](telnetpp::element const &elem)
        {
            received_result.push_back(elem);

            // Have to check here due to the span's short life.
            ASSERT_EQ(expected_result, received_result);
        });
    assert(server.active());

    ASSERT_EQ(size_t{1}, received_result.size());
}

TEST(naws_server_test, setting_screen_size_when_activated_sends_screen_size)
{
    telnetpp::options::naws::server server;
    server.activate([&](auto &&){});
    server.negotiate(telnetpp::do_, [&](auto &&){});

    telnetpp::byte const expected_content[] = {
        0, 80, 0, 24
    };

    std::vector<telnetpp::element> const expected_result = {
        telnetpp::subnegotiation{server.option_code(), expected_content}
    };

    std::vector<telnetpp::element> received_result;
    server.set_window_size(
        80, 24,
        [&](telnetpp::element const &elem)
        {
            received_result.push_back(elem);

            // Have to check here due to the span's short life.
            ASSERT_EQ(expected_result, received_result);
        });

    ASSERT_EQ(size_t{1}, received_result.size());
}
