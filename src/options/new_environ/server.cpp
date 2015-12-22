#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    
// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : server_option(telnetpp::options::new_environ::option)
{
    
}

// ==========================================================================
// REQUEST
// ==========================================================================
std::vector<token> server::request_variables(std::vector<request> const &requests)
{
    telnetpp::u8stream content;
    content.push_back(telnetpp::options::new_environ::send);
    
    for(auto &req : requests)
    {
        content.push_back(req.type);
        
        for (auto &ch : req.name)
        {
            switch (ch)
            {
                case telnetpp::options::new_environ::var : // Fall-through
                case telnetpp::options::new_environ::value : // Fall-through
                case telnetpp::options::new_environ::esc : // Fall-through
                case telnetpp::options::new_environ::uservar :
                    // Fall-through
                    content.push_back(telnetpp::options::new_environ::esc);
                default :
                    content.push_back(ch);
                    break;
            }
        }                
    }

    return {
        telnetpp::element { 
            telnetpp::subnegotiation(option(), content)
        }
    };
}

}}}
