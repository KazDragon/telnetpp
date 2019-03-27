#include "telnetpp/options/terminal_type/client.hpp"
#include <gtest/gtest.h>

TEST(terminal_type_client_test, option_is_terminal_type)
{
    telnetpp::options::terminal_type::client client;
    ASSERT_EQ(24, client.option_code());
}

TEST(terminal_type_client_test, requesting_terminal_type_sends_terminal_type_request)
{
    telnetpp::options::terminal_type::client client;
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});
    assert(client.active());

    static constexpr telnetpp::byte const expected_content[] = {
        0x01
    };

    std::vector<telnetpp::element> const expected_elements = {
        telnetpp::subnegotiation{client.option_code(), expected_content}
    };

    std::vector<telnetpp::element> received_elements;
    client.request_terminal_type(
        [&received_elements](telnetpp::element const &elem)
        {
            received_elements.push_back(elem);
        });

    ASSERT_EQ(expected_elements, received_elements);
}

TEST(terminal_type_client_test, receiving_terminal_type_reports_terminal_type)
{
    telnetpp::options::terminal_type::client client;
    client.activate([](auto &&){});
    client.negotiate(telnetpp::will, [](auto &&){});
    assert(client.active());

    std::string terminal_type;
    client.on_terminal_type.connect(
        [&terminal_type](telnetpp::bytes content, auto &&cont)
        {
            terminal_type.reserve(content.size());
            std::transform(
                content.begin(), 
                content.end(), 
                std::back_inserter(terminal_type),
                [](auto ch)
                {
                    return static_cast<char>(ch);
                });
        });

    static std::string const expected_terminal_type = "abc";
    static constexpr telnetpp::byte const content[] = {
        0x00, 'a', 'b', 'c'
    };

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
    ASSERT_EQ(expected_terminal_type, terminal_type);
}
