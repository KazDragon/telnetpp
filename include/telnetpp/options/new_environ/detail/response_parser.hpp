#pragma once

#include "telnetpp/options/new_environ/detail/response_parser_helper.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

template <typename Continuation>
void parse_responses(telnetpp::bytes content, Continuation &&cont)
{
    parsing_state state;

    auto current = content.begin();
    auto end     = content.end();

    while (current != end)
    {
        parse_byte(current++, state, cont);
    }

    parse_end(state, cont);
}

}}}}
