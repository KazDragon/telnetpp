#ifndef TELNETPP_DETAIL_NEGOTIATION_ROUTER_HPP_
#define TELNETPP_DETAIL_NEGOTIATION_ROUTER_HPP_

#include "telnetpp/detail/router.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/element.hpp"
#include <vector>

namespace telnetpp { namespace detail {
    
struct negotiation_router_key_from_message_policy
{
    static negotiation key_from_message(negotiation const &neg)
    {
        return neg;
    }
};

//* =========================================================================
/// \brief A structure that can route incoming negotiations to their 
/// associated options.  
///
/// This can be used as a multiplexer between incoming
/// negotiations and the set of supported client and server options.
/// =========================================================================
class negotiation_router
  : public router<
        negotiation, 
        negotiation,
        std::vector<telnetpp::token>,
        detail::negotiation_router_key_from_message_policy
    >
{
};

}}

#endif
