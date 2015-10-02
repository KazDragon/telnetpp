#include "telnetpp/subnegotiation.hpp"
#include <utility>

namespace telnetpp {
    
// ==========================================================================    
// CONSTRUCTOR
// ==========================================================================
subnegotiation::subnegotiation(u8 option, subnegotiation_content content)
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
subnegotiation_content const &subnegotiation::content() const
{
    return content_;
}

}
