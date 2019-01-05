#pragma once

#include "telnetpp/server_option.hpp"
#include "telnetpp/options/new_environ/protocol.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"
#include "telnetpp/options/new_environ/detail/stream.hpp"
#include <map>

namespace telnetpp { namespace options { namespace new_environ {

//* =========================================================================
/// \brief An implementation of the server side of the Telnet New-Environ
/// option.
//* =========================================================================
class TELNETPP_EXPORT server final : public server_option
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    server();

    //* =====================================================================
    /// \brief Sets a "VAR" type variable in the environment.
    //* =====================================================================
    template <typename Continuation>
    void set_variable(
        telnetpp::bytes name,
        telnetpp::bytes value,
        Continuation &&cont)
    {
        variables_[telnetpp::byte_storage{name.begin(), name.end()}] = 
            telnetpp::byte_storage{value.begin(), value.end()};

        if (active())
        {
            broadcast_variable_update(variable_type::var, name, value, cont);
        }
    }

    //* =====================================================================
    /// \brief Deletes a "VAR" type variable from the environment.
    //* =====================================================================
    template <typename Continuation>
    void delete_variable(telnetpp::bytes name, Continuation &&cont)
    {
        variables_.erase(telnetpp::byte_storage{name.begin(), name.end()});

        if (active())
        {
            broadcast_variable_deletion(variable_type::var, name, cont);
        }
    }

    //* =====================================================================
    /// \brief Sets a "USERVAR" type variable in the environment.
    //* =====================================================================
    template <typename Continuation>
    void set_user_variable(
        telnetpp::bytes name,
        telnetpp::bytes value,
        Continuation &&cont)
    {
        user_variables_[telnetpp::byte_storage{name.begin(), name.end()}] = 
            telnetpp::byte_storage{value.begin(), value.end()};

        if (active())
        {
            broadcast_variable_update(
                variable_type::uservar, name, value, cont);
        }
    }

    //* =====================================================================
    /// \brief Deletes a "USERVAR" type variable from the environment.
    //* =====================================================================
    template <typename Continuation>
    void delete_user_variable(telnetpp::bytes name, Continuation &&cont)
    {
        user_variables_.erase(
            telnetpp::byte_storage{name.begin(), name.end()});

        if (active())
        {
            broadcast_variable_deletion(variable_type::uservar, name, cont);
        }
    }

private:
    using variable_storage = std::map<
        telnetpp::byte_storage, 
        telnetpp::byte_storage
    >;

    variable_storage variables_;
    variable_storage user_variables_;

    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active.  Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(
        telnetpp::bytes data,
        continuation const &cont) override;

    //* =====================================================================
    /// \brief Broadcasts a variable update via a subnegotiation
    //* =====================================================================
    template <typename Continuation>
    void broadcast_variable_update(
        variable_type   type,
        telnetpp::bytes name,
        telnetpp::bytes value,
        Continuation &&cont)
    {
        telnetpp::byte_storage storage;
        storage.reserve(3 + name.size() + value.size());

        storage.push_back(detail::info);
        append_variable(storage, type, name, value);

        cont(telnetpp::subnegotiation{option_code(), storage});
    }

    //* =====================================================================
    /// \brief Broadcasts a deleted variable via a subnegotiation
    //* =====================================================================
    template <typename Continuation>
    void broadcast_variable_deletion(
        variable_type   type,
        telnetpp::bytes name,
        Continuation &&cont)
    {
        telnetpp::byte_storage storage;
        storage.reserve(2 + name.size());

        storage.push_back(detail::info);
        append_variable(storage, type, name);

        cont(telnetpp::subnegotiation{option_code(), storage});
    }

    //* =====================================================================
    /// \brief Appends the byte representation of a variable to storage.
    //* =====================================================================
    void append_variable(
        telnetpp::byte_storage &storage,
        variable_type           type,
        telnetpp::bytes         name);

    //* =====================================================================
    /// \brief Appends the byte representation of a variable to storage.
    //* =====================================================================
    void append_variable(
        telnetpp::byte_storage &storage,
        variable_type           type,
        telnetpp::bytes         name,
        telnetpp::bytes         value);
};

}}}
