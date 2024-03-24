#pragma once

#include "telnetpp/options/basic_server.hpp"
#include "telnetpp/options/naws/detail/protocol.hpp"

#include <optional>
#include <utility>

namespace telnetpp::options::naws {

//* =========================================================================
/// \brief An implementation of the server side of the Telnet NAWS option.
//* =========================================================================
class TELNETPP_EXPORT server
  : public telnetpp::options::basic_server<detail::option>
{
public:
    using window_dimension = std::uint16_t;

    //* =====================================================================
    /// Constructor
    //* =====================================================================
    explicit server(telnetpp::session &sess);

    //* =====================================================================
    /// \brief Sets the window size that this option will report.
    //* =====================================================================
    void set_window_size(window_dimension width, window_dimension height);

private:
    //* =====================================================================
    /// \brief Reports the window size
    //* =====================================================================
    void report_window_size();

    std::optional<std::pair<window_dimension, window_dimension>> window_size_;
};

}  // namespace telnetpp::options::naws
