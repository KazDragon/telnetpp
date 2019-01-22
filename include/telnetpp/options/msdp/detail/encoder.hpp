#pragma once

#include "telnetpp/options/msdp/variable.hpp"
#include "telnetpp/options/msdp/detail/protocol.hpp"
#include "telnetpp/detail/lambda_visitor.hpp"

namespace telnetpp { namespace options { namespace msdp { namespace detail {

//* =========================================================================
/// \brief Encode a list of MSDP variables into a sequence of bytes.
//* =========================================================================
template <typename Continuation>
void encode(
    telnetpp::options::msdp::variable const &var,
    Continuation &&cont)
{
    telnetpp::byte_storage storage;
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
        [&](auto &&)
        {
        });
        
    cont(storage);
}

}}}}
