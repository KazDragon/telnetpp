#pragma once

#include "telnetpp/options/charset/detail/protocol.hpp"
#include "telnetpp/server_option.hpp"

#include <boost/signals2.hpp>

#include <vector>

namespace telnetpp::options::charset {

//* =========================================================================
/// \brief An implementation of the server side of the Telnet CHARSET option.
//* =========================================================================
class TELNETPP_EXPORT server final : public telnetpp::server_option
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    explicit server(telnetpp::session &sess) noexcept;

    //* =====================================================================
    /// \brief Requests that the remote end advertise its supported charsets.
    //* =====================================================================
    void request_charsets();

    //* =====================================================================
    /// \brief Selects the negotiated charset.
    //* =====================================================================
    void select_charset(telnetpp::bytes charset);

    boost::signals2::signal<void(
        std::vector<telnetpp::byte_storage> const &)>
        on_charsets_advertised;  // NOLINT

    boost::signals2::signal<void(telnetpp::bytes)>
        on_charset_selected;  // NOLINT

private:
    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active. Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(telnetpp::bytes data) override;
};

}  // namespace telnetpp::options::charset
