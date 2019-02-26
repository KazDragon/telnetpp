#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/new_environ/protocol.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

//* =========================================================================
/// \brief Appends escaped text to the content.
//* =========================================================================
TELNETPP_EXPORT
void append_escaped(telnetpp::byte_storage &storage, telnetpp::bytes content);

//* =========================================================================
/// \brief Returns a byte interpreted as a variable type.
//* =========================================================================
constexpr telnetpp::options::new_environ::variable_type byte_to_type(byte value)
{
    return value == detail::var ? variable_type::var : variable_type::uservar;
}

}}}}
