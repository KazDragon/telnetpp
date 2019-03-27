#include <telnetpp/options/mccp/compressor.hpp>

class fake_compressor : public telnetpp::options::mccp::compressor
{
public:
    //* =====================================================================
    /// \brief Compress the given byte sequence and return the compressed
    /// sequence.
    //* =====================================================================
    telnetpp::byte_stream compress(telnetpp::byte_stream const &sequence) override
    {
        ++compress_called;
        return compress_result;
    }
    
    //* =====================================================================
    /// \brief Ends the compression stream.  Compressing further will cause
    /// the stream to restart.
    //* =====================================================================
    telnetpp::byte_stream end_compression() override
    {
        ++end_compression_called;
        return end_compression_result;
    }
    
    telnetpp::byte_stream compress_result;
    telnetpp::byte_stream end_compression_result;
    
    size_t compress_called = 0;
    size_t end_compression_called = 0;
    
};
