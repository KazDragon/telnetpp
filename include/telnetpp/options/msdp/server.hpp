#ifndef TELNETPP_OPTIONS_MSDP_SERVER_HPP_
#define TELNETPP_OPTIONS_MSDP_SERVER_HPP_

#include "telnetpp/options/msdp.hpp"
#include "telnetpp/server_option.hpp"

namespace telnetpp { namespace options { namespace msdp {

class TELNETPP_EXPORT server : public telnetpp::server_option
{
public :
    server();
    
    std::vector<telnetpp::token> send(std::vector<variable> const &variables);
        
    boost::signals2::signal<
        std::vector<telnetpp::token> (std::vector<variable> const &),
        telnetpp::token_combiner
    > on_receive;
};

}}}

#endif
