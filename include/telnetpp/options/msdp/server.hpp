#pragma once

#include "telnetpp/options/msdp.hpp"
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
    /// \brief Send a list of variables to the remote client.
    //* =====================================================================
    std::vector<telnetpp::token> send(std::vector<variable> const &variables);

    //* =====================================================================
    /// \fn on_receive
    /// \brief Register for a signal whenever a list of variables is received
    /// from the remote client.
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
