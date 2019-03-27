#include "telnetpp/options/msdp/detail/encoder.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"

using variable = telnetpp::options::msdp::variable;

namespace telnetpp { namespace options { namespace msdp { namespace detail {

// ==========================================================================
// ENCODE_VARIABLE
// ==========================================================================
void encode_variable(
    telnetpp::options::msdp::variable const &var,
    telnetpp::byte_storage &storage)
{
    storage.push_back(telnetpp::options::msdp::detail::var);
    storage += var.name;
    storage.push_back(telnetpp::options::msdp::detail::val);
    
    telnetpp::detail::visit_lambdas(
        var.value,
        [&](telnetpp::options::msdp::string_value const &val)
        {
            storage += val;
        },
        [&](telnetpp::options::msdp::array_value const &arr)
        {
            storage.push_back(telnetpp::options::msdp::detail::array_open);
            for(auto const &val : arr)
            {
                storage.push_back(telnetpp::options::msdp::detail::val);
                storage += val;
            }
            storage.push_back(telnetpp::options::msdp::detail::array_close);
        },
        [&](telnetpp::options::msdp::table_value const &tbl)
        {
            for (auto const &val : tbl)
            {
                storage.push_back(telnetpp::options::msdp::detail::table_open);
                encode_variable(val, storage);
                storage.push_back(telnetpp::options::msdp::detail::table_close);
            }
        });
}

}}}}
