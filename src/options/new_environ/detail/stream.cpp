#include "telnetpp/options/new_environ/detail/stream.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

// ==========================================================================
// APPEND_ESCAPED
// ==========================================================================
void append_escaped(
    telnetpp::u8stream &stream, std::string const &data)
{
    for (auto &ch : data)
    {
        switch (ch)
        {
            case telnetpp::options::new_environ::detail::var :   // Fall-through
            case telnetpp::options::new_environ::detail::value : // Fall-through
            case telnetpp::options::new_environ::detail::esc :   // Fall-through
            case telnetpp::options::new_environ::detail::uservar :
                // Fall-through
                stream.push_back(telnetpp::options::new_environ::detail::esc);
            default :
                // Fall-through
                stream.push_back(ch);
                break;
        }
    }
}

}}}}
