#pragma once

#include "telnetpp/core.hpp"
#include <tuple>

namespace telnetpp { namespace options { namespace mccp {

//* =========================================================================
/// \brief Represents an object that can decompress arbitrary byte sequences.
//* =========================================================================
class decompressor
{
public :
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~decompressor() {}

    //* =====================================================================
    /// \brief Decompress the given byte, and return a tuple of the
    /// decompressed data and a boolean that is set to true if this was the
    /// end of the decompression stream.
    //* =====================================================================
    virtual std::tuple<telnetpp::u8stream, bool> decompress(
        telnetpp::u8 byte) = 0;

    //* =====================================================================
    /// \brief Ends the current decompression stream.  Any further calls
    /// to decompress continue as if the stream were created fresh.
    //* =====================================================================
    virtual void end_decompression() = 0;
};

}}}
