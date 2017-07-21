#pragma once

#include "telnetpp/options/new_environ/client.hpp"
#include <vector>

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

std::vector<request> parse_requests(telnetpp::byte_stream const &stream);

}}}}
