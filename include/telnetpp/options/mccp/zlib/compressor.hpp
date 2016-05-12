#pragma once

#include "telnetpp/options/mccp/compressor.hpp"
#include <memory>

namespace telnetpp { namespace options { namespace mccp { namespace zlib {

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
    telnetpp::u8stream compress(telnetpp::u8stream const &sequence) override;
    
    //* =====================================================================
    /// \brief Ends the compression stream.  Compressing further will cause
    /// the stream to restart.
    //* =====================================================================
    telnetpp::u8stream end_compression() override;

private :
    struct impl;
    std::unique_ptr<impl> pimpl_;
};

}}}}
