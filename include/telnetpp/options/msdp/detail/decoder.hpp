#pragma once

#include "telnetpp/options/msdp.hpp"

namespace telnetpp { namespace options { namespace msdp { namespace detail {

//* =========================================================================
/// \brief Decode a byte stream into a list of MSDP variables.
//* =========================================================================
std::vector<telnetpp::options::msdp::variable> decode(
    telnetpp::u8stream const &stream);

}}}}
