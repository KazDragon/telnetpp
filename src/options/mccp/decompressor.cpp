#include "telnetpp/options/mccp/decompressor.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// BEGIN_DECOMPRESSION
// ==========================================================================
void decompressor::begin_decompression()
{
    engaged_ = true;
    do_begin_decompression();
}

// ==========================================================================
// END_DECOMPRESSION
// ==========================================================================
void decompressor::end_decompression()
{
    engaged_ = false; 
    do_end_decompression();
}

// ==========================================================================
// OPERATOR()
// ==========================================================================
void decompressor::operator()(telnetpp::bytes data, continuation const &cont)
{
    while (!data.empty())
    {
        if (engaged_)
        {
            data = decompress_chunk(
                data,
                [&](telnetpp::bytes decompressed_data, bool decompression_ended)
                {
                    engaged_ = !decompression_ended;
                    cont(decompressed_data, decompression_ended);
                });
        }
        else
        {
            // Every byte might activate decompression, so they must be
            // transmitted individually.
            cont(data.subspan(0, 1), false);
            data = data.subspan(1);
        }
    }
}

// ==========================================================================
// CORRUPTED_STREAM_ERROR::CONSTRUCTOR
// ==========================================================================
corrupted_stream_error::corrupted_stream_error(char const *what_arg)
  : domain_error(what_arg)
{
}

}}}
