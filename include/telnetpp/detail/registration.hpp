#pragma once

#include "telnetpp/element.hpp"
#include "telnetpp/detail/negotiation_router.hpp"
#include "telnetpp/detail/subnegotiation_router.hpp"
#include "telnetpp/client_option.hpp"
#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace detail {

//* =========================================================================
/// \brief Registers an option with negotiation and subnegotiation routers.
//* =========================================================================
void register_client_option(
    telnetpp::client_option &option,
    negotiation_router &neg_router,
    subnegotiation_router &sub_router);

//* =========================================================================
/// \brief Registers an option with negotiation and subnegotiation routers.
//* =========================================================================
void register_server_option(
    telnetpp::server_option &option,
    negotiation_router &neg_router,
    subnegotiation_router &sub_router);

/// =========================================================================
/// \brief Registers a negotiation with a router to route it to a client or
/// server option.
/// =========================================================================
template <class NegotiableOption>
void register_route_from_negotiation_to_option(
    negotiation_router &route,
    negotiation_type    request,
    NegotiableOption   &option)
{
    route.register_route(
        negotiation{request, option.option_code()},
        [&option, request](telnetpp::negotiation const &)
        {
            return option.negotiate(request);
        });
}

//* =========================================================================
/// \brief Registers an option to receive subnegotiations for it.
//* =========================================================================
template <class SubnegotiableOption>
void register_route_from_subnegotiation_to_option(
    subnegotiation_router &route,
    SubnegotiableOption   &option)
{
    route.register_route(option.option_code(),
        [&option](telnetpp::subnegotiation const &sub)
        {
            return option.subnegotiate(sub.content());
        });
}

}}
