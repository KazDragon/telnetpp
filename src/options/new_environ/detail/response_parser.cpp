#include "telnetpp/options/new_environ/detail/response_parser.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

namespace {

enum class parse_state
{
    is_or_info,
    type,
    name,
    name_esc,
    value,
    value_esc,
};

// ==========================================================================
// BYTE_TO_TYPE
// ==========================================================================
static telnetpp::options::new_environ::variable_type byte_to_type(byte value)
{
    return value == detail::var ? variable_type::var : variable_type::uservar;
}

// ==========================================================================
// PARSE_VALUE_ESC
// ==========================================================================
boost::optional<response> parse_value_esc(
    parse_state &state, response &temp, byte data)
{
    temp.value->push_back(data);
    state = parse_state::value;

    return {};
}

// ==========================================================================
// PARSE_VALUE
// ==========================================================================
boost::optional<response> parse_value(
    parse_state &state, response &temp, byte data)
{
    boost::optional<response> resp;

    switch (data)
    {
        case detail::esc :
            state = parse_state::value_esc;
            break;

        case detail::var : // Fall-through
        case detail::uservar :
            resp = temp;

            temp.type = byte_to_type(data);
            temp.name = "";
            temp.value = boost::none;
            state = parse_state::name;
            break;

        default :
            temp.value->push_back(data);
            break;
    }

    return resp;
}

// ==========================================================================
// PARSE_NAME_ESC
// ==========================================================================
boost::optional<response> parse_name_esc(
    parse_state &state, response &temp, byte data)
{
    temp.name.push_back(char(data));
    state = parse_state::name;

    return {};
}

// ==========================================================================
// PARSE_NAME
// ==========================================================================
boost::optional<response> parse_name(
    parse_state &state, response &temp, byte data)
{
    boost::optional<response> resp;

    switch (data)
    {
        case detail::esc :
            state = parse_state::name_esc;
            break;

        case detail::var : // Fall-through
        case detail::uservar :
            resp = temp;

            temp.type = byte_to_type(data);
            temp.name = "";
            temp.value = boost::none;
            state = parse_state::name;
            break;

        case detail::value :
            temp.value = "";
            state = parse_state::value;
            break;

        default :
            temp.name.push_back(char(data));
            break;
    }

    return resp;
}

// ==========================================================================
// PARSE_TYPE
// ==========================================================================
boost::optional<response> parse_type(
    parse_state &state, response &temp, byte data)
{
    temp.type = byte_to_type(data);
    state = parse_state::name;
    return {};
}

// ==========================================================================
// PARSE_IS_OR_INFO
// ==========================================================================
boost::optional<response> parse_is_or_info(
    parse_state &state, response &temp, byte data)
{
    state = parse_state::type;
    return {};
}

// ==========================================================================
// PARSE_DATA
// ==========================================================================
boost::optional<response> parse_data(
    parse_state &state, response &temp, byte data)
{
    boost::optional<response> resp;

    switch (state)
    {
        case parse_state::is_or_info :
            resp = parse_is_or_info(state, temp, data);
            break;

        case parse_state::type :
            resp = parse_type(state, temp, data);
            break;

        case parse_state::name :
            resp = parse_name(state, temp, data);
            break;

        case parse_state::name_esc :
            resp = parse_name_esc(state, temp, data);
            break;

        case parse_state::value :
            resp = parse_value(state, temp, data);
            break;

        case parse_state::value_esc :
            resp = parse_value_esc(state, temp, data);
            break;

        default :
            return {};
            break;
    }

    return resp;
}

}

// ==========================================================================
// PARSE_RESPONSES
// ==========================================================================
std::vector<response> parse_responses(telnetpp::byte_stream const &stream)
{
    std::vector<response> responses;
    parse_state state = parse_state::is_or_info;

    response temp;
    boost::optional<response> resp;

    for (auto const &data : stream)
    {
        resp = parse_data(state, temp, data);

        if (resp)
        {
            responses.push_back(*resp);
        }
    }

    if (!temp.name.empty())
    {
        responses.push_back(temp);
    }

    return responses;
}

}}}}
