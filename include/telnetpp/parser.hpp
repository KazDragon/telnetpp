#ifndef TELNETPP_PARSER_HPP_
#define TELNETPP_PARSER_HPP_

#include "telnetpp/command.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/protocol.hpp"
#include "telnetpp/subnegotiation.hpp"
#include "telnetpp/token.hpp"
#include "telnetpp/detail/parse_helper.hpp"
#include "telnetpp/detail/parse_state.hpp"
#include <cassert>
#include <vector>

namespace telnetpp {
    
//* =========================================================================
/// \brief Parse a series of bytes into a stream of Telnet tokens
/// \param begin a reference to the start of the series.  This will be 
///        modified to be an iterator to the position after the last
///        completely-parsed token.
/// \param end an iterator to the end of the series.
/// \returns a collection of completely parsed tokens.
//* =========================================================================
template <class InputIterator1, class InputIterator2>
std::vector<token> parse(InputIterator1 &begin, InputIterator2 end)
{
    std::vector<token> result;
    
    detail::parse_state current_state(detail::parse_state::idle);
    detail::parse_temps temps;
    
    auto position = begin;
    while (position != end)
    {
        switch (current_state)
        {
            case detail::parse_state::idle : 
                current_state = detail::parse_idle(*position, result);
                break;
                
            case detail::parse_state::iac :
                current_state = detail::parse_iac(
                    *position, result, temps);
                break;
                
            case detail::parse_state::negotiation :
                current_state = detail::parse_negotiation(
                    *position, result, temps);
                break;
                
            case detail::parse_state::subnegotiation :
                current_state = detail::parse_subnegotiation(
                    *position, result, temps);
                break;
                
            case detail::parse_state::subnegotiation_content :
                current_state = detail::parse_subnegotiation_content(
                    *position, result, temps);
                break;
                
            case detail::parse_state::subnegotiation_content_iac :
                current_state = detail::parse_subnegotiation_content_iac(
                    *position, result, temps);
                break;
                
            default :
                assert(!"Telnet Parser in invalid state");
                break;
        }
        
        ++position;
        
        // If the current state is idle, then it means that a token has been
        // completely parsed, and therefore 
        if (current_state == detail::parse_state::idle)
        {
            begin = position;
        }
    }
    
    return result;
}

}

#endif
