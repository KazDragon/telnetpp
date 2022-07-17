#include "telnetpp/options/msdp/detail/encoder.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"
#include "telnetpp/detail/overloaded.hpp"

using variable = telnetpp::options::msdp::variable;

namespace telnetpp { namespace options { namespace msdp { namespace detail {

// ==========================================================================
// ENCODE_VARIABLE
// ==========================================================================
void encode_variable(
    telnetpp::options::msdp::variable const &encode_var,
    telnetpp::byte_storage &storage)
{
    storage.push_back(telnetpp::options::msdp::detail::var);
    storage += encode_var.name_;
    storage.push_back(telnetpp::options::msdp::detail::val);
    
    std::visit(telnetpp::detail::overloaded{
        [&](telnetpp::options::msdp::string_value const &vbl)
        {
            storage += vbl;
        },
        [&](telnetpp::options::msdp::array_value const &arr)
        {
            storage.push_back(telnetpp::options::msdp::detail::array_open);
            for(auto const &vbl : arr)
            {
                storage.push_back(telnetpp::options::msdp::detail::val);
                storage += vbl;
            }
            storage.push_back(telnetpp::options::msdp::detail::array_close);
        },
        [&](telnetpp::options::msdp::table_value const &tbl)
        {
            for (auto const &vbl : tbl)
            {
                storage.push_back(telnetpp::options::msdp::detail::table_open);
                encode_variable(vbl, storage);
                storage.push_back(telnetpp::options::msdp::detail::table_close);
            }
        }},
        encode_var.value_);
}

}}}}
