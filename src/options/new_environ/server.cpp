#include "telnetpp/options/new_environ/server.hpp"
#include "telnetpp/options/new_environ/detail/for_each_request.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"
#include "telnetpp/options/new_environ/detail/stream.hpp"
#include <boost/range/algorithm/for_each.hpp>
#include <utility>

namespace telnetpp { namespace options { namespace new_environ {

namespace {

// ==========================================================================
// TYPE_TO_BYTE
// ==========================================================================
static byte type_to_byte(variable_type const &type)
{
    return type == variable_type::var
         ? telnetpp::options::new_environ::detail::var
         : telnetpp::options::new_environ::detail::uservar;
}

}


// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
server::server()
  : telnetpp::server_option(telnetpp::options::new_environ::detail::option)
{
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void server::handle_subnegotiation(
    telnetpp::bytes data,
    continuation const &cont)
{
    telnetpp::byte_storage response{
        telnetpp::options::new_environ::detail::is
    };

    if (data.size() == 1)
    {
        // It can be assumed that this is an empty "SEND" subnegotiation.
        // This is interpreted to have the meaning "Send all the things".
        boost::for_each(
            variables_,
            [&](auto &&variable)
            {
                this->append_variable(
                    response, variable_type::var, 
                    variable.first, variable.second);
            });

        boost::for_each(
            user_variables_,
            [&](auto &&variable)
            {
                this->append_variable(
                    response, variable_type::uservar,
                    variable.first, variable.second);
            });
    }
    else
    {
        detail::for_each_request(
            data,
            [&](request const &req)
            {
                if (req.type == variable_type::var)
                {
                    auto it = variables_.find(req.name);

                    if (it != variables_.end())
                    {
                        this->append_variable(
                            response, variable_type::var,
                            req.name, it->second);
                    }
                }
                else
                {
                    auto it = user_variables_.find(req.name);

                    if (it != user_variables_.end())
                    {
                        this->append_variable(
                            response, variable_type::uservar,
                            req.name, it->second);
                    }
                }
            });
    }
    
    cont(telnetpp::subnegotiation{
        option_code(),
        response
    });
}

// ==========================================================================
// APPEND_VARIABLE
// ==========================================================================
void server::append_variable(
    telnetpp::byte_storage &storage,
    variable_type           type,
    telnetpp::bytes         name)
{
    storage.push_back(type_to_byte(type));
    detail::append_escaped(storage, name);
}

// ==========================================================================
// APPEND_VARIABLE
// ==========================================================================
void server::append_variable(
    telnetpp::byte_storage &storage,
    variable_type           type,
    telnetpp::bytes         name,
    telnetpp::bytes         value)
{
    append_variable(storage, type, name);
    storage.push_back(detail::value);
    detail::append_escaped(storage, value);
}

}}}

