#pragma once

#include "telnetpp/options/new_environ.hpp"
#include <vector>

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

std::vector<request> parse_requests(telnetpp::u8stream const &stream);

}}}}
