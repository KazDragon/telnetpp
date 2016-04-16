#pragma once

#include "telnetpp/element.hpp"
#include <vector>

namespace telnetpp { namespace options { namespace mccp {

class TELNETPP_EXPORT codec
{
public :
    std::vector<telnetpp::token> send(
        std::vector<telnetpp::token> const &input);

private :
    bool                         blocked_ = false;
    std::vector<telnetpp::token> buffer_;
};

}}}
