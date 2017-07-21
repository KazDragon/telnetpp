#pragma once

#include "telnetpp/options/mccp/compressor.hpp"
#include <memory>

//* =========================================================================
/// \namespace telnetpp::options::mccp::zlib
/// \brief Implementation of the compressor/decompressor functionality for
/// use with a telnetpp::options::mccp::codec.
//* =========================================================================
namespace telnetpp { namespace options { namespace mccp { namespace zlib {

//* =========================================================================
/// \brief Represents an object that can compress arbitrary byte sequences.
//* =========================================================================
class TELNETPP_EXPORT compressor : public telnetpp::options::mccp::compressor
{
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    compressor();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~compressor() override;

    //* =====================================================================
    /// \brief Compress the given byte sequence and return the compressed
    /// sequence.
    //* =====================================================================
    telnetpp::byte_stream compress(
        telnetpp::byte_stream const &sequence) override;

    //* =====================================================================
    /// \brief Ends the compression stream.  Compressing further will cause
    /// the stream to restart.
    //* =====================================================================
    telnetpp::byte_stream end_compression() override;

private :
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

}}}}
