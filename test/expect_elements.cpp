#include "expect_elements.hpp"
#include <gtest/gtest.h>

namespace {
    
struct elements_match : boost::static_visitor<>
{
public :
    elements_match(telnetpp::element const &expected)
      : expected_(expected)
    {
    }
    
    template <class T>
    void operator()(T const &value) const
    {
        ASSERT_TRUE(expected_.type() == typeid(value));
        ASSERT_EQ(boost::get<T>(expected_), value);
    }
    
    telnetpp::element const &expected_;
};

struct tokens_match : boost::static_visitor<>
{
    tokens_match(telnetpp::token const &expected)
      : expected_(expected)
    {
    }
    
    void operator()(boost::any const &any) const
    {
        ASSERT_TRUE(expected_.type() == typeid(boost::any));
        
        // Unfortunately, we can't actually compare the anys, because we have
        // no idea what they are by design.
    }
    
    void operator()(telnetpp::element const &tok) const
    {
        ASSERT_TRUE(expected_.type() == typeid(telnetpp::element));
        
        boost::apply_visitor(
            elements_match(boost::get<telnetpp::element>(expected_)),
            tok);
    }
    
    telnetpp::token const &expected_;
};

struct stream_tokens_match : boost::static_visitor<>
{
    stream_tokens_match(telnetpp::stream_token const &expected)
      : expected_(expected)
    {
    }
    
    void operator()(boost::any const &any) const
    {
        ASSERT_TRUE(expected_.type() == typeid(boost::any));
        
        // See above.
    }
    
    void operator()(telnetpp::u8stream const &stream) const
    {
        auto *expected_stream = boost::get<telnetpp::u8stream>(&expected_);
        ASSERT_TRUE(expected_stream != nullptr);
        ASSERT_EQ(*expected_stream, stream);
    }
    
    telnetpp::stream_token const &expected_;
};

}

void expect_elements(
    std::vector<telnetpp::element> const &expected, 
    std::vector<telnetpp::element> const &result)
{
    ASSERT_EQ(expected.size(), result.size());
    
    for (auto &&current_expected = expected.begin(),
              &&current_result   = result.begin();
         current_expected != expected.end()
      && current_result != result.end();
         ++current_expected,
         ++current_result)
    {
        boost::apply_visitor(elements_match(
            *current_expected), *current_result);
    }
}

void expect_elements(
    std::vector<telnetpp::element> const &expected,
    std::vector<telnetpp::token> const &result)
{
    ASSERT_EQ(expected.size(), result.size());
    
    
    auto &&current_expected = expected.begin();
    auto &&current_result   = result.begin();
    
    for (;
         current_expected != expected.end()
      && current_result != result.end();
         ++current_expected,
         ++current_result)
    {
        boost::apply_visitor(
            elements_match(*current_expected),
            boost::get<telnetpp::element>(*current_result));
    }
}

void expect_tokens(
    std::vector<telnetpp::token> const &expected, 
    std::vector<telnetpp::token> const &result)
{
    ASSERT_EQ(expected.size(), result.size());
    
    for (auto &&current_expected = expected.begin(),
              &&current_result   = result.begin();
         current_expected != expected.end()
      && current_result != result.end();
         ++current_expected,
         ++current_result)
    {
        boost::apply_visitor(tokens_match(*current_expected), *current_result);
    }
}

void expect_tokens(
    std::vector<telnetpp::stream_token> const &expected, 
    std::vector<telnetpp::stream_token> const &result)
{
    ASSERT_EQ(expected.size(), result.size());
    
    for (auto &&current_expected = expected.begin(),
              &&current_result   = result.begin();
         current_expected != expected.end()
      && current_result != result.end();
         ++current_expected,
         ++current_result)
    {
        boost::apply_visitor(
            stream_tokens_match(*current_expected), *current_result);
    }
}
