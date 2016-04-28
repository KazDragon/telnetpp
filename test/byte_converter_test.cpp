#include <telnetpp/byte_converter.hpp>
#include <gtest/gtest.h>

TEST(token_filter_test, sending_an_empty_list_returns_an_empty_array)
{
    telnetpp::byte_converter converter;

    auto expected = telnetpp::u8stream {};
    ASSERT_EQ(expected, converter.send({}));
}

TEST(token_filter_test, sending_a_list_with_a_non_stream_element_returns_an_empty_array)
{
    struct tag {};

    telnetpp::byte_converter converter;

    auto expected = telnetpp::u8stream {};
    ASSERT_EQ(expected, converter.send({ boost::any(tag{}) }));
}

TEST(token_filter_test, sending_a_list_with_a_stream_element_returns_that_stream)
{
    telnetpp::byte_converter converter;

    auto expected = telnetpp::u8stream { 't', 'e', 's', 't' };
    ASSERT_EQ(expected, converter.send({
        telnetpp::u8stream { 't', 'e', 's', 't' }
    }));
}

TEST(token_filter_test, sending_a_list_of_two_stream_elements_returns_those_elements_concatenated)
{
    telnetpp::byte_converter converter;

    auto expected =
        telnetpp::u8stream { 't', 'e', 's', 't', 'd', 'a', 't', 'a' };

    ASSERT_EQ(expected, converter.send({
        telnetpp::u8stream { 't', 'e', 's', 't' },
        telnetpp::u8stream { 'd', 'a', 't', 'a' }
    }));
}

TEST(token_filter_test, sending_two_stream_elements_with_a_tag_between_them_returns_the_elements_concatenated)
{
    telnetpp::byte_converter converter;
    struct tag {};

    auto expected =
        telnetpp::u8stream { 't', 'e', 's', 't', 'd', 'a', 't', 'a' };

    ASSERT_EQ(expected, converter.send({
        telnetpp::u8stream { 't', 'e', 's', 't' },
        boost::any(tag{}),
        telnetpp::u8stream { 'd', 'a', 't', 'a' }
    }));
}