#pragma once

#include "telnetpp/core.hpp"

#include <boost/exception/exception.hpp>

#include <functional>
#include <stdexcept>

namespace telnetpp::options::mccp {

//* =========================================================================
/// \brief Represents an object that can transform (encode or decode)
/// arbitrary byte sequences.  For this option, this implies compression
/// and decompression.
//* =========================================================================
class TELNETPP_EXPORT codec
{
public:
    using continuation =
        std::function<void(telnetpp::bytes data, bool continuation_ended)>;

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    virtual ~codec() = default;

    //* =====================================================================
    /// \brief Starts the transformation stream.  Calls to operator() now
    /// assume that the input requires transformation.
    //* =====================================================================
    void start();

    //* =====================================================================
    /// \brief Finishes the current decompression stream.  Calls to
    /// operator() now assume that the input does not require transformation.
    //* =====================================================================
    void finish(continuation const &cont);

    //* =====================================================================
    /// \brief Transform data, if the stream is started, sending the result
    /// of the transformation to the continuation.  If the stream is not
    /// started, the data is passed on untransformed.
    ///
    /// \throws corrupted_stream_error if the data was malformed.
    //* =====================================================================
    void operator()(telnetpp::bytes data, continuation const &cont);

private:
    //* =====================================================================
    /// \brief A hook for when the transformation stream starts.
    //* =====================================================================
    virtual void do_start() {};

    //* =====================================================================
    /// \brief A hook for when transformation stream ends.
    //* =====================================================================
    virtual void do_finish(continuation const & /*cont*/) {};

    //* =====================================================================
    /// \brief Transform the given bytes, sending the transformed data
    /// to the continuation, along with a boolean indicating whether the
    /// transformation stream was ended inline (e.g. a compression stream
    /// itself might indicate that compression has ended).
    ///
    /// \returns a subsequence of the bytes that were not transformed due to
    /// e.g. the stream ending.
    ///
    /// \throws telnetpp::options::mccp::corrupted_stream_error if the data
    /// was malformed.
    //* =====================================================================
    virtual telnetpp::bytes transform_chunk(
        telnetpp::bytes data, continuation const &continuation) = 0;

    bool engaged_{false};
};

//* =========================================================================
/// \brief An exception that is thrown in the case that a stream of data
/// cannot be decompressed.
//* =========================================================================
class TELNETPP_EXPORT corrupted_stream_error : virtual std::domain_error,
                                               virtual boost::exception
{
public:
    //* =====================================================================
    /// Constructor
    //* =====================================================================
    explicit corrupted_stream_error(char const *what_arg);
};

}  // namespace telnetpp::options::mccp
