#pragma once

namespace telnetpp { namespace options { namespace mccp {

//* =========================================================================
/// \brief A token passed down (in the boost::any part of the token stream)
/// to indicate that an MCCP negotiation is in progress and any output from
/// the stream must be halted until the negotiation is resolved.  Otherwise,
/// there is a risk of sending compressed data where uncompressed data was
/// expected, or vice versa.
//* =========================================================================
struct block_token
{
};

//* =========================================================================
/// \brief A token passed down (in the boost::any part of the token stream)
/// to indicate that the data stream can be resumed if it were halted, and
/// that further data should be sent in a compressed format.
//* =========================================================================
struct resume_compressed_token
{
};

//* =========================================================================
/// \brief A token passed down (in the boost::any part of the token stream)
/// to indicate that the data stream can be resumed if it were halted, and
/// that further data should be sent in an uncompressed format.
//* =========================================================================
struct resume_uncompressed_token
{
};

}}}
