#ifndef TELNETPP_NEGOTIATION_ROUTER_HPP_
#define TELNETPP_NEGOTIATION_ROUTER_HPP_

#include "telnetpp/router.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/token.hpp"
#include <vector>

namespace telnetpp {

namespace detail {
    
struct negotiation_router_key_from_message_policy
{
    static negotiation key_from_message(negotiation const &neg)
    {
        return neg;
    }
};
    
}

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
        std::vector<token_pass>,
        detail::negotiation_router_key_from_message_policy
    >
{
};

/// =========================================================================
/// \brief Registers a negotiation with a router to route it to a client or
/// server option.
/// =========================================================================
template <class NegotiableOption>
void register_route_from_negotiation_to_option(
    negotiation_router &route,
    u8                  request,
    NegotiableOption   &option)
{
    route.register_route(negotiation(request, option.option()), 
        [&option, request](auto &&neg)
        {
            return option.negotiate(request);
        });
}

}

#endif
