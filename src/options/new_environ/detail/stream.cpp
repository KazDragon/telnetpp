#include "telnetpp/options/new_environ/detail/stream.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"

namespace telnetpp { namespace options { namespace new_environ {
    namespace detail {

// ==========================================================================
// APPEND_ESCAPED
// ==========================================================================
void append_escaped(telnetpp::byte_storage &storage, telnetpp::bytes content)
{
    for (auto &ch : content)
    {
        switch (ch)
        {
            case telnetpp::options::new_environ::detail::var :   // Fall-through
            case telnetpp::options::new_environ::detail::value : // Fall-through
            case telnetpp::options::new_environ::detail::esc :   // Fall-through
            case telnetpp::options::new_environ::detail::uservar : // Fall-through
                storage.push_back(telnetpp::options::new_environ::detail::esc);
                // fall-through
            default :
                storage.push_back(ch);
                break;
        }
    }
}

}}}}
