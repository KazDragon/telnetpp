#include "telnetpp/detail/parse_helper.hpp"
#include "telnetpp/protocol.hpp"

namespace telnetpp { namespace detail {
  
namespace {
 
//* =========================================================================
/// \brief Given a set of tokens, either appends or, if appropriate,
/// merges the given token to the collection.
//* =========================================================================
void append_parsed_token(
    std::vector<telnetpp::token> &tokens,
    telnetpp::token const &token);
    
}

// ==========================================================================
// PARSE_IDLE
// ==========================================================================
parse_state parse_idle(
    u8                            byte, 
    std::vector<telnetpp::token> &tokens)
{
    if (byte == telnetpp::iac)
    {
        return parse_state::iac;
    }
    
    append_parsed_token(tokens, std::string{char(byte)});
    return parse_state::idle;
}

// ==========================================================================
// PARSE_IAC
// ==========================================================================
parse_state parse_iac(
    u8                            byte,
    std::vector<telnetpp::token> &tokens,
    parse_temps                  &temps)
{
    if (byte == telnetpp::iac)
    {
        append_parsed_token(tokens, std::string{char(byte)});
        return parse_state::idle;
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
                return parse_state::negotiation;
                
            case telnetpp::sb :
                return parse_state::subnegotiation;
        }

        append_parsed_token(tokens, telnetpp::command(byte));
        return parse_state::idle;
    }
}

// ==========================================================================
// PARSE_NEGOTIATION
// ==========================================================================
parse_state parse_negotiation(
    telnetpp::u8        byte, 
    std::vector<token> &tokens, 
    parse_temps        &temps)
{
    append_parsed_token(tokens, negotiation{temps.id, byte});
    return parse_state::idle;
}

// ==========================================================================
// PARSE_SUBNEGOTIATION
// ==========================================================================
parse_state parse_subnegotiation(
    telnetpp::u8        byte, 
    std::vector<token> &tokens, 
    parse_temps        &temps)
{
    temps.id = byte;
    temps.content.clear();
    return parse_state::subnegotiation_content;
}

// ==========================================================================
// PARSE_SUBNEGOTIATION_CONTENT
// ==========================================================================
parse_state parse_subnegotiation_content(
    telnetpp::u8        byte, 
    std::vector<token> &tokens, 
    parse_temps        &temps)
{
    if (byte == telnetpp::iac)
    {
        return parse_state::subnegotiation_content_iac;
    }
    else
    {
        temps.content.push_back(byte);
        return parse_state::subnegotiation_content;
    }
}

// ==========================================================================
// PARSE_SUBNEGOTIATION_CONTENT_IAC
// ==========================================================================
parse_state parse_subnegotiation_content_iac(
    telnetpp::u8        byte, 
    std::vector<token> &tokens, 
    parse_temps        &temps)
{
    if (byte == telnetpp::se)
    {
        append_parsed_token(
            tokens,
            subnegotiation(temps.id, temps.content));
        return parse_state::idle;
    }
    else
    {
        temps.content.push_back(byte);
        return parse_state::subnegotiation_content;
    }
}

namespace {

//* =========================================================================
/// \brief A visitor that will append a token to a collection of tokens.
//* =========================================================================
struct token_visitor : boost::static_visitor<>
{
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    token_visitor(std::vector<telnetpp::token> &tokens)
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
    token_visitor visitor(tokens);
    boost::apply_visitor(visitor, token);
}
    
}

}}