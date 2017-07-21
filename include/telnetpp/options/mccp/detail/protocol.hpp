#pragma once

#include "telnetpp/core.hpp"

//* =========================================================================
/// \namespace telnetpp::options::mccp
/// \brief An implementation of the Mud Client Compression Protocol
/// \par Overview
/// MCCP is used to control whether data should be transmitted as compressed
/// or uncompressed data.  A telnetpp::options::mccp::server controls
/// compression of an output stream, and telnetpp::options::mccp::client
/// controls compression of an input stream.
/// \par Codec
/// This implementation of the MCCP comprises two distinct parts: the
/// client/server pair, and the codec.  Where the client/server pair are
/// used to control when compression/decompression should happen, it is the
/// codec that actually performs the compression and decompression.  It does
/// this with the aid of a compressor and a decompressor object.  In
/// principle, any API that conforms to the ZLIB compressed data format can
/// be used to perform the compression/decompression functions.  Telnet++
/// bundles compressors for the common zlib library "libz".
/// \par Usage - Server
/// Assuming you want to use an MCCP server in order to send compressed data,
/// you first need to instantiate your server and codec objects:
/// \code
///     telnetpp::options::mccp::server mccp_server;
///
///     telnetpp::options::mccp::codec mccp_codec(
///         std::make_shared<telnetpp::options::mccp::zlib::compressor>(),
///         std::make_shared<telnetpp::options::mccp::zlib::decompressor>());
/// \endcode
/// Install the server into your telnet session.
/// \code
///     telnet_session.install(mccp_server);
/// \endcode
/// Run through the normal negotiations to make the MCCP server active.  To
/// begin compression, ensure that you call begin_compression() on the server
/// and write the result to your lower layer
/// \code
///     my_lower_layer_write(telnet_session.send(
///         mccp_server.begin_compression()));
/// \endcode
/// Finally, ensure that your lower layer write function passes the data
/// through the codec.
/// \code
///     void my_lower_layer_write(std::vector<telnetpp::stream_token> const &d)
///     {
///         static telnetpp::byte_converter byte_converter;
///         // Compress (if necessary) the data.
///         auto const &compressed_data = mccp_codec.send(d);
///         // Flatten the stream_tokens to a single byte_stream.
///         auto const &stream = byte_converter.send(compressed_data);
///         // Output the byte_stream to your socket (or whatever).
///         socket_->write({stream.begin(), stream.end()});
///     }
/// \endcode
/// \par Usage - Client
/// If you want to use the client to ensure that data is decompressed,
/// then the implementation is very similar.  Of particular note is that
/// the codec must still be in my_lower_layer_write, and also in
/// my_lower_layer_read.
/// \see http://tintin.sourceforge.net/mccp/
/// \see https://www.ietf.org/rfc/rfc1950.txt
//* =========================================================================

namespace telnetpp { namespace options { namespace mccp { namespace detail {

static constexpr option_type option = 86;

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

//* =========================================================================
/// \brief A token passed down (in the boost::any part of the token stream)
/// to indicate that the data stream should not be decompressed from the next
/// token onward.
//* =========================================================================
struct end_decompression
{
};


}}}}
