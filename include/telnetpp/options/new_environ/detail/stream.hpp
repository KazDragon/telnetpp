#ifndef TELNETPP_OPTIONS_NEW_ENVIRON_DETAIL_STREAM_HPP_
#define TELNETPP_OPTIONS_NEW_ENVIRON_DETAIL_STREAM_HPP_

#include "telnetpp/core.hpp"
#include <string>

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

void append_escaped(
    telnetpp::u8stream &stream, std::string const &data);
    
}}}}

#endif
