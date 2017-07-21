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
public :
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    decompressor();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~decompressor() override;

    //* =====================================================================
    /// \brief Decompress the given byte, and return a tuple of the
    /// decompressed data and a boolean that is set to true if this was the
    /// end of the decompression stream.
    //* =====================================================================
    std::tuple<telnetpp::byte_stream, bool> decompress(byte data) override;

    //* =====================================================================
    /// \brief Ends the current decompression stream.  Any further calls
    /// to decompress continue as if the stream were created fresh.
    //* =====================================================================
    void end_decompression() override;

private :
    class impl;
    std::unique_ptr<impl> pimpl_;
};

}}}}
