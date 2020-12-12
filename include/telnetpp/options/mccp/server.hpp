#pragma once

#include "telnetpp/options/mccp/detail/protocol.hpp"
#include "telnetpp/options/basic_server.hpp"

namespace telnetpp { namespace options { namespace mccp {

class codec;

//* =========================================================================
/// \brief A server option responsible for negotiating the server part of the
/// MCCP protocol.
//* =========================================================================
class TELNETPP_EXPORT server 
  : public telnetpp::options::basic_server<detail::option>
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    server(codec &cdc);

    //* =====================================================================
    /// \brief Requests that compression begins.
    /// If the option is active, then this sends a sequence that will begin 
    /// compression.  Otherwise, the sequence will be sent as soon as the 
    /// option is activated.
    //* =====================================================================
    void start_compression(continuation const &cont);

    //* =====================================================================
    /// \brief Requests that compression ends.
    /// If the option is active, then this sends a sequence that will end 
    /// compression.  If the option is not active, then this also cancels any
    /// request to begin_compression that would auto-start compression on
    /// activation.
    //* =====================================================================
    void finish_compression(continuation const &cont);
    
private:
    codec &codec_;
    bool   compression_active_;
};

}}}
