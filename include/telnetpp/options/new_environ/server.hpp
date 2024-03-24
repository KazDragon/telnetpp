#pragma once

#include "telnetpp/options/new_environ/protocol.hpp"
#include "telnetpp/server_option.hpp"

#include <map>

namespace telnetpp::options::new_environ {

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
    explicit server(telnetpp::session &sess) noexcept;

    //* =====================================================================
    /// \brief Sets a "VAR" type variable in the environment.
    //* =====================================================================
    void set_variable(telnetpp::bytes name, telnetpp::bytes value);

    //* =====================================================================
    /// \brief Deletes a "VAR" type variable from the environment.
    //* =====================================================================
    void delete_variable(telnetpp::bytes name);

    //* =====================================================================
    /// \brief Sets a "USERVAR" type variable in the environment.
    //* =====================================================================
    void set_user_variable(telnetpp::bytes name, telnetpp::bytes value);

    //* =====================================================================
    /// \brief Deletes a "USERVAR" type variable from the environment.
    //* =====================================================================
    void delete_user_variable(telnetpp::bytes name);

private:
    using variable_storage =
        std::map<telnetpp::byte_storage, telnetpp::byte_storage>;

    variable_storage variables_;
    variable_storage user_variables_;

    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active.  Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(telnetpp::bytes data) override;

    //* =====================================================================
    /// \brief Broadcasts a variable update via a subnegotiation
    //* =====================================================================
    void broadcast_variable_update(
        variable_type type, telnetpp::bytes name, telnetpp::bytes value);

    //* =====================================================================
    /// \brief Broadcasts a deleted variable via a subnegotiation
    //* =====================================================================
    void broadcast_variable_deletion(variable_type type, telnetpp::bytes name);

    //* =====================================================================
    /// \brief Appends the byte representation of a variable to storage.
    //* =====================================================================
    void append_variable(
        telnetpp::byte_storage &storage,
        variable_type type,
        telnetpp::bytes name);

    //* =====================================================================
    /// \brief Appends the byte representation of a variable to storage.
    //* =====================================================================
    void append_variable(
        telnetpp::byte_storage &storage,
        variable_type type,
        telnetpp::bytes name,
        telnetpp::bytes value);
};

}  // namespace telnetpp::options::new_environ
