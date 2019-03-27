#pragma once

#include "telnetpp/detail/parse_helper.hpp"
#include "telnetpp/core.hpp"

namespace telnetpp {

//* =========================================================================
/// \brief Parse a series of bytes into a stream of Telnet tokens
/// \param bytes a span representing the bytes to parse.
/// \param cont a continuation that will be called for each complete
///        token that is parsed out of the data.
/// \returns the remaining bytes that did not form a complete token.
//* =========================================================================
template <typename Continuation>
constexpr bytes parse(bytes data, Continuation &&cont)
{
    detail::parser_state state{data.begin()};

    auto current = data.begin();
    auto end     = data.end();

    while(current != end)
    {
        parse_byte(state, current++, cont);
    }

    // If any data has been parsed, and we are in the idle state, then
    // it means that we should emit a non-protocol token.
    if (state.current_state == detail::parsing_state::idle)
    {
        emit_bytes(state, cont);
    }

    return data.subspan(std::distance(data.begin(), state.last_parsed));
}

}
