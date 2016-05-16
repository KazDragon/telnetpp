#pragma once

#include "telnetpp/client_option.hpp"

namespace telnetpp { namespace options { namespace mccp {

//* =========================================================================
/// \brief A server option responsible for negotiating the client part of the
/// MCCP protocol.
//* =========================================================================
class TELNETPP_EXPORT client : public telnetpp::client_option
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    client();

private :
    //* =====================================================================
    /// \brief Handle a negotiation that has been received in the active
    /// state.
    //* =====================================================================
    virtual std::vector<telnetpp::token> handle_subnegotiation(
        telnetpp::u8stream const &content);

};

}}}
