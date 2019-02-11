#pragma once

#include "telnetpp/options/mccp/decompressor.hpp"
#include <memory>

namespace telnetpp { namespace options { namespace mccp { namespace zlib {

//* =========================================================================
/// \brief Represents an object that can decompress arbitrary byte sequences.
//* =========================================================================
class TELNETPP_EXPORT decompressor
  : public telnetpp::options::mccp::decompressor
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    decompressor();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~decompressor() override;

    //* =====================================================================
    /// \brief Ends the current decompression stream.  Any further calls
    /// to decompress continue as if the stream were created fresh.
    //* =====================================================================
    //void end_decompression() override;

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
      continuation const &cont) override;

    class impl;
    std::unique_ptr<impl> pimpl_;
};

}}}}
