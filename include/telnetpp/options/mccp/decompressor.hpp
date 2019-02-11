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
public:
    using continuation = std::function<
        void (telnetpp::bytes data, bool continuation_ended)
    >;

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~decompressor() = default;

    //* =====================================================================
    /// \brief begins the decompression stream.  Calls to operator() now
    /// assume that the input is compressed.
    //* =====================================================================
    void begin_decompression();

    //* =====================================================================
    /// \brief Ends the current decompression stream.  Calls to operator()
    /// now assume that the input is uncompressed.
    //* =====================================================================
    void end_decompression();
    
    //* =====================================================================
    /// \brief Decompress data, sending the result of the decompression
    /// to the continuation.
    //* =====================================================================
    void operator()(telnetpp::bytes data, continuation const &cont);
    
private:
    //* =====================================================================
    /// \brief A hook for when compression starts.
    //* =====================================================================
    virtual void do_begin_decompression() {};

    //* =====================================================================
    /// \brief A hook for when compression ends.
    //* =====================================================================
    virtual void do_end_decompression() {};

    //* =====================================================================
    /// \brief Decompress the given byte, and return a tuple of the
    /// decompressed data and a boolean that is set to true if this was the
    /// end of the decompression stream.
    /// \throws telnetpp::options::mccp::corrupted_stream_error if data was
    /// passed that it could not decompress.  I.e. the stream has been
    /// corrupted or otherwise constructed in an invalid manner.
    //* =====================================================================
    virtual telnetpp::bytes decompress_chunk(
      telnetpp::bytes data,
      continuation const &continuation) = 0;
      
    bool engaged_{false};
};

//* =========================================================================
/// \brief An exception that is thrown in the case that a stream of data
/// cannot be decompressed.
//* =========================================================================
class corrupted_stream_error
  : virtual std::domain_error,
    virtual boost::exception
{
public:
    //* =====================================================================
    /// Constructor
    //* =====================================================================
    explicit corrupted_stream_error(char const *what_arg);
};

}}}
