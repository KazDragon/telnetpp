#include <telnetpp/generator.hpp>
#include <gtest/gtest.h>

namespace {

class generator_test : public testing::Test
{
protected:
    void generate(gsl::span<telnetpp::element const> elements)
    {
        telnetpp::generate(
            elements,
            [this](telnetpp::bytes data)
            {
                output.insert(output.end(), data.begin(), data.end());
            });
    }

    std::vector<telnetpp::byte> output;
};

}

TEST_F(generator_test, empty_array_generates_nothing)
{
    std::vector<telnetpp::element> data = {};
    std::vector<telnetpp::byte> expected = {};

    generate(data);

    ASSERT_EQ(output, expected);
}

TEST_F(generator_test, empty_span_generates_nothing)
{
    std::vector<telnetpp::element> data = {
        telnetpp::element{telnetpp::bytes{}}
    };

    std::vector<telnetpp::byte> expected = {};

    generate(data);

    ASSERT_EQ(output, expected);
}

TEST_F(generator_test, span_with_content_generates_string)
{
    static constexpr telnetpp::byte const content[] = { 'a', 'b', 'c', 'd', 'e' };

    std::vector<telnetpp::element> data = {
        telnetpp::element{content}
    };

    std::vector<telnetpp::byte> expected = { 'a', 'b', 'c', 'd', 'e' };

    generate(data);

    ASSERT_EQ(output, expected);
}

TEST_F(generator_test, string_with_iac_content_generates_doubled_iac)
{
    static constexpr telnetpp::byte const content[] = { 'a', 'b', 0xFF, 'c', 'd' };
    std::vector<telnetpp::element> data = {
        telnetpp::element{content}
    };

    std::vector<telnetpp::byte> expected = { 'a', 'b', 0xFF, 0xFF, 'c', 'd' };

    generate(data);

    ASSERT_EQ(output, expected);
}

TEST_F(generator_test, command_generates_command)
{
    std::vector<telnetpp::element> data = {
        telnetpp::element{telnetpp::command{telnetpp::nop}}
    };

    std::vector<telnetpp::byte> expected = { 0xFF, 0xF1 };

    generate(data);

    ASSERT_EQ(output, expected);
}

TEST_F(generator_test, negotiation_generates_negotiation)
{
    std::vector<telnetpp::element> data = {
        telnetpp::element{telnetpp::negotiation{telnetpp::will, 0xDE}}
    };

    std::vector<telnetpp::byte> expected = { 0xFF, 0xFB, 0xDE };

    generate(data);

    ASSERT_EQ(output, expected);
}

TEST_F(generator_test, empty_subnegotiation_generates_empty_subnegotiation)
{
    std::vector<telnetpp::element> data = {
        telnetpp::element{telnetpp::subnegotiation{0xAB, {}}}
    };

    std::vector<telnetpp::byte> expected = { 0xFF, 0xFA, 0xAB, 0xFF, 0xF0 };

    generate(data);

    ASSERT_EQ(output, expected);
}

TEST_F(generator_test, subnegotiation_with_content_generates_subnegotiation_with_content)
{
    static constexpr telnetpp::byte const content[] = { 'a', 'b', 'c', 'd', 'e' };

    std::vector<telnetpp::element> data = {
        telnetpp::element{telnetpp::subnegotiation{0xCD, content}}
    };

    std::vector<telnetpp::byte> expected = {
        0xFF, 0xFA,
        0xCD,
        'a', 'b', 'c', 'd', 'e',
        0xFF, 0xF0
    };

    generate(data);

    ASSERT_EQ(output, expected);
}

TEST_F(generator_test, subnegotiation_with_iac_content_generates_doubled_iac)
{
    static constexpr telnetpp::byte const content[] = { 'a', 'b', 0xFF, 'c', 'd' };

    std::vector<telnetpp::element> data = {
        telnetpp::element{telnetpp::subnegotiation{0xCD, content}}
    };

    std::vector<telnetpp::byte> expected = {
        0xFF, 0xFA,
        0xCD,
        'a', 'b', 0xFF, 0xFF, 'c', 'd',
        0xFF, 0xF0
    };

    generate(data);

    ASSERT_EQ(output, expected);
}

TEST_F(generator_test, many_elements_generates_many_elements)
{
    static constexpr telnetpp::byte const content0[] = { 'a', 'b', 'c', 'd' };
    static constexpr telnetpp::byte const content1[] = { 'e', 'f', 0xFF, 'g' };
    static constexpr telnetpp::byte const content3[] = { 0xFF };
    
    std::vector<telnetpp::element> data = {
        telnetpp::element{content0},
        telnetpp::element{content1},
        telnetpp::element{telnetpp::negotiation{telnetpp::wont, 0xFF}},
        telnetpp::element{telnetpp::subnegotiation{0xFF, content3}}
    };

    std::vector<telnetpp::byte> expected = {
        'a', 'b', 'c', 'd',
        'e', 'f', 0xFF, 0xFF, 'g',
        0xFF, 0xFC, 0xFF,
        0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0
    };

    generate(data);

    ASSERT_EQ(output, expected);
}
