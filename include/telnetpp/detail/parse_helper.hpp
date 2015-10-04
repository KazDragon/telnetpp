#ifndef TELNETPP_DETAIL_PARSE_HELPER_HPP_
#define TELNETPP_DETAIL_PARSE_HELPER_HPP_

#include "telnetpp/core.hpp"
#include "telnetpp/token.hpp"
#include "telnetpp/detail/parse_state.hpp"
#include <string>

namespace telnetpp { namespace detail {


parse_state parse_idle(
    telnetpp::u8                  byte,
    std::vector<telnetpp::token> &tokens);

parse_state parse_iac(
    telnetpp::u8                  byte,
    std::vector<telnetpp::token> &tokens,
    telnetpp::u8                 &iac_starter);

parse_state parse_negotiation(
    telnetpp::u8                  byte,
    std::vector<telnetpp::token> &tokens,
    telnetpp::u8                  iac_starter);
}}

#endif
