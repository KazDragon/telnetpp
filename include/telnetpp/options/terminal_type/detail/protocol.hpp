#pragma once

#include "telnetpp/core.hpp"

namespace telnetpp { namespace options { namespace terminal_type {
    namespace detail {

static constexpr telnetpp::u8 option = 24;
static constexpr telnetpp::u8 is     = 0;
static constexpr telnetpp::u8 send   = 1;

}}}}
