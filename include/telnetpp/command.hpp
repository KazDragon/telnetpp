#pragma once

#include "telnetpp/core.hpp"

#include <iosfwd>

namespace telnetpp {

//* =========================================================================
/// \brief A class that encapsulates the value of a Telnet command.
//* =========================================================================
class TELNETPP_EXPORT command
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    explicit constexpr command(command_type cmnd) noexcept : command_(cmnd)
    {
    }

    //* =====================================================================
    /// \brief Returns the value of the command.
    //* =====================================================================
    [[nodiscard]] constexpr command_type value() const noexcept
    {
        return command_;
    }

private:
    command_type command_;
};

//* =========================================================================
/// \brief Comparison function for commands
//* =========================================================================
constexpr bool operator==(command const &lhs, command const &rhs) noexcept
{
    return lhs.value() == rhs.value();
}

//* =========================================================================
/// \brief Comparison function for commands
//* =========================================================================
constexpr bool operator<(command const &lhs, command const &rhs) noexcept
{
    return lhs.value() < rhs.value();
}

//* =========================================================================
/// \brief Stream output for commands
//* =========================================================================
TELNETPP_EXPORT
std::ostream &operator<<(std::ostream &out, command const &cmd);

}  // namespace telnetpp
