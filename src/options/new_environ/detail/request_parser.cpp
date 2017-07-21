#include "telnetpp/options/new_environ/detail/request_parser.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

namespace {

enum class parse_state
{
    send,
    type,
    name,
    name_esc,
};

// ==========================================================================
// BYTE_TO_TYPE
// ==========================================================================
static telnetpp::options::new_environ::variable_type byte_to_type(byte value)
{
    return value == detail::var ? variable_type::var : variable_type::uservar;
}

// ==========================================================================
// PARSE_NAME_ESC
// ==========================================================================
boost::optional<request> parse_name_esc(
    parse_state &state, request &temp, byte data)
{
    temp.name.push_back(char(data));
    state = parse_state::name;

    return {};
}

// ==========================================================================
// PARSE_NAME
// ==========================================================================
boost::optional<request> parse_name(
    parse_state &state, request &temp, byte data)
{
    boost::optional<request> resp;

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
            state = parse_state::name;
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
boost::optional<request> parse_type(
    parse_state &state, request &temp, byte data)
{
    temp.type = byte_to_type(data);
    state = parse_state::name;
    return {};
}

// ==========================================================================
// PARSE_IS_OR_INFO
// ==========================================================================
boost::optional<request> parse_send(
    parse_state &state, request &temp, byte data)
{
    state = parse_state::type;
    return {};
}

// ==========================================================================
// PARSE_DATA
// ==========================================================================
boost::optional<request> parse_data(
    parse_state &state, request &temp, byte data)
{
    boost::optional<request> req;

    switch (state)
    {
        case parse_state::send :
            req = parse_send(state, temp, data);
            break;

        case parse_state::type :
            req = parse_type(state, temp, data);
            break;

        case parse_state::name :
            req = parse_name(state, temp, data);
            break;

        case parse_state::name_esc :
            req = parse_name_esc(state, temp, data);
            break;

        default :
            return {};
            break;
    }

    return req;
}

}

// ==========================================================================
// PARSE_RESPONSES
// ==========================================================================
std::vector<request> parse_requests(telnetpp::byte_stream const &stream)
{
    std::vector<request> requests;
    parse_state state = parse_state::send;

    request temp;
    boost::optional<request> resp;

    for (auto data : stream)
    {
        resp = parse_data(state, temp, data);

        if (resp)
        {
            requests.push_back(*resp);
        }
    }

    if (!temp.name.empty())
    {
        requests.push_back(temp);
    }

    return requests;
}

}}}}
