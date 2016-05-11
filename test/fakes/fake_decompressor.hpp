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

    //* =====================================================================
    /// \brief Ends the current decompression stream.  Any further calls
    /// to decompress continue as if the stream were created fresh.
    //* =====================================================================
    virtual void end_decompression()
    {
        ++end_decompression_called;
    }

    size_t decompress_called = 0;
    size_t end_decompression_called = 0;
};
