#pragma once

#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options { namespace mccp {

//* =========================================================================
/// \brief A server option responsible for negotiating the server part of the
/// MCCP protocol.
//* =========================================================================
class TELNETPP_EXPORT server : public telnetpp::server_option
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    server();

    //* =====================================================================
    /// \brief Requests that compression begins.
    /// If the option is active, then this sends a sequence that will begin 
    /// compression.  Otherwise, the sequence will be sent as soon as the 
    /// option is activated.
    //* =====================================================================
    //std::vector<telnetpp::token> begin_compression();

    //* =====================================================================
    /// \brief Requests that compression ends.
    /// If the option is active, then this sends a sequence that will end 
    /// compression.  If the option is not active, then this also cancels any
    /// request to begin_compression that would auto-start compression on
    /// activation.
    //* =====================================================================
    //std::vector<telnetpp::token> end_compression();
    
private:
    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active.  Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(
        telnetpp::bytes data,
        continuation const &cont) override;

    //bool compression_requested_;
};

}}}
