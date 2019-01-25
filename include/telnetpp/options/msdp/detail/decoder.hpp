#pragma once

#include "telnetpp/options/msdp/variable.hpp"
#include <functional>

namespace telnetpp { namespace options { namespace msdp { namespace detail {

//* =========================================================================
/// \brief Decode a byte stream into a list of MSDP variables.
//* =========================================================================
void decode(
    telnetpp::bytes data, 
    std::function<
        void (telnetpp::options::msdp::variable const &)
    > const &cont);

}}}}
