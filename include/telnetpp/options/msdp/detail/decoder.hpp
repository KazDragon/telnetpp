#ifndef TELNETPP_OPTIONS_MSDP_DETAIL_DECODER_HPP_
#define TELNETPP_OPTIONS_MSDP_DETAIL_DECODER_HPP_

#include "telnetpp/options/msdp.hpp"

namespace telnetpp { namespace options { namespace msdp { namespace detail {

std::vector<telnetpp::options::msdp::variable> decode(
    telnetpp::u8stream const &stream);

}}}}

#endif
