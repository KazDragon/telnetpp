#pragma once

#include "telnetpp/core.hpp"
#include <iosfwd>

namespace telnetpp {

//* =========================================================================
/// \brief A class that encapsulates the value of a Telnet command.
//* =========================================================================
class TELNETPP_EXPORT command
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    explicit constexpr command(u8 command)
      : command_(command)
    {
    }

    //* =====================================================================
    /// \brief Returns the value of the command.
    //* =====================================================================
    constexpr u8 value() const
    {
        return command_;
    }

private :
    u8 command_;
};

//* =========================================================================
/// \brief Comparison function for commands
//* =========================================================================
constexpr bool operator==(command const &lhs, command const &rhs)
{
    return lhs.value() == rhs.value();
}

//* =========================================================================
/// \brief Comparison function for commands
//* =========================================================================
constexpr bool operator<(command const &lhs, command const &rhs)
{
    return lhs.value() < rhs.value();
}

//* =========================================================================
/// \brief Stream output for commands
//* =========================================================================
TELNETPP_EXPORT
std::ostream &operator<<(std::ostream &out, command const &cmd);

}
