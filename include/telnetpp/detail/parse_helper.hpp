#pragma once

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
    byte_stream                    subnegotiation_content;

    parse_state                    state = parse_state::idle;
    byte                           id;
};

void parse_helper(parse_temps &temps, byte data);

}}
