#pragma once
#include "telnetpp/element.hpp"

namespace telnetpp {

//* =========================================================================
/// \brief A class that can convert from a
/// std::vector<telnetpp::stream_token> to a telnetpp::byte_stream.
/// \par
/// Use this class at the bottom of your protocol stack (after compression,
/// if you are using that) to collapse the output from the vector of
/// stream_tokens to a straightforward byte_stream.  Any Boost.Any instances
/// in the input will be discarded since they should all have been consumed
/// by now anyway.
//* =========================================================================
class TELNETPP_EXPORT byte_converter
{
public :
    //* =====================================================================
    /// \brief Convert the tokens into a stream of bytes.
    /// Takes the input set of tokens and converts them into a single array
    /// of bytes, which can be transmitted to a remote target.  Any non-
    /// byte_stream elements are discarded.
    //* =====================================================================
    telnetpp::byte_stream send(
        std::vector<telnetpp::stream_token> const &tokens);
};

}
