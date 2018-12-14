#pragma once

#include "telnetpp/detail/parse_helper.hpp"
#include "telnetpp/core.hpp"

namespace telnetpp {

//* =========================================================================
/// \brief Parse a series of bytes into a stream of Telnet tokens
/// \param begin a reference to the start of the series.  This will be
///        modified to be an iterator to the position after the last
///        completely-parsed token.
/// \param end an iterator to one past the end of the series.
/// \returns a collection of completely parsed tokens.
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
