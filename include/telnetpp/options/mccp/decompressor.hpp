#pragma once

#include "telnetpp/core.hpp"
#include <boost/exception/all.hpp>
#include <tuple>
#include <stdexcept>

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
    /// \throws telnetpp::options::mccp::corrupted_stream_error if data was
    /// passed that it could not decompress.  I.e. the stream has been
    /// corrupted or otherwise constructed in an invalid manner.
    //* =====================================================================
    virtual std::tuple<telnetpp::u8stream, bool> decompress(
        telnetpp::u8 byte) = 0;

    //* =====================================================================
    /// \brief Ends the current decompression stream.  Any further calls
    /// to decompress continue as if the stream were created fresh.
    //* =====================================================================
    virtual void end_decompression() = 0;
};

//* =========================================================================
/// \brief An exception that is thrown in the case that a stream of data
/// cannot be decompressed.
//* =========================================================================
class corrupted_stream_error
  : virtual std::domain_error,
    virtual boost::exception
{
public :
    corrupted_stream_error(char const *what_arg);
};

}}}
