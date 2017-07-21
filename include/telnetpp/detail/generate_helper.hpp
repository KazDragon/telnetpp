#pragma once

#include "telnetpp/element.hpp"

namespace telnetpp { namespace detail {

void generate_helper(byte_stream &result, telnetpp::element const &element);

}}
