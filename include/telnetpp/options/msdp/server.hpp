#pragma once

#include "telnetpp/options/msdp/variable.hpp"
#include "telnetpp/server_option.hpp"

namespace telnetpp::options::msdp {

//* =========================================================================
/// \brief An implementation of the server side of an MSDP Telnet option.
//* =========================================================================
class TELNETPP_EXPORT server : public telnetpp::server_option
{
 public:
  //* =====================================================================
  /// \brief Constructor
  //* =====================================================================
  explicit server(telnetpp::session &sess) noexcept;

  //* =====================================================================
  /// \brief Send a variables to the remote server.
  //* =====================================================================
  void send(variable const &var);

  //* =====================================================================
  /// \fn on_receive
  /// \brief Register for a signal whenever a list of variables is received
  /// from the remote server.
  //* =====================================================================
  boost::signals2::signal<void(variable const &)> on_receive;

 private:
  //* =====================================================================
  /// \brief Called when a subnegotiation is received while the option is
  /// active.  Override for option-specific functionality.
  //* =====================================================================
  void handle_subnegotiation(telnetpp::bytes data) override;
};

}  // namespace telnetpp::options::msdp
