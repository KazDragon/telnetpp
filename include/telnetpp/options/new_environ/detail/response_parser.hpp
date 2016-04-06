#pragma once

#include "telnetpp/options/new_environ/client.hpp"
#include <vector>

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

std::vector<response> parse_responses(telnetpp::u8stream const &stream);

}}}}
