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
    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~decompressor() {}

    //* =====================================================================
    /// \brief starts the decompression stream.  Calls to operator() now
    /// assume that the input is compressed.
    //* =====================================================================
    void start_decompression() { engaged_ = true; }

    //* =====================================================================
    /// \brief Ends the current decompression stream.  Calls to operator()
    /// now assume that the input is uncompressed.
    //* =====================================================================
    void end_decompression() { engaged_ = false; }
    
    //* =====================================================================
    /// \brief Decompress data, sending the result of the decompression
    /// to the continuation.
    //* =====================================================================
    template <typename Continuation>
    void operator()(telnetpp::bytes data, Continuation &&continuation)
    {
        while (!data.empty())
        {
            if (engaged_)
            {
                data = decompress_chunk(
                    data,
                    [&](telnetpp::bytes decompressed_data, bool decompression_ended)
                    {
                        engaged_ = !decompression_ended;
                        continuation(decompressed_data, decompression_ended);
                    });
            }
            else
            {
                // Every byte might activate decompression, so they must be
                // transmitted individually.
                continuation(data.subspan(0, 1), false);
                data = data.subspan(1);
            }
        }
    }
    
private:
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
      std::function<void (telnetpp::bytes, bool)> const &continuation) = 0;
      
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
