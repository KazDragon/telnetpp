#ifndef TELNETPP_SUBNEGOTIATION_ROUTER_HPP_
#define TELNETPP_SUBNEGOTIATION_ROUTER_HPP_

#include "telnetpp/router.hpp"
#include "telnetpp/subnegotiation.hpp"
#include "telnetpp/token.hpp"
#include <vector>

namespace telnetpp {
    
namespace detail {

struct subnegotiation_router_key_from_message_policy
{
    static telnetpp::u8 key_from_message(subnegotiation const &sub)
    {
        return sub.option();
    }
};

}

class subnegotiation_router
  : public router<
        u8,
        subnegotiation,
        std::vector<token_pass>,
        detail::subnegotiation_router_key_from_message_policy
    >
{
};
 
template <class SubnegotiableOption>
void register_route_from_subnegotiation_to_option(
    subnegotiation_router &route,
    SubnegotiableOption   &option)
{
    route.register_route(option.option(), 
        [&option](auto &&sub)
        {
            return option.subnegotiate(sub.content());
        });
}

}

#endif
