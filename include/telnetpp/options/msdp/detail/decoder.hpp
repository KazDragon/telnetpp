#pragma once

#include "telnetpp/options/msdp/variable.hpp"
#include <functional>

namespace telnetpp::options::msdp::detail {

//* =========================================================================
/// \brief Decode a byte stream into a list of MSDP variables.
//* =========================================================================
TELNETPP_EXPORT
void decode(
    telnetpp::bytes data,
    std::function<void(telnetpp::options::msdp::variable const &)> const &cont);

}  // namespace telnetpp::options::msdp::detail
