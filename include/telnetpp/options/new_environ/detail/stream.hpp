#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"
#include "telnetpp/options/new_environ/protocol.hpp"

namespace telnetpp::options::new_environ::detail {

//* =========================================================================
/// \brief Appends escaped text to the content.
//* =========================================================================
TELNETPP_EXPORT
void append_escaped(telnetpp::byte_storage &storage, telnetpp::bytes content);

//* =========================================================================
/// \brief Returns a byte interpreted as a variable type.
//* =========================================================================
constexpr telnetpp::options::new_environ::variable_type byte_to_type(byte by)
{
    return by == detail::var ? variable_type::var : variable_type::uservar;
}

}  // namespace telnetpp::options::new_environ::detail
