#include "telnetpp/detail/registration.hpp"

namespace telnetpp { namespace detail {

// ==========================================================================
// REGISTER_CLIENT_OPTION
// ==========================================================================
void register_client_option(
    client_option &option,
    negotiation_router &neg_router,
    subnegotiation_router &sub_router)
{
    register_route_from_negotiation_to_option(
        neg_router, telnetpp::will, option);
    register_route_from_negotiation_to_option(
        neg_router, telnetpp::wont, option);
    register_route_from_subnegotiation_to_option(
        sub_router, option);
}

// ==========================================================================
// REGISTER_SERVER_OPTION
// ==========================================================================
void register_server_option(
    server_option &option,
    negotiation_router &neg_router,
    subnegotiation_router &sub_router)
{
    register_route_from_negotiation_to_option(
        neg_router, telnetpp::do_, option);
    register_route_from_negotiation_to_option(
        neg_router, telnetpp::dont, option);
    register_route_from_subnegotiation_to_option(
        sub_router, option);
}

}}
