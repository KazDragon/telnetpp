#ifndef TELNETPP_TEST_EXPECT_ELEMENTS_HPP_
#define TELNETPP_TEST_EXPECT_ELEMENTS_HPP_

#include "telnetpp/element.hpp"
#include <vector>

void expect_elements(
    std::vector<telnetpp::element> const &expected,
    std::vector<telnetpp::element> const &result);

void expect_tokens(
    std::vector<telnetpp::token> const &expected,
    std::vector<telnetpp::token> const &result);

#endif
