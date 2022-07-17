#include "telnetpp/element.hpp"
#include <iostream>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, telnetpp::element const &elem)
{
    std::visit(
        [&out](auto &&data)
        {
            out << "element[" << data << "]";
        },
        elem);

    return out;
}

}
