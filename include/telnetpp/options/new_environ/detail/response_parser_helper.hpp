#pragma once

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

enum class parser_state
{
    is_or_info,
    type,
    name,
    name_esc,
    value,
    value_esc,
};

struct parsing_state
{
    parser_state state = parser_state::is_or_info;
    response rsp;
};

template <typename Continuation>
void parse_is_or_info(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&cont)
{
    state.state = parser_state::type;
}

template <typename Continuation>
void parse_type(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&cont)
{
    state.rsp.type = byte_to_type(*current);
    state.state = parser_state::name;
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
            state.state = parser_state::name;
            break;

        case detail::esc :
            state.state = parser_state::name_esc;
            break;

        case detail::value :
            state.rsp.value = ""_tb;
            state.state = parser_state::value;
            break;

        default :
            state.rsp.name.push_back(*current);
            break;
    }  
}

template <typename Continuation>
void parse_name_esc(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&cont)
{
    state.rsp.name.push_back(*current);
    state.state = parser_state::name;
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
            state.state = parser_state::name;
            break;

        case detail::esc :
            state.state = parser_state::value_esc;
            break;

        default :
            state.rsp.value->push_back(*current);
            break;
    }
}

template <typename Continuation>
void parse_value_esc(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&cont)
{
    state.rsp.value->push_back(*current);
    state.state = parser_state::value;
}

template <typename Continuation>
void parse_byte(telnetpp::bytes::iterator current, parsing_state &state, Continuation &&cont)
{
    switch (state.state)
    {
        case parser_state::is_or_info :
            parse_is_or_info(current, state, cont);
            break;

        case parser_state::type :
            parse_type(current, state, cont);
            break;

        case parser_state::name :
            parse_name(current, state, cont);
            break;

        case parser_state::name_esc :
            parse_name_esc(current, state, cont);
            break;

        case parser_state::value :
            parse_value(current, state, cont);
            break;

        case parser_state::value_esc :
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
