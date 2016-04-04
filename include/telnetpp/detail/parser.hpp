#pragma once

#include "telnetpp/command.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/protocol.hpp"
#include "telnetpp/subnegotiation.hpp"
#include "telnetpp/element.hpp"
#include "telnetpp/detail/parse_helper.hpp"
#include <vector>

namespace telnetpp { namespace detail {

//* =========================================================================
/// \brief Parse a series of bytes into a stream of Telnet tokens
/// \param begin a reference to the start of the series.  This will be
///        modified to be an iterator to the position after the last
///        completely-parsed token.
/// \param end an iterator to one past the end of the series.
/// \returns a collection of completely parsed tokens.
//* =========================================================================
template <class InputIterator1, class InputIterator2>
std::vector<element> parse(InputIterator1 &begin, InputIterator2 end)
{
    detail::parse_temps temps;

    auto position = begin;
    while (position != end)
    {
        detail::parse_helper(temps, *position);
        ++position;

        // If the current state is idle, then it means that a token has been
        // completely parsed, and therefore we can move the begin iterator
        // indicating that a range has been consumed.
        if (temps.state == detail::parse_state::idle)
        {
            begin = position;
        }
    }

    return temps.elements;
}

}}
