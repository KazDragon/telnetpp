#include "telnetpp/options/mccp/server.hpp"
#include "telnetpp/options/mccp/codec.hpp"

namespace telnetpp::options::mccp {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server(telnetpp::session &sess, codec &cdc)
  : basic_server(sess), codec_(cdc), compression_active_(false)
{
  on_state_changed.connect(
      [this]()
      {
        if (compression_active_)
        {
          codec_.finish([&](telnetpp::bytes data, bool) { write_text(data); });

          compression_active_ = false;
        }
      });
}

// ==========================================================================
// BEGIN_COMPRESSION
// ==========================================================================
void server::start_compression()
{
  if (active())
  {
    write_subnegotiation({});
    codec_.start();
    compression_active_ = true;
  }
}

// ==========================================================================
// END_COMPRESSION
// ==========================================================================
void server::finish_compression()
{
  if (active() && compression_active_)
  {
    codec_.finish([&](telnetpp::bytes data, bool) { write_text(data); });

    compression_active_ = false;
  }
}

}  // namespace telnetpp::options::mccp
