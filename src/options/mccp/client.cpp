#include "telnetpp/options/mccp/client.hpp"
#include "telnetpp/options/mccp/codec.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp::options::mccp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client(telnetpp::session &sess, codec &cdc)
  : client_option(sess, detail::option), codec_(cdc)
{
  on_state_changed.connect(
      [this]()
      {
        if (!active())
        {
          codec_.finish([&](telnetpp::bytes data, bool) { write_text(data); });
        }
      });
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(telnetpp::bytes /*data*/)
{
  codec_.start();
}

}  // namespace telnetpp::options::mccp
