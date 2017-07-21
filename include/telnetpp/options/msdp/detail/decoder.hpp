#pragma once

#include "telnetpp/options/msdp/variable.hpp"

namespace telnetpp { namespace options { namespace msdp { namespace detail {

//* =========================================================================
/// \brief Decode a byte stream into a list of MSDP variables.
//* =========================================================================
std::vector<telnetpp::options::msdp::variable> decode(
    telnetpp::byte_stream const &stream);

}}}}
