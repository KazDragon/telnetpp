#pragma once

#include "telnetpp/options/mccp/codec.hpp"
#include <memory>

namespace telnetpp { namespace options { namespace mccp { namespace zlib {

//* =========================================================================
/// \brief Represents an object that can decompress arbitrary byte sequences.
//* =========================================================================
class TELNETPP_EXPORT decompressor
  : public telnetpp::options::mccp::codec
{
public:
    //* =====================================================================
    /// \brief Constructor
    //* =====================================================================
    decompressor();

    //* =====================================================================
    /// \brief Destructor
    //* =====================================================================
    ~decompressor() override;

private:
    //* =====================================================================
    /// \brief A hook for when the transformation stream starts.
    //* =====================================================================
    void do_start() override;

    //* =====================================================================
    /// \brief A hook for when transformation stream ends.
    //* =====================================================================
    void do_finish(continuation const &cont) override;

    //* =====================================================================
    /// \brief Transform the given bytes, sending the transformed data
    /// to the continuation, along with a boolean indicating whether the
    /// transformation stream was ended inline (e.g. a compression stream
    /// itself might indicate that compression has ended).
    ///
    /// \returns a subsequence of the bytes that were not transformed due to
    /// e.g. the stream ending.
    ///
    /// \throws telnetpp::options::mccp::corrupted_stream_error if the data
    /// was malformed.
    //* =====================================================================
    telnetpp::bytes transform_chunk(
      telnetpp::bytes data,
      continuation const &cont) override;

    class impl;
    std::unique_ptr<impl> pimpl_;
};

}}}}
