#include "telnetpp/options/mccp/codec.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// START
// ==========================================================================
void codec::start()
{
    engaged_ = true;
    do_start();
}

// ==========================================================================
// FINISH
// ==========================================================================
void codec::finish(continuation const &cont)
{
    engaged_ = false; 
    do_finish(cont);
}

// ==========================================================================
// OPERATOR()
// ==========================================================================
void codec::operator()(telnetpp::bytes data, continuation const &cont)
{
    while (!data.empty())
    {
        if (engaged_)
        {
            data = transform_chunk(
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
