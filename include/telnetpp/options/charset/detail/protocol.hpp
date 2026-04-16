#pragma once

#include "telnetpp/core.hpp"

namespace telnetpp::options::charset::detail {

constexpr telnetpp::option_type option = 42;

constexpr telnetpp::byte request = 0x01;
constexpr telnetpp::byte accepted = 0x02;

}  // namespace telnetpp::options::charset::detail
