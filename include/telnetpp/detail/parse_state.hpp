#ifndef TELNETPP_DETAIL_PARSE_STATE_HPP_
#define TELNETPP_DETAIL_PARSE_STATE_HPP_

namespace telnetpp { namespace detail {
    
enum class parse_state
{
    idle,
    iac,
    negotiation,
    subnegotiation,
};

}}


#endif
