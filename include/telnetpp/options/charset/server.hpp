#pragma once

#include "telnetpp/options/charset/detail/protocol.hpp"
#include "telnetpp/server_option.hpp"

#include <optional>
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
    /// \brief Returns the last advertised charsets in offer order.
    //* =====================================================================
    [[nodiscard]] std::vector<telnetpp::byte_storage> const &
    advertised_charsets() const noexcept
    {
        return advertised_charsets_;
    }

    //* =====================================================================
    /// \brief Returns the negotiated charset, if one has been selected.
    //* =====================================================================
    [[nodiscard]] std::optional<telnetpp::byte_storage> const &
    negotiated_charset() const noexcept
    {
        return negotiated_charset_;
    }

private:
    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active. Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(telnetpp::bytes data) override;

    std::vector<telnetpp::byte_storage> advertised_charsets_;
    std::optional<telnetpp::byte_storage> negotiated_charset_;
};

}  // namespace telnetpp::options::charset
