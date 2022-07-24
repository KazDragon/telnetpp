#pragma once

#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options {

//* =========================================================================
/// \brief A class template that generates basic classes that handle no
///        subnegotiations; they merely exist for enabling and disabling
///        the option.
//* =========================================================================
template <option_type Option>
class TELNETPP_EXPORT basic_server : public telnetpp::server_option
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr explicit basic_server(telnetpp::session &sess) noexcept
      : server_option(sess, Option)
    {
    }

private:
    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active.  Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(telnetpp::bytes /*content*/) override
    {
    }
};

}}
