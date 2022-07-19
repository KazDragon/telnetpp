#include "telnetpp/element.hpp"
#include <iostream>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, telnetpp::element const &elem)
{
    return std::visit(
        [&out](auto &&data) -> std::ostream &
        {
            return out << "element[" << data << "]";
        },
        elem);
}

}
