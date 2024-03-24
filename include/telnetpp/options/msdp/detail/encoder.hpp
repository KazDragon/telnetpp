#pragma once

#include "telnetpp/options/msdp/variable.hpp"

namespace telnetpp::options::msdp::detail {

//* =========================================================================
/// \brief Encode a list of MSDP variables into a sequence of bytes.
//* =========================================================================
TELNETPP_EXPORT
void encode_variable(
    telnetpp::options::msdp::variable const &var,
    telnetpp::byte_storage &storage);

//* =========================================================================
/// \brief Encode a list of MSDP variables into a sequence of bytes.
//* =========================================================================
template <typename Continuation>
void encode(telnetpp::options::msdp::variable const &vbl, Continuation &&cont)
{
    telnetpp::byte_storage storage;
    encode_variable(vbl, storage);
    cont(storage);
}

}  // namespace telnetpp::options::msdp::detail
