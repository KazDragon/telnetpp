#ifndef TELNETPP_TEST_EXPECT_TOKENS_HPP_
#define TELNETPP_TEST_EXPECT_TOKENS_HPP_

#include "telnetpp/token.hpp"
#include <vector>

void expect_tokens(
    std::vector<telnetpp::token> const &expected,
    std::vector<telnetpp::token> const &result);

#endif
