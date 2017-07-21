#include "telnetpp/detail/generator.hpp"
#include <gtest/gtest.h>

namespace {

template <class InputIterator>
void expect(
    InputIterator begin,
    InputIterator end,
    telnetpp::byte_stream const &expectation)
{
    auto result = telnetpp::detail::generate(begin, end);

    if (expectation.empty())
    {
        ASSERT_EQ(size_t{0}, result.size());
    }
    else
    {
        auto stream = boost::get<telnetpp::byte_stream>(result[0]);

        ASSERT_EQ(expectation.size(), stream.size());

        for (size_t index = 0; index < expectation.size(); ++index)
        {
            ASSERT_EQ(expectation[index], stream[index]);
        }
    }
}

template <class Collection>
void expect(
    Collection &&collection,
    telnetpp::byte_stream const &expectation)
{
    auto result = telnetpp::detail::generate(std::forward<Collection>(collection));

    if (expectation.empty())
    {
        ASSERT_EQ(size_t{0}, result.size());
    }
    else
    {
        auto stream = boost::get<telnetpp::byte_stream>(
            result[0]);

        ASSERT_EQ(expectation.size(), stream.size());

        for (size_t index = 0; index < expectation.size(); ++index)
        {
            ASSERT_EQ(expectation[index], stream[index]);
        }
    }
}

}

TEST(generator_test, empty_array_generates_nothing)
{
    std::vector<telnetpp::token> data = {};
    telnetpp::byte_stream expected = {};

    expect(data.begin(), data.end(), expected);
}

TEST(generator_test, empty_string_generates_nothing)
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(std::string(""))
    };

    telnetpp::byte_stream expected = {};

    expect(data.begin(), data.end(), expected);
}

TEST(generator_test, string_with_content_generates_string)
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(std::string("abcde"))
    };

    telnetpp::byte_stream expected = { 'a', 'b', 'c', 'd', 'e' };

    expect(data.begin(), data.end(), expected);
}

TEST(generator_test, string_with_iac_content_generates_doubled_iac)
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(std::string("ab\xFF""cd"))
    };

    telnetpp::byte_stream expected = { 'a', 'b', 0xFF, 0xFF, 'c', 'd' };

    expect(data.begin(), data.end(), expected);
}

TEST(generator_test, command_generates_command)
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(telnetpp::command(telnetpp::nop))
    };

    telnetpp::byte_stream expected = { 0xFF, 0xF1 };

    expect(data.begin(), data.end(), expected);
}

TEST(generator_test, negotiation_generates_negotiation)
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(telnetpp::negotiation(telnetpp::will, 0xDE))
    };

    telnetpp::byte_stream expected = { 0xFF, 0xFB, 0xDE };

    expect(data.begin(), data.end(), expected);
}

TEST(generator_test, empty_subnegotiation_generates_empty_subnegotiation)
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(telnetpp::subnegotiation(0xAB, {}))
    };

    telnetpp::byte_stream expected = { 0xFF, 0xFA, 0xAB, 0xFF, 0xF0 };

    expect(data.begin(), data.end(), expected);
}

TEST(generator_test, subnegotiation_with_content_generates_subnegotiation_with_content)
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(telnetpp::subnegotiation(0xCD, { 'a', 'b', 'c', 'd', 'e' }))
    };

    telnetpp::byte_stream expected = {
        0xFF, 0xFA,
        0xCD,
        'a', 'b', 'c', 'd', 'e',
        0xFF, 0xF0
    };

    expect(data.begin(), data.end(), expected);
}

TEST(generator_test, subnegotiation_with_iac_content_generates_doubled_iac)
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(telnetpp::subnegotiation(0x74, { 'a', 'b', 0xFF, 'c', 'd' }))
    };

    telnetpp::byte_stream expected = {
        0xFF, 0xFA,
        0x74,
        'a', 'b', 0xFF, 0xFF, 'c', 'd',
        0xFF, 0xF0
    };

    expect(data.begin(), data.end(), expected);
}

TEST(generator_test, many_elements_generates_many_elements)
{
    std::vector<telnetpp::token> data = {
        telnetpp::element(std::string("abcd")),
        telnetpp::element(std::string("ef\xFFg")),
        telnetpp::element(telnetpp::negotiation(telnetpp::wont, 0xFF)),
        telnetpp::element(telnetpp::subnegotiation(0xFF, { 0xFF }))
    };

    telnetpp::byte_stream expected = {
        'a', 'b', 'c', 'd',
        'e', 'f', 0xFF, 0xFF, 'g',
        0xFF, 0xFC, 0xFF,
        0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0
    };

    expect(data, expected);
}
