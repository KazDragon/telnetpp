#include "telnetpp/options/new_environ/client.hpp"
#include "telnetpp/options/new_environ/detail/for_each_response.hpp"

namespace telnetpp { namespace options { namespace new_environ {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : client_option(telnetpp::options::new_environ::detail::option)
{
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(
    telnetpp::bytes data,
    continuation const &cont)
{
    detail::for_each_response(
      data,
      [&](response const &rsp)
      {
        on_variable_changed(rsp, cont);
      });
}

}}}
