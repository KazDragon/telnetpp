#ifndef TELNETPP_TEST_EXPECT_ELEMENTS_HPP_
#define TELNETPP_TEST_EXPECT_ELEMENTS_HPP_

#include "telnetpp/element.hpp"
#include <vector>

// Check that the elements of two vectors match.
void expect_elements(
    std::vector<telnetpp::element> const &expected,
    std::vector<telnetpp::element> const &result);

// Check that the elements of two vectors match; assumes that the result vector
// contains no non-element entries.
void expect_elements(
    std::vector<telnetpp::element> const &expected,
    std::vector<telnetpp::token> const &result);

// Check that the tokens of two vectors match.
void expect_tokens(
    std::vector<telnetpp::token> const &expected,
    std::vector<telnetpp::token> const &result);

#endif
