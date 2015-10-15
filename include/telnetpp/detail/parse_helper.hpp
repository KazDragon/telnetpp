#ifndef TELNETPP_DETAIL_PARSE_HELPER_HPP_
#define TELNETPP_DETAIL_PARSE_HELPER_HPP_

#include "telnetpp/core.hpp"
#include "telnetpp/element.hpp"

namespace telnetpp { namespace detail {

enum class parse_state
{
    idle,
    iac,
    negotiation,
    subnegotiation,
    subnegotiation_content,
    subnegotiation_content_iac
};

struct parse_temps
{
    std::vector<telnetpp::element> elements;
    u8stream                       subnegotiation_content;
    
    parse_state                    state = parse_state::idle;
    u8                             id;
};

void parse_helper(
    parse_temps &temps,
    telnetpp::u8 byte);

}}

#endif
