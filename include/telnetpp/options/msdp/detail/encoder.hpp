#ifndef TELNETPP_OPTIONS_MSDP_DETAIL_ENCODER_HPP_
#define TELNETPP_OPTIONS_MSDP_DETAIL_ENCODER_HPP_

#include "telnetpp/options/msdp.hpp"
#include <vector>

namespace telnetpp { namespace options { namespace msdp { namespace detail {

telnetpp::u8stream encode(
    std::vector<telnetpp::options::msdp::variable> const &variables);
    
}}}}

#endif
