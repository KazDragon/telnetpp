#include "expect_tokens.hpp"
#include <cppunit/TestAssert.h>

namespace {
    
struct tokens_match : boost::static_visitor<>
{
public :
    tokens_match(telnetpp::token const &expected)
      : expected_(expected)
    {
    }
    
    template <class T>
    void operator()(T const &value) const
    {
        CPPUNIT_ASSERT(expected_.type() == typeid(value));
        CPPUNIT_ASSERT_EQUAL(boost::get<T>(expected_), value);
    }
    
    telnetpp::token const &expected_;
};

}

void expect_tokens(
    std::vector<telnetpp::token> const &expected, 
    std::vector<telnetpp::token> const &result)
{
    CPPUNIT_ASSERT_EQUAL(expected.size(), result.size());
    
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
