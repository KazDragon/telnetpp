#include "telnetpp/options/msdp/variable.hpp"

#include "telnetpp/detail/overloaded.hpp"

#include <boost/algorithm/string/join.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <ostream>
#include <sstream>

namespace telnetpp::options::msdp {

// ==========================================================================
// OPERATOR<<
// ==========================================================================
std::ostream &operator<<(std::ostream &out, variable const &var)
{
    using boost::adaptors::transformed;

    static auto const to_string = [](auto const &str) {
        return std::string{reinterpret_cast<char const *>(str.c_str())};
    };

    out << to_string(var.name_) << "=";

    std::visit(
        detail::overloaded{
            [&](string_value const &sv) {
                out << "\"" << to_string(sv) << "\"";
            },
            [&](array_value const &av) {
                out << "[";
                out << boost::algorithm::join(av | transformed(to_string), ",");
                out << "]";
            },
            [&](table_value const &tv) {
                static auto const variable_to_string = [](auto const &tab_var) {
                    std::stringstream stream;
                    stream << tab_var;
                    return stream.str();
                };

                out << "{";
                out << boost::algorithm::join(
                    tv | transformed(variable_to_string), ",");
                out << "}";
            }},
        var.value_);

    return out;
}

}  // namespace telnetpp::options::msdp
