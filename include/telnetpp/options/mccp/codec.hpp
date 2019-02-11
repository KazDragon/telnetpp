#pragma once

#include "telnetpp/element.hpp"
#include <boost/optional.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <vector>

namespace telnetpp { namespace options { namespace mccp {

class compressor;
class decompressor;

//* =========================================================================
/// \brief A class responsible for compressing and decompressing data for the
/// MCCP server option.
///
/// In the mccp::codec class, control flow is managed by interleaving
/// certain tags within the data stream.
///
/// In particular, the begin_compression tag will cause all data from the
/// next token to be sent in a compressed format.  The end_compression tag
/// ends this process.
///
/// Additionally, sending a begin_decompression tag will cause all data from
/// the next token to be received in a compressed format, and will be returned
/// in a decompressed format.  This can be cancelled by either sending an
/// end_decompression tag, or if the compressed stream is marked as finished.
///
/// Actual compression and decompression is handled by the compressor and
/// decompressor objects passed in during construction.
//* =========================================================================
class TELNETPP_EXPORT codec final
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    constexpr codec(compressor &co, decompressor &dec) noexcept
      : compressor_(co),
        decompressor_(dec)
    {
    }

    //* =====================================================================
    /// \brief In a manner consistent with the rest of the telnetpp library,
    /// interprets the stream and returns a similar stream that is either
    /// compressed or uncompressed, as appropriate, that can be sent to the
    /// next lower layer.
    //* =====================================================================
    template <class Continuation>
    constexpr void send(telnetpp::bytes data, Continuation &&cont)
    {
        cont(data);
    }
    
    // std::vector<telnetpp::stream_token> send(
    //    std::vector<telnetpp::stream_token> const &tokens);

    //* =====================================================================
    /// \brief Receive a byte from the lower layer, decompress it if
    /// necessary, and return the result as a stream of bytes.
    //* =====================================================================
    // telnetpp::byte_stream receive(byte data);

private:
    codec(codec const &) = delete;
    codec &operator=(codec const &) = delete;

    compressor &compressor_;
    decompressor &decompressor_;
    
    bool output_compressed_ = false;
    bool input_compressed_  = false;
};

}}}
