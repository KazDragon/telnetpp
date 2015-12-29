#include "telnetpp/options/new_environ/client.hpp"
#include "telnetpp/options/new_environ.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
client::client()
  : telnetpp::client_option(telnetpp::options::new_environ::option)
{
}

// ==========================================================================
// SET_VARIABLE
// ==========================================================================
void client::set_variable(std::string const &name, std::string const &value)
{
    variables_[name] = value;
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
std::vector<telnetpp::token> client::handle_subnegotiation(
    u8stream const &content)
{
    u8stream result = { is };
    
    for (auto &variable : variables_)
    {
        auto &name = variable.first;
        auto &val = variable.second;
        
        result.push_back(var);

        std::copy(name.begin(), name.end(), back_inserter(result));
        
        result.push_back(value);
        
        std::copy(val.begin(), val.end(), back_inserter(result));
    }
    
    return { telnetpp::element{
        telnetpp::subnegotiation{option(), result}
    }};    
}


}}}
