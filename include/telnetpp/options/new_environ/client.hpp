#pragma once

#include "telnetpp/client_option.hpp"
#include "telnetpp/options/new_environ/protocol.hpp"

namespace telnetpp::options::new_environ {

//* =========================================================================
/// \brief An implementation of the client side of the Telnet New-Environ
/// option.
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::client_option
{
 public:
  //* =====================================================================
  /// CONSTRUCTOR
  //* =====================================================================
  explicit client(telnetpp::session &sess) noexcept;

  //* =====================================================================
  /// \brief Requests that a particular set of environment variables be
  /// transmitted by the client.
  //* =====================================================================
  void request_variables(requests const &reqs);

  //* =====================================================================
  /// \brief Signal called whenever an environment variable is updated.
  /// \param rsp the response the response from the remote
  /// \param cont a continuation to pass any Telnet response that may
  ///        occur as a result of receiving this response.
  //* =====================================================================
  boost::signals2::signal<void(response const &rsp)> on_variable_changed;

 private:
  //* =====================================================================
  /// \brief Called when a subnegotiation is received while the option is
  /// active.  Override for option-specific functionality.
  //* =====================================================================
  void handle_subnegotiation(telnetpp::bytes data) override;
};

}  // namespace telnetpp::options::new_environ
