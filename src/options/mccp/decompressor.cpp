#include "telnetpp/options/mccp/decompressor.hpp"

namespace telnetpp { namespace options { namespace mccp {

// ==========================================================================
// CORRUPTED_STREAM_ERROR::CONSTRUCTOR
// ==========================================================================
corrupted_stream_error::corrupted_stream_error(std::string const &what_arg)
  : domain_error(what_arg)
{
}
  
// ==========================================================================
// CORRUPTED_STREAM_ERROR::CONSTRUCTOR
// ==========================================================================
corrupted_stream_error::corrupted_stream_error(char const *what_arg)
  : domain_error(what_arg)
{
}
  
}}}
