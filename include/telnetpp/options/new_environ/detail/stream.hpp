#pragma once

#include "telnetpp/core.hpp"
#include <string>

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

//* =========================================================================
/// \brief Appends escaped text to the content.
//* =========================================================================
void append_escaped(telnetpp::byte_storage &storage, telnetpp::bytes content);

}}}}
