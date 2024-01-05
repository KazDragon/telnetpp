#include "telnetpp/detail/registration.hpp"

namespace telnetpp::detail {

// ==========================================================================
// REGISTER_CLIENT_OPTION
// ==========================================================================
void register_client_option(
    client_option &option,
    negotiation_router &neg_router,
    subnegotiation_router &sub_router)
{
  register_route_from_negotiation_to_option(
      neg_router, telnetpp::client_option::remote_positive, option);
  register_route_from_negotiation_to_option(
      neg_router, telnetpp::client_option::remote_negative, option);
  register_route_from_subnegotiation_to_option(sub_router, option);
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
      neg_router, telnetpp::server_option::remote_positive, option);
  register_route_from_negotiation_to_option(
      neg_router, telnetpp::server_option::remote_negative, option);
  register_route_from_subnegotiation_to_option(sub_router, option);
}

}  // namespace telnetpp::detail
