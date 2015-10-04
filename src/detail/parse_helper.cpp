#include "telnetpp/detail/parse_helper.hpp"
#include "telnetpp/protocol.hpp"
#include <cassert>

namespace telnetpp { namespace detail {
  
namespace {
 
//* =========================================================================
/// \brief Given a set of tokens, either appends or, if appropriate,
/// merges the given token to the collection.
//* =========================================================================
void append_parsed_token(
    std::vector<telnetpp::token> &tokens,
    telnetpp::token const &token);
    
// ==========================================================================
// PARSE_IDLE
// ==========================================================================
void parse_idle(
    parse_temps &temps,
    u8           byte)
{
    if (byte == telnetpp::iac)
    {
        temps.state = parse_state::iac;
    }
    else
    {
        append_parsed_token(temps.tokens, std::string{char(byte)});
    }
}

// ==========================================================================
// PARSE_IAC
// ==========================================================================
void parse_iac(
    parse_temps &temps,
    u8           byte)
{
    if (byte == telnetpp::iac)
    {
        append_parsed_token(temps.tokens, std::string{char(byte)});
        temps.state = parse_state::idle;
    }
    else
    {
        switch (byte)
        {
            case telnetpp::will : // fall-through
            case telnetpp::wont : // fall-through
            case telnetpp::do_  : // fall-through
            case telnetpp::dont : // fall-through
                temps.id = byte;
                temps.state = parse_state::negotiation;
                break;
                
            case telnetpp::sb :
                temps.state = parse_state::subnegotiation;
                break;
                
            default :
                append_parsed_token(temps.tokens, telnetpp::command(byte));
                temps.state = parse_state::idle;
        }
    }
}

// ==========================================================================
// PARSE_NEGOTIATION
// ==========================================================================
void parse_negotiation(
    parse_temps  &temps,
    telnetpp::u8  byte)
{
    append_parsed_token(temps.tokens, negotiation{temps.id, byte});
    temps.state = parse_state::idle;
}

// ==========================================================================
// PARSE_SUBNEGOTIATION
// ==========================================================================
void parse_subnegotiation(
    parse_temps  &temps,
    telnetpp::u8  byte)
{
    temps.id = byte;
    temps.subnegotiation_content.clear();
    temps.state = parse_state::subnegotiation_content;
}

// ==========================================================================
// PARSE_SUBNEGOTIATION_CONTENT
// ==========================================================================
void parse_subnegotiation_content(
    parse_temps  &temps,
    telnetpp::u8  byte)
{
    if (byte == telnetpp::iac)
    {
        temps.state = parse_state::subnegotiation_content_iac;
    }
    else
    {
        temps.subnegotiation_content.push_back(byte);
        temps.state = parse_state::subnegotiation_content;
    }
}

// ==========================================================================
// PARSE_SUBNEGOTIATION_CONTENT_IAC
// ==========================================================================
void parse_subnegotiation_content_iac(
    parse_temps  &temps,
    telnetpp::u8  byte)
{
    if (byte == telnetpp::se)
    {
        append_parsed_token(
            temps.tokens,
            subnegotiation(temps.id, temps.subnegotiation_content));
        temps.state = parse_state::idle;
    }
    else
    {
        temps.subnegotiation_content.push_back(byte);
        temps.state = parse_state::subnegotiation_content;
    }
}

//* =========================================================================
/// \brief A visitor that will append a token to a collection of tokens.
//* =========================================================================
struct parser_token_visitor : boost::static_visitor<>
{
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    parser_token_visitor(std::vector<telnetpp::token> &tokens)
      : tokens_(tokens)
    {
    }
      
    //* =====================================================================
    /// \brief Either appends or merges a string of text depending on whether
    /// the last element of the collection is also a string.
    //* =====================================================================
    void operator()(std::string const &text)
    {
        if (tokens_.empty() || tokens_.back().type() != typeid(std::string))
        {
            tokens_.emplace_back(text);
        }
        else
        {
            boost::get<std::string>(tokens_.back()) += text;
        }
    }
    
    //* =====================================================================
    /// \brief appends a Telnet command to the collection.
    //* =====================================================================
    void operator()(telnetpp::command const &cmd)
    {
        tokens_.emplace_back(cmd);
    }
    
    //* =====================================================================
    /// \brief appends a Telnet negotiation to the collection.
    //* =====================================================================
    void operator()(telnetpp::negotiation const &neg)
    {
        tokens_.emplace_back(neg);
    }

    //* =====================================================================
    /// \brief appends a Telnet subnegotiation to the collection.
    //* =====================================================================
    void operator()(telnetpp::subnegotiation const &sub)
    {
        tokens_.emplace_back(sub);
    }
    
    std::vector<telnetpp::token> &tokens_;
};

// ==========================================================================
// APPEND_PARSED_TOKEN
// ==========================================================================
void append_parsed_token(
    std::vector<telnetpp::token> &tokens,
    telnetpp::token const &token)
{
    parser_token_visitor visitor(tokens);
    boost::apply_visitor(visitor, token);
}

}

// ==========================================================================
// PARSE_HELPER
// ==========================================================================
void parse_helper(parse_temps &temps, telnetpp::u8 byte)
{
    switch (temps.state)
    {
        case parse_state::idle :
            parse_idle(temps, byte);
            break;
            
        case parse_state::iac :
            parse_iac(temps, byte);
            break;
            
        case parse_state::negotiation :
            parse_negotiation(temps, byte);
            break;
            
        case parse_state::subnegotiation :
            parse_subnegotiation(temps, byte);
            break;
            
        case parse_state::subnegotiation_content :
            parse_subnegotiation_content(temps, byte);
            break;
            
        case parse_state::subnegotiation_content_iac :
            parse_subnegotiation_content_iac(temps, byte);
            break;
            
        default :
            assert(!"Token parser in invalid state");
            break;
    }
}


}}