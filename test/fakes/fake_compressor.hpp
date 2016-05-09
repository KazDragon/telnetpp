#include <telnetpp/options/mccp/compressor.hpp>

class fake_compressor : public telnetpp::options::mccp::compressor
{
public :
    //* =====================================================================
    /// \brief Compress the given byte sequence and return the compressed
    /// sequence.
    //* =====================================================================
    telnetpp::u8stream compress(telnetpp::u8stream const &sequence) override
    {
        ++compress_called;
        return compress_result;
    }
    
    //* =====================================================================
    /// \brief Ends the compression stream.  Compressing further will cause
    /// the stream to restart.
    //* =====================================================================
    telnetpp::u8stream end_compression() override
    {
        ++end_compression_called;
        return end_compression_result;
    }
    
    telnetpp::u8stream compress_result;
    telnetpp::u8stream end_compression_result;
    
    size_t compress_called = 0;
    size_t end_compression_called = 0;
    
};
