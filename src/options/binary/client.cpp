#include "telnetpp/options/binary/client.hpp"

namespace telnetpp::options::binary {

client::client(telnetpp::session &sess) noexcept : basic_client(sess)
{
}

}  // namespace telnetpp::options::binary
