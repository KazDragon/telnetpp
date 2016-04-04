#pragma once

#include "telnetpp/element.hpp"

namespace telnetpp { namespace detail {

void generate_helper(u8stream &result, telnetpp::element const &element);

}}
