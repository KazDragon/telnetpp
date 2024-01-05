#include "telnetpp/options/terminal_type/client.hpp"
#include "telnetpp/options/terminal_type/detail/protocol.hpp"

namespace telnetpp::options::terminal_type {

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client(telnetpp::session &sess)
  : client_option(sess, telnetpp::options::terminal_type::detail::option)
{
}

// ==========================================================================
// REQUEST_TERMINAL_TYPE
// ==========================================================================
void client::request_terminal_type()
{
  static constexpr telnetpp::byte const request_content[] = {detail::send};

  write_subnegotiation(request_content);
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void client::handle_subnegotiation(telnetpp::bytes data)
{
  if (!data.empty() && data[0] == detail::is)
  {
    on_terminal_type(data.subspan(1));
  }
}

}  // namespace telnetpp::options::terminal_type
