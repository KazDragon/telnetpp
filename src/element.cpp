#include "telnetpp/element.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"
#include <iostream>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, telnetpp::element const &elem)
{
    detail::visit_lambdas(
        elem,
        [&out](auto &&data)
        {
            out << "element[" << data << "]";
        }
    );

    return out;
}

}
