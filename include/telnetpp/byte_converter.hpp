#pragma once
#include "telnetpp/element.hpp"

namespace telnetpp {

class TELNETPP_EXPORT byte_converter
{
public :
    //* =====================================================================
    /// \brief Convert the tokens into a stream of bytes.
    /// Takes the input set of tokens and converts them into a single array
    /// of bytes, which can be transmitted to a remote target.  Any non-
    /// u8stream elements are discarded.
    //* =====================================================================
    telnetpp::u8stream send(std::vector<telnetpp::stream_token> const &tokens);
};

}