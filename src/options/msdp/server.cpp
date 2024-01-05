#include "telnetpp/options/msdp/server.hpp"
#include "telnetpp/options/msdp/detail/decoder.hpp"
#include "telnetpp/options/msdp/detail/encoder.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"

namespace telnetpp::options::msdp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server(telnetpp::session &sess) noexcept
  : telnetpp::server_option(sess, telnetpp::options::msdp::detail::option)
{
}

// ==========================================================================
// SEND
// ==========================================================================
void server::send(variable const &var)
{
  detail::encode(
      var, [this](telnetpp::bytes data) { write_subnegotiation(data); });
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void server::handle_subnegotiation(telnetpp::bytes data)
{
  detail::decode(data, [&](auto const &var) { on_receive(var); });
}

}  // namespace telnetpp::options::msdp
