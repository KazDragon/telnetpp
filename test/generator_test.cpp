#include <gtest/gtest.h>
#include <telnetpp/generator.hpp>

namespace {

class generator_test : public testing::Test
{
protected:
    void generate(telnetpp::element const &elem)
    {
        telnetpp::generate(elem, [this](telnetpp::bytes data) {
            output_.insert(output_.end(), data.begin(), data.end());
        });
    }

    std::vector<telnetpp::byte> output_;
};

}  // namespace

TEST_F(generator_test, span_with_content_generates_string)
{
    static constexpr telnetpp::byte const content[] = {'a', 'b', 'c', 'd', 'e'};
    std::vector<telnetpp::byte> expected = {'a', 'b', 'c', 'd', 'e'};

    generate(content);

    ASSERT_EQ(output_, expected);
}

TEST_F(generator_test, string_with_iac_content_generates_doubled_iac)
{
    static constexpr telnetpp::byte const content[] = {
        'a', 'b', 0xFF, 'c', 'd'};
    std::vector<telnetpp::byte> expected = {'a', 'b', 0xFF, 0xFF, 'c', 'd'};

    generate(content);

    ASSERT_EQ(output_, expected);
}

TEST_F(generator_test, command_generates_command)
{
    std::vector<telnetpp::byte> expected = {0xFF, 0xF1};

    generate(telnetpp::command{telnetpp::nop});

    ASSERT_EQ(output_, expected);
}

TEST_F(generator_test, negotiation_generates_negotiation)
{
    std::vector<telnetpp::byte> expected = {0xFF, 0xFB, 0xDE};

    generate(telnetpp::negotiation{telnetpp::will, 0xDE});

    ASSERT_EQ(output_, expected);
}

TEST_F(generator_test, empty_subnegotiation_generates_empty_subnegotiation)
{
    std::vector<telnetpp::byte> expected = {0xFF, 0xFA, 0xAB, 0xFF, 0xF0};

    generate(telnetpp::subnegotiation{0xAB, {}});

    ASSERT_EQ(output_, expected);
}

TEST_F(
    generator_test,
    subnegotiation_with_content_generates_subnegotiation_with_content)
{
    static constexpr telnetpp::byte const content[] = {'a', 'b', 'c', 'd', 'e'};

    std::vector<telnetpp::byte> expected = {
        0xFF, 0xFA, 0xCD, 'a', 'b', 'c', 'd', 'e', 0xFF, 0xF0};

    generate(telnetpp::subnegotiation{0xCD, content});

    ASSERT_EQ(output_, expected);
}

TEST_F(generator_test, subnegotiation_with_iac_content_generates_doubled_iac)
{
    static constexpr telnetpp::byte const content[] = {
        'a', 'b', 0xFF, 'c', 'd'};

    std::vector<telnetpp::byte> expected = {
        0xFF, 0xFA, 0xCD, 'a', 'b', 0xFF, 0xFF, 'c', 'd', 0xFF, 0xF0};

    generate(telnetpp::subnegotiation{0xCD, content});

    ASSERT_EQ(output_, expected);
}
