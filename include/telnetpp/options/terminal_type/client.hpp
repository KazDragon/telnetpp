#pragma once

#include "telnetpp/client_option.hpp"
#include "telnetpp/options/terminal_type/detail/protocol.hpp"
#include <boost/signals2.hpp>

namespace telnetpp { namespace options { namespace terminal_type {

//* =========================================================================
/// \brief An implementation of the client side of the Telnet Terminal
/// Type option.
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::client_option {
public:
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    client();

    //* =====================================================================
    /// \brief Requests that the remote end send its terminal type.
    //* =====================================================================
    template <typename Continuation>
    void request_terminal_type(Continuation &&send)
    {
        static constexpr telnetpp::byte const request_content[] = {
            detail::send
        };

        send(telnetpp::subnegotiation{option_code(), request_content});
    }

    boost::signals2::signal<
        void (telnetpp::bytes, continuation const &)
    > on_terminal_type;

private:
    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active.  Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(
        telnetpp::bytes data,
        continuation const &cont) override;
};

}}}
