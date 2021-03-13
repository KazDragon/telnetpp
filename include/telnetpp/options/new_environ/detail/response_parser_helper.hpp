#pragma once

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

enum class parser_state
{
    state_is_or_info,
    state_type,
    state_name,
    state_name_esc,
    state_value,
    state_value_esc,
};

struct parsing_state
{
    parser_state state = parser_state::state_is_or_info;
    response rsp;
};

template <typename Continuation>
void parse_is_or_info(telnetpp::bytes::iterator /*current*/, parsing_state &state, Continuation &&/*cont*/)
{
    state.state = parser_state::state_type;
}

template <typename Continuation>
void parse_type(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&/*cont*/)
{
    state.rsp.type = byte_to_type(*current);
    state.state = parser_state::state_name;
}

template <typename Continuation>
void parse_name(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&cont)
{
    using namespace telnetpp::literals;

    switch (*current)
    {
        case detail::var : // Fall-through
        case detail::uservar :
            cont(state.rsp);
            state.rsp = {};
            state.rsp.type = byte_to_type(*current);
            state.state = parser_state::state_name;
            break;

        case detail::esc :
            state.state = parser_state::state_name_esc;
            break;

        case detail::value :
            state.rsp.value = ""_tb;
            state.state = parser_state::state_value;
            break;

        default :
            state.rsp.name.push_back(*current);
            break;
    }  
}

template <typename Continuation>
void parse_name_esc(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&/*cont*/)
{
    state.rsp.name.push_back(*current);
    state.state = parser_state::state_name;
}

template <typename Continuation>
void parse_value(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&cont)
{
    switch (*current)
    {
        case detail::var : // Fall-through
        case detail::uservar :
            cont(state.rsp);
            state.rsp = {};
            state.rsp.type = byte_to_type(*current);
            state.state = parser_state::state_name;
            break;

        case detail::esc :
            state.state = parser_state::state_value_esc;
            break;

        default :
            state.rsp.value->push_back(*current);
            break;
    }
}

template <typename Continuation>
void parse_value_esc(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&/*cont*/)
{
    state.rsp.value->push_back(*current);
    state.state = parser_state::state_value;
}

template <typename Continuation>
void parse_byte(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&cont)
{
    switch (state.state)
    {
        case parser_state::state_is_or_info :
            parse_is_or_info(current, state, cont);
            break;

        case parser_state::state_type :
            parse_type(current, state, cont);
            break;

        case parser_state::state_name :
            parse_name(current, state, cont);
            break;

        case parser_state::state_name_esc :
            parse_name_esc(current, state, cont);
            break;

        case parser_state::state_value :
            parse_value(current, state, cont);
            break;

        case parser_state::state_value_esc :
            parse_value_esc(current, state, cont);
            break;

        default :
            assert(!"Invalid state in NEW_ENVIRON response parser");
            break;
    }
}

template <typename Continuation>
void parse_end(parsing_state &state, Continuation &&cont)
{
    if (!state.rsp.name.empty())
    {
        cont(state.rsp);
    }
}

}}}}
