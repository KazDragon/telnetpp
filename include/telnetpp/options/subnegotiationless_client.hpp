#pragma once

#include "telnetpp/client_option.hpp"

namespace telnetpp { namespace options {

//* =========================================================================
/// \brief A class template that generates basic classes that handle no
///        subnegotiations; they merely exist for enabling and disabling
///        the option.
//* =========================================================================
template <telnetpp::u8 Option>
class subnegotiationless_client : public telnetpp::client_option
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    subnegotiationless_client()
      : client_option(Option)
    {
    }
};

}}
