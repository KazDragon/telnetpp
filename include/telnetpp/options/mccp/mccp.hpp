#pragma once

namespace telnetpp { namespace options { namespace mccp {

//* =========================================================================
/// \brief A token passed down (in the boost::any part of the token stream)
/// to indicate that the data stream should be compressed from the next
/// token onward.
//* =========================================================================
struct begin_compression
{
};

//* =========================================================================
/// \brief A token passed down (in the boost::any part of the token stream)
/// to indicate that the data stream should not be compressed from the next
/// token onward.
//* =========================================================================
struct end_compression
{
};

//* =========================================================================
/// \brief A token passed down (in the boost::any part of the token stream)
/// to indicate that the data stream should be decompressed from the next
/// token onward.
//* =========================================================================
struct begin_decompression
{
};

}}}
