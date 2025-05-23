#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/options/new_environ/detail/request_parser_helper.hpp"

namespace telnetpp::options::new_environ::detail {

template <typename Continuation>
void for_each_request(telnetpp::bytes requests, Continuation &&cont)
{
    request_parsing_state state;

    auto current = requests.begin();
    auto end = requests.end();

    while (current != end)
    {
        parse_request_byte(state, current++, cont);
    }

    parse_request_end(state, cont);
}

}  // namespace telnetpp::options::new_environ::detail
