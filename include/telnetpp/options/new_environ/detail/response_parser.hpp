#ifndef TELNETPP_OPTIONS_NEW_ENVIRON_DETAIL_RESPONSE_PARSER_HPP_
#define TELNETPP_OPTIONS_NEW_ENVIRON_DETAIL_RESPONSE_PARSER_HPP_

#include "telnetpp/options/new_environ.hpp"
#include <vector>

namespace telnetpp { namespace options { namespace new_environ { 
    namespace detail {

std::vector<response> parse_responses(telnetpp::u8stream const &stream);
        
}}}}

#endif
