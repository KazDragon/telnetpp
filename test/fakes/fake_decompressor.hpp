#include <telnetpp/options/mccp/decompressor.hpp>

class fake_decompressor : public telnetpp::options::mccp::decompressor
{
public :
    //* =====================================================================
    /// \brief Decompress the given byte, and return a tuple of the
    /// decompressed data and a boolean that is set to true if this was the
    /// end of the decompression stream.
    //* =====================================================================
    std::tuple<telnetpp::u8stream, bool> decompress(
        telnetpp::u8 byte) override
    {
        ++decompress_called;
        return {};
    }
    
    size_t decompress_called;
};
