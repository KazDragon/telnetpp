#include "telnetpp/negotiation.hpp"

#include <format>
#include <iostream>

namespace telnetpp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, negotiation const &neg)
{
    std::string_view const neg_type = [](negotiation_type const type) {
        switch (type)
        {
            case telnetpp::will:
                return "WILL";
            case telnetpp::wont:
                return "WONT";
            case telnetpp::do_:
                return "DO";
            case telnetpp::dont:
                return "DONT";
            default:
                assert(false);
                return "";
        }
    }(neg.request());

    return out << std::format(
               "negotiation[{}, 0x{:02X}]", neg_type, neg.option_code());
}

}  // namespace telnetpp
