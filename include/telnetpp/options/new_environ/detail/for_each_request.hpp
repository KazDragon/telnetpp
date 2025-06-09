#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/new_environ/detail/request_parser_helper.hpp"

#include <algorithm>

namespace telnetpp::options::new_environ::detail {

template <typename Continuation>
constexpr void for_each_request(telnetpp::bytes requests, Continuation &&cont)
{
    request_parsing_state state;

    std::ranges::for_each(requests, [&](telnetpp::byte byte) {
        parse_request_byte(state, byte, cont);
    });
    parse_request_end(state, cont);
}

}  // namespace telnetpp::options::new_environ::detail
