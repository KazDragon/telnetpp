#pragma once

#include "telnetpp/core.hpp"
#include <string>

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

void append_escaped(
    telnetpp::u8stream &stream, std::string const &data);

}}}}
