#pragma once

#include "telnetpp/client_option.hpp"
#include <boost/signals2.hpp>

namespace telnetpp { namespace options { namespace naws {

//* =========================================================================
/// \brief An implementation of the client side of the Telnet NAWS option.
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::client_option {
public:
    using window_dimension = std::uint16_t;

    //* =====================================================================
    /// Constructor
    //* =====================================================================
    explicit client(telnetpp::session& sess) noexcept;

    boost::signals2::signal<
        void (window_dimension, window_dimension)
    > on_window_size_changed;

private:
    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active.  Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(telnetpp::bytes content) override;
};

}}}
