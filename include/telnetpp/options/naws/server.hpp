#pragma once

#include "telnetpp/server_option.hpp"
#include <boost/optional.hpp>
#include <utility>

namespace telnetpp { namespace options { namespace naws {

//* =========================================================================
/// \brief An implementation of the server side of the Telnet NAWS option.
//* =========================================================================
class TELNETPP_EXPORT server : public telnetpp::server_option {
public :
    using window_dimension = std::uint16_t;

    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    server();

    //* =====================================================================
    /// \brief Sets the window size that this option will report.
    //* =====================================================================
    std::vector<telnetpp::token> set_window_size(
        window_dimension width,
        window_dimension height);

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    std::vector<telnetpp::token> handle_subnegotiation(
        byte_stream const &content) override;

    //* =====================================================================
    /// \brief Report the window size.
    //* =====================================================================
    std::vector<telnetpp::token> report_window_size();

    boost::optional<std::pair<
        window_dimension, window_dimension
    >> window_size_;
};

}}}
