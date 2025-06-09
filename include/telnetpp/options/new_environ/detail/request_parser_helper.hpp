#pragma once

#include "telnetpp/options/new_environ/detail/stream.hpp"
#include "telnetpp/options/new_environ/protocol.hpp"

#include <cassert>

namespace telnetpp::options::new_environ::detail {

enum class request_parser_state : std::uint8_t
{
    state_send,
    state_type,
    state_name,
    state_name_esc,
};

struct request_parsing_state
{
    request_parser_state state_ = request_parser_state::state_send;
    request req_;
};

// ==========================================================================
// PARSE_REQUEST_SEND
// ==========================================================================
template <typename Continuation>
constexpr void parse_request_send(
    request_parsing_state &state,
    telnetpp::byte /*current*/,
    Continuation && /*cont*/)
{
    state.state_ = request_parser_state::state_type;
}

// ==========================================================================
// PARSE_REQUEST_TYPE
// ==========================================================================
template <typename Continuation>
constexpr void parse_request_type(
    request_parsing_state &state,
    telnetpp::byte current,
    Continuation && /*cont*/)
{
    state.req_.type = byte_to_type(current);
    state.state_ = request_parser_state::state_name;
}

// ==========================================================================
// PARSE_REQUEST_NAME
// ==========================================================================
template <typename Continuation>
constexpr void parse_request_name(
    request_parsing_state &state, telnetpp::byte current, Continuation &&cont)
{
    switch (current)
    {
        case detail::esc:
            state.state_ = request_parser_state::state_name_esc;
            break;

        case detail::var:  // Fall-through
        case detail::uservar:
            cont(state.req_);
            state.req_ = {};
            state.req_.type = byte_to_type(current);
            break;

        default:
            state.req_.name.push_back(current);
            break;
    }
}

// ==========================================================================
// PARSE_REQUEST_NAME_ESC
// ==========================================================================
template <typename Continuation>
constexpr void parse_request_name_esc(
    request_parsing_state &state,
    telnetpp::byte current,
    Continuation && /*cont*/)
{
    state.req_.name.push_back(current);
    state.state_ = request_parser_state::state_name;
}

// ==========================================================================
// PARSE_REQUEST_BYTE
// ==========================================================================
template <typename Continuation>
constexpr void parse_request_byte(
    request_parsing_state &state, telnetpp::byte current, Continuation &&cont)
{
    switch (state.state_)
    {
        case request_parser_state::state_send:
            parse_request_send(state, current, cont);
            break;

        case request_parser_state::state_type:
            parse_request_type(state, current, cont);
            break;

        case request_parser_state::state_name:
            parse_request_name(state, current, cont);
            break;

        case request_parser_state::state_name_esc:
            parse_request_name_esc(state, current, cont);
            break;

        default:
            assert(!"Invalid state in request parser");
            break;
    }
}

// ==========================================================================
// PARSE_REQUEST_BYTE
// ==========================================================================
template <typename Continuation>
constexpr void parse_request_end(
    request_parsing_state &state, Continuation &&cont)
{
    if (!state.req_.name.empty())
    {
        cont(state.req_);
    }
}

}  // namespace telnetpp::options::new_environ::detail
