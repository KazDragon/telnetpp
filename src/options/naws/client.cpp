#include "telnetpp/options/naws/client.hpp"
#include "telnetpp/options/naws/detail/protocol.hpp"

namespace telnetpp::options::naws {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client(telnetpp::session &sess) noexcept
  : client_option(sess, telnetpp::options::naws::detail::option)
{
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(telnetpp::bytes content)
{
  if (content.size() == sizeof(window_dimension) + sizeof(window_dimension))
  {
    window_dimension width = content[0] << 8 | content[1];
    window_dimension height = content[2] << 8 | content[3];

    on_window_size_changed(width, height);
  }
}

}  // namespace telnetpp::options::naws
