#pragma once

#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options {

//* =========================================================================
/// \brief A class template that generates basic classes that handle no
///        subnegotiations; they merely exist for enabling and disabling
///        the option.
//* =========================================================================
template <telnetpp::u8 Option>
class subnegotiationless_server : public telnetpp::server_option
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    subnegotiationless_server()
      : server_option(Option)
    {
    }
};

}}
