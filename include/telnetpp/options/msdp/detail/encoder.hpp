#ifndef TELNETPP_OPTIONS_MSDP_DETAIL_ENCODER_HPP_
#define TELNETPP_OPTIONS_MSDP_DETAIL_ENCODER_HPP_

#include "telnetpp/options/msdp.hpp"

namespace telnetpp { namespace options { namespace msdp { namespace detail {

//* =========================================================================
/// \brief Encode a list of MSDP variables into a byte stream.
//* =========================================================================
telnetpp::u8stream encode(
    std::vector<telnetpp::options::msdp::variable> const &variables);

}}}}

#endif
