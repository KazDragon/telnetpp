#ifndef TELNETPP_COMMAND_HPP_
#define TELNETPP_COMMAND_HPP_

#include "telnetpp/core.hpp"
#include <utility>

namespace telnetpp {
    
//* =========================================================================
/// \brief A class that encapsulates the value of a TELNET command.
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

}

#endif
