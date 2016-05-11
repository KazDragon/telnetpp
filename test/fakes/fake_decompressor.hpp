#include <telnetpp/options/mccp/decompressor.hpp>
#include <deque>

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

        if (!decompress_result.empty())
        {
            auto result = decompress_result[0];
            decompress_result.pop_front();
            return result;
        }
        else
        {
            return {};
        }
    }

    //* =====================================================================
    /// \brief Ends the current decompression stream.  Any further calls
    /// to decompress continue as if the stream were created fresh.
    //* =====================================================================
    virtual void end_decompression()
    {
        ++end_decompression_called;
    }

    std::deque<std::tuple<telnetpp::u8stream, bool>> decompress_result;

    size_t decompress_called = 0;
    size_t end_decompression_called = 0;
};
