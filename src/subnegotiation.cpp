#include "telnetpp/subnegotiation.hpp"
#include <iostream>
#include <utility>

namespace telnetpp {
    
// ==========================================================================    
// CONSTRUCTOR
// ==========================================================================
subnegotiation::subnegotiation(u8 option, u8stream content)
  : option_(std::move(option)),
    content_(std::move(content))
{
}
   
// ==========================================================================    
// OPTION
// ==========================================================================
u8 subnegotiation::option() const
{
    return option_;
}

// ==========================================================================    
// CONTENT
// ==========================================================================
u8stream const &subnegotiation::content() const
{
    return content_;
}

// ==========================================================================    
// OPERATOR==
// ==========================================================================    
bool operator==(subnegotiation const &lhs, subnegotiation const &rhs)
{
    return lhs.option() == rhs.option()
        && lhs.content() == rhs.content();
}

// ==========================================================================    
// OPERATOR<<
// ==========================================================================    
std::ostream &operator<<(std::ostream& out, const subnegotiation& sub)
{
    out << "subnegotiation[0x"
        << std::hex << u32(sub.option()) 
        << ", [";
        
    for (auto &&byte : sub.content())
    {
        out << "0x" << std::hex << u32(byte) << ", ";
    }
    
    return out << "]]";
}

}
