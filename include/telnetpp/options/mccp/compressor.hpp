#pragma once

#include "telnetpp/element.hpp"
#include <boost/optional.hpp>
#include <boost/utility.hpp>
#include <memory>
#include <vector>

namespace telnetpp { namespace options { namespace mccp {

//* =========================================================================
/// \brief A class responsible for compressing data for the MCCP server
/// option.
///
/// In the mccp::compressor class, control flow is managed by interleaving
/// certain tags within the data stream.  
///
/// In particular, the blocked_token tag will cause the stream to buffer up
/// output data.  This can be used in the circumstance that it is unknown 
/// whether data should be compressed or not (e.g. IAC WILL MCCP has just
/// been sent, but no response has been received.)  Further, the
/// resume_compressed_token and resume_uncompressed_token tags can be used
/// to unblock the stream and send any data onward in either a compressed
/// or uncompressed fashion, respectively.
//* =========================================================================
class TELNETPP_EXPORT compressor : boost::noncopyable
{
public :
    compressor();
    ~compressor();

    //* =====================================================================
    /// \brief In a manner consistent with the rest of the telnetpp library,
    /// interprets the stream and returns a similar stream that is either
    /// compressed or uncompressed, as appropriate, that can be sent to the
    /// next lower layer.
    //* =====================================================================
    std::vector<telnetpp::stream_token> send(
        std::vector<telnetpp::stream_token> const &tokens);

    struct impl;

private :
    std::shared_ptr<impl> pimpl_;
};

}}}
