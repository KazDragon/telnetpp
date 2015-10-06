#ifndef TELNETPP_COMMAND_HPP_
#define TELNETPP_COMMAND_HPP_

#include "telnetpp/core.hpp"
#include <iosfwd>
#include <utility>

namespace telnetpp {
    
//* =========================================================================
/// \brief A class that encapsulates the value of a Telnet command.
//* =========================================================================
class command
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    explicit constexpr command(u8 command)
      : command_(std::move(command))
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
std::ostream &operator<<(std::ostream &out, command const &cmd);

}

#endif
