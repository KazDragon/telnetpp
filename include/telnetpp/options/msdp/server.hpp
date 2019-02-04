#pragma once

#include "telnetpp/options/msdp/variable.hpp"
#include "telnetpp/options/msdp/detail/encoder.hpp"
#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options { namespace msdp {

//* =========================================================================
/// \brief An implementation of the server side of an MSDP Telnet option.
//* =========================================================================
class TELNETPP_EXPORT server : public telnetpp::server_option
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    server();

    //* =====================================================================
    /// \brief Send a variables to the remote server.
    //* =====================================================================
    template <typename Continuation>
    void send(variable const &var, Continuation &&cont)
    {
        detail::encode(
            var,
            [&](telnetpp::bytes data)
            {
                auto const elem = telnetpp::element{telnetpp::subnegotiation{
                    option_code(),
                    data
                }};
                
                telnetpp::elements elems = { elem };
                cont(elems);
            });
    }

    //* =====================================================================
    /// \fn on_receive
    /// \brief Register for a signal whenever a list of variables is received
    /// from the remote server.
    //* =====================================================================
    boost::signals2::signal<
        void (variable const &, continuation const &)
    > on_receive;

private :
    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active.  Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(
        telnetpp::bytes data,
        continuation const &cont) override;
};

}}}
