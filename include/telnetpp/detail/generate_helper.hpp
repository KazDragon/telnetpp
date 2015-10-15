#ifndef TELNETPP_DETAIL_GENERATE_HELPER_HPP_
#define TELNETPP_DETAIL_GENERATE_HELPER_HPP_

#include "telnetpp/element.hpp"

namespace telnetpp { namespace detail {

void generate_helper(
    u8stream &result,
    telnetpp::element const &element);

}}

#endif
