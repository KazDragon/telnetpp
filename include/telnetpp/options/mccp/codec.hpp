#pragma once

#include "telnetpp/element.hpp"
#include <boost/optional.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <vector>

namespace telnetpp { namespace options { namespace mccp {

class TELNETPP_EXPORT codec : boost::noncopyable
{
public :
    codec();
    ~codec();

    std::vector<telnetpp::stream_token> send(
        std::vector<telnetpp::stream_token> const &tokens);

    struct impl;

private :
    std::shared_ptr<impl> pimpl_;
};

}}}
