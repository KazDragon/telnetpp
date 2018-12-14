#pragma once

#include "telnetpp/core.hpp"
#include "telnetpp/element.hpp"
#include <boost/container/small_vector.hpp>
#include <algorithm>
#include <cassert>

namespace telnetpp { namespace detail {

//* =========================================================================
/// \exclude
//* =========================================================================
enum class parsing_state
{
    idle,
    iac,
    negotiation,
    subnegotiation,
    subnegotiation_content,
    subnegotiation_content_iac
};

//* =========================================================================
/// \exclude
//* =========================================================================
struct parser_state
{
    constexpr parser_state(bytes::iterator begin)
      : last_parsed(begin),
        token_begin(begin),
        token_end(begin)
    {
    }

    parsing_state   current_state = parsing_state::idle;
    bytes::iterator last_parsed;
    bytes::iterator token_begin;
    bytes::iterator token_end;
    byte            id    = 0;
    bool            has_escape = false;
};

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void emit_bytes(parser_state &state, Continuation &&cont)
{
    if (state.token_begin != state.token_end)
    {
        cont(bytes{state.token_begin, state.token_end});
    }

    state.last_parsed = state.token_begin = state.token_end;
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void emit_command(parser_state &state, Continuation &&cont)
{
    cont(telnetpp::command{*state.token_end++});
    state.last_parsed = state.token_begin = state.token_end;
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void emit_negotiation(parser_state &state, Continuation &&cont)
{
    cont(telnetpp::negotiation{*state.token_begin, *state.token_end});
    state.last_parsed = state.token_begin = ++state.token_end;
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void emit_subnegotiation(parser_state &state, Continuation &&cont)
{
    telnetpp::bytes content{state.token_begin, state.token_end};

    if (state.has_escape)
    {
        // In the case that the subnegotiation has any escaped characters,
        // (e.g. a value of 0xFF is escaped as 0xFF, 0xFF), then we must 
        // present a new span with those escaped characters omitted.  Since
        // most subnegotiations are small, we use a boost::small_vector here
        // to prevent any costly memory allocations.
        constexpr std::size_t small_subnegotiation = 16;
        boost::container::small_vector<
            telnetpp::byte, 
            small_subnegotiation
        > small_content;
        small_content.reserve(content.size() - 1);

        bool escape = false;
        for (auto const &byte : content)
        {
            if (escape)
            {
                small_content.push_back(byte);
                escape = false;
            }
            else if (byte == telnetpp::iac)
            {
                escape = true;
            }
            else
            {
                small_content.push_back(byte);
            }
        }

        cont(telnetpp::subnegotiation{state.id, small_content});
    }
    else
    {
        cont(telnetpp::subnegotiation{state.id, content});
    }

    state.has_escape = false;
    std::advance(state.token_end, 2);
    state.last_parsed = state.token_begin = state.token_end;
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void parse_idle(
    parser_state &state, byte data, Continuation &&cont)
{
    switch (data)
    {
        case telnetpp::iac:
            emit_bytes(state, cont);
            ++state.token_end;
            state.current_state = parsing_state::iac;
            break;

        default:
            ++state.token_end;
            break;
    }
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void parse_iac(
    parser_state &state, byte data, Continuation &&cont)
{
    switch (data)
    {
        case telnetpp::iac:
            ++state.token_begin;
            ++state.token_end;
            state.current_state = parsing_state::idle;
            break;
            
        case telnetpp::sb:
            ++state.token_end;
            state.current_state = parsing_state::subnegotiation;
            break;

        case telnetpp::will: // fall-through
        case telnetpp::wont: // fall-through
        case telnetpp::do_:  // fall-through
        case telnetpp::dont:
            ++state.token_begin;
            ++state.token_end;
            state.current_state = parsing_state::negotiation;
            break;

        default :
            emit_command(state, cont);
            state.current_state = parsing_state::idle;
            break;
    }
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void parse_negotiation(
    parser_state &state, byte data, Continuation &&cont)
{
    emit_negotiation(state, cont);
    state.current_state = parsing_state::idle;
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void parse_subnegotiation(
    parser_state &state, byte data, Continuation &&cont)
{
    state.id = data;
    state.token_begin = ++state.token_end;
    state.current_state = parsing_state::subnegotiation_content;
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void parse_subnegotiation_content(
    parser_state &state, byte data, Continuation &&cont)
{
    if (data == telnetpp::iac)
    {
        state.current_state = parsing_state::subnegotiation_content_iac;
    }
    else
    {
        ++state.token_end;
    }
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void parse_subnegotiation_content_iac(
    parser_state &state, byte data, Continuation &&cont)
{
    if (data == telnetpp::se)
    {
        emit_subnegotiation(state, cont);
        state.current_state = parsing_state::idle;
    }
    else
    {
        state.has_escape = true;
        ++state.token_end;
        ++state.token_end;
        state.current_state = parsing_state::subnegotiation_content;
    }
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <typename Continuation>
static constexpr void parse_byte(
    parser_state &state, bytes::iterator data, Continuation &&cont)
{
    switch (state.current_state)
    {
        case parsing_state::idle:
            parse_idle(state, *data, cont);
            break;

        case parsing_state::iac:
            parse_iac(state, *data, cont);
            break;

        case parsing_state::negotiation:
            parse_negotiation(state, *data, cont);
            break;

        case parsing_state::subnegotiation:
            parse_subnegotiation(state, *data, cont);
            break;

        case parsing_state::subnegotiation_content:
            parse_subnegotiation_content(state, *data, cont);
            break;

        case parsing_state::subnegotiation_content_iac:
            parse_subnegotiation_content_iac(state, *data, cont);
            break;

        default :
            assert(!"Telnet parser in invalid state");
            break;
    }
}

}}

