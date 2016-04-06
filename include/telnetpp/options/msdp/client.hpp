#pragma once

#include "telnetpp/options/msdp/variable.hpp"
#include "telnetpp/client_option.hpp"

namespace telnetpp { namespace options { namespace msdp {

//* =========================================================================
/// \brief An implementation of the client side of an MSDP Telnet option.
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::client_option
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    client();

    //* =====================================================================
    /// \brief Send a list of variables to the remote server.
    //* =====================================================================
    std::vector<telnetpp::token> send(std::vector<variable> const &variables);

    //* =====================================================================
    /// \fn on_receive
    /// \brief Register for a signal whenever a list of variables is received
    /// from the remote server.
    //* =====================================================================
    boost::signals2::signal<
        std::vector<telnetpp::token> (std::vector<variable> const &),
        telnetpp::token_combiner
    > on_receive;

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    std::vector<telnetpp::token> handle_subnegotiation(
        u8stream const &content) override;
};

}}}
