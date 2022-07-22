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
server::server(telnetpp::session &sess) noexcept
  : telnetpp::server_option(sess, telnetpp::options::new_environ::detail::option)
{
}

// ==========================================================================
// SET_VARIABLE
// ==========================================================================
void server::set_variable(telnetpp::bytes name, telnetpp::bytes value)
{
    variables_[telnetpp::byte_storage{name.begin(), name.end()}] = 
        telnetpp::byte_storage{value.begin(), value.end()};

    if (active())
    {
        broadcast_variable_update(variable_type::var, name, value);
    }
}

// ==========================================================================
// DELETE_VARIABLE
// ==========================================================================
void server::delete_variable(telnetpp::bytes name)
{
    variables_.erase(telnetpp::byte_storage{name.begin(), name.end()});

    if (active())
    {
        broadcast_variable_deletion(variable_type::var, name);
    }
}

// ==========================================================================
// SET_USER_VARIABLE
// ==========================================================================
void server::set_user_variable(telnetpp::bytes name, telnetpp::bytes value)
{
    user_variables_[telnetpp::byte_storage{name.begin(), name.end()}] = 
        telnetpp::byte_storage{value.begin(), value.end()};

    if (active())
    {
        broadcast_variable_update(variable_type::uservar, name, value);
    }
}

// ==========================================================================
// DELETE_USER_VARIABLE
// ==========================================================================
void server::delete_user_variable(telnetpp::bytes name)
{
    user_variables_.erase(telnetpp::byte_storage{name.begin(), name.end()});

    if (active())
    {
        broadcast_variable_deletion(variable_type::uservar, name);
    }
}

// ==========================================================================
// HANDLE_SUBNEGOTIATION
// ==========================================================================
void server::handle_subnegotiation(telnetpp::bytes data)
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
    
    write_subnegotiation(response);
}

// ==========================================================================
// BROADCAST_VARIABLE_UPDATE
// ==========================================================================
void server::broadcast_variable_update(
    variable_type   type,
    telnetpp::bytes name,
    telnetpp::bytes value)
{
    telnetpp::byte_storage storage;
    storage.reserve(3 + name.size() + value.size());

    storage.push_back(detail::info);
    append_variable(storage, type, name, value);

    write_subnegotiation(storage);
}

// ==========================================================================
// BROADCAST_VARIABLE_DELETION
// ==========================================================================
void server::broadcast_variable_deletion(
    variable_type   type,
    telnetpp::bytes name)
{
    telnetpp::byte_storage storage;
    storage.reserve(2 + name.size());

    storage.push_back(detail::info);
    append_variable(storage, type, name);

    write_subnegotiation(storage);
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
