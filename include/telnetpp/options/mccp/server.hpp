#pragma once

#include "telnetpp/options/basic_server.hpp"
#include "telnetpp/options/mccp/detail/protocol.hpp"

namespace telnetpp::options::mccp {

class codec;

//* =========================================================================
/// \brief A server option responsible for negotiating the server part of the
/// MCCP protocol.
//* =========================================================================
class TELNETPP_EXPORT server
  : public telnetpp::options::basic_server<detail::option>
{
 public:
  //* =====================================================================
  /// \brief Constructor
  //* =====================================================================
  explicit server(telnetpp::session &sess, codec &cdc);

  //* =====================================================================
  /// \brief Requests that compression begins.
  /// If the option is active, then this sends a sequence that will begin
  /// compression.  Otherwise, the sequence will be sent as soon as the
  /// option is activated.
  //* =====================================================================
  void start_compression();

  //* =====================================================================
  /// \brief Requests that compression ends.
  /// If the option is active, then this sends a sequence that will end
  /// compression.  If the option is not active, then this also cancels any
  /// request to begin_compression that would auto-start compression on
  /// activation.
  //* =====================================================================
  void finish_compression();

 private:
  codec &codec_;
  bool compression_active_;
};

}  // namespace telnetpp::options::mccp
