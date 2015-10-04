#ifndef TELNETPP_DETAIL_PARSE_STATE_HPP_
#define TELNETPP_DETAIL_PARSE_STATE_HPP_

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
    u8 id;
    std::vector<u8> content;
};

}}


#endif
