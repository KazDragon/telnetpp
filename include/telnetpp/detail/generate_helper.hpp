#ifndef TELNETPP_DETAIL_GENERATE_HELPER_HPP_
#define TELNETPP_DETAIL_GENERATE_HELPER_HPP_

#include "telnetpp/token.hpp"

namespace telnetpp { namespace detail {

void generate_helper(
    std::vector<telnetpp::u8> &result,
    telnetpp::token const &token);

}}

#endif
