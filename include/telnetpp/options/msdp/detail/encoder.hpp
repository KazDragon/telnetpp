#pragma once

#include "telnetpp/options/msdp/variable.hpp"

namespace telnetpp { namespace options { namespace msdp { namespace detail {

//* =========================================================================
/// \brief Encode a list of MSDP variables into a byte stream.
//* =========================================================================
telnetpp::u8stream encode(
    std::vector<telnetpp::options::msdp::variable> const &variables);

}}}}
