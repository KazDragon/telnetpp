#include "telnetpp/detail/parse_helper.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"
#include <cassert>

namespace telnetpp { namespace detail {

namespace {

//* =========================================================================
/// \brief Given a set of elements, either appends or, if appropriate,
/// merges the given element to the collection.
//* =========================================================================
void append_parsed_element(
    std::vector<telnetpp::element> &elements,
    telnetpp::element const &element);

// ==========================================================================
// PARSE_IDLE
// ==========================================================================
void parse_idle(
    parse_temps &temps,
    byte         data)
{
    if (data == telnetpp::iac)
    {
        temps.state = parse_state::iac;
    }
    else
    {
        append_parsed_element(temps.elements, std::string{char(data)});
    }
}

// ==========================================================================
// PARSE_IAC
// ==========================================================================
void parse_iac(
    parse_temps &temps,
    byte         data)
{
    if (data == telnetpp::iac)
    {
        append_parsed_element(temps.elements, std::string{char(data)});
        temps.state = parse_state::idle;
    }
    else
    {
        switch (data)
        {
            case telnetpp::will : // fall-through
            case telnetpp::wont : // fall-through
            case telnetpp::do_  : // fall-through
            case telnetpp::dont : // fall-through
                temps.id = data;
                temps.state = parse_state::negotiation;
                break;

            case telnetpp::sb :
                temps.state = parse_state::subnegotiation;
                break;

            default :
                append_parsed_element(temps.elements, telnetpp::command(data));
                temps.state = parse_state::idle;
        }
    }
}

// ==========================================================================
// PARSE_NEGOTIATION
// ==========================================================================
void parse_negotiation(
    parse_temps &temps,
    byte         data)
{
    append_parsed_element(temps.elements, negotiation{temps.id, data});
    temps.state = parse_state::idle;
}

// ==========================================================================
// PARSE_SUBNEGOTIATION
// ==========================================================================
void parse_subnegotiation(
    parse_temps &temps,
    byte         data)
{
    temps.id = data;
    temps.subnegotiation_content.clear();
    temps.state = parse_state::subnegotiation_content;
}

// ==========================================================================
// PARSE_SUBNEGOTIATION_CONTENT
// ==========================================================================
void parse_subnegotiation_content(
    parse_temps &temps,
    byte         data)
{
    if (data == telnetpp::iac)
    {
        temps.state = parse_state::subnegotiation_content_iac;
    }
    else
    {
        temps.subnegotiation_content.push_back(data);
        temps.state = parse_state::subnegotiation_content;
    }
}

// ==========================================================================
// PARSE_SUBNEGOTIATION_CONTENT_IAC
// ==========================================================================
void parse_subnegotiation_content_iac(
    parse_temps &temps,
    byte         data)
{
    if (data == telnetpp::se)
    {
        append_parsed_element(
            temps.elements,
            subnegotiation(temps.id, temps.subnegotiation_content));
        temps.state = parse_state::idle;
    }
    else
    {
        temps.subnegotiation_content.push_back(data);
        temps.state = parse_state::subnegotiation_content;
    }
}

// ==========================================================================
// APPEND_PARSED_ELEMENT
// ==========================================================================
void append_parsed_element(
    std::vector<telnetpp::element> &elements,
    telnetpp::element const &element)
{
    detail::visit_lambdas(
        element,
        // In the case of the string, we want to merge strings together where
        // possible so that there are never adjacent string elements.
        [&elements](std::string const &text)
        {
            if (elements.empty())
            {
                elements.push_back(text);
            }
            else
            {
                detail::visit_lambdas(
                    elements.back(),
                    [&text](std::string &existing_text)
                    {
                        existing_text += text;
                    },
                    [&text, &elements](auto const &)
                    {
                        elements.push_back(text);
                    });
            }
        },
        // All other types can just be appended to the list of elements.
        [&elements](auto const &element)
        {
            elements.emplace_back(element);
        });
}

}

// ==========================================================================
// PARSE_HELPER
// ==========================================================================
void parse_helper(parse_temps &temps, byte data)
{
    switch (temps.state)
    {
        case parse_state::idle :
            parse_idle(temps, data);
            break;

        case parse_state::iac :
            parse_iac(temps, data);
            break;

        case parse_state::negotiation :
            parse_negotiation(temps, data);
            break;

        case parse_state::subnegotiation :
            parse_subnegotiation(temps, data);
            break;

        case parse_state::subnegotiation_content :
            parse_subnegotiation_content(temps, data);
            break;

        case parse_state::subnegotiation_content_iac :
            parse_subnegotiation_content_iac(temps, data);
            break;

        default :
            assert(!"element parser in invalid state");
            break;
    }
}


}}
