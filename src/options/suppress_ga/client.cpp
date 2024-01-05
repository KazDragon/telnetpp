#include "telnetpp/options/suppress_ga/client.hpp"

namespace telnetpp::options::suppress_ga {

client::client(telnetpp::session &sess) noexcept : basic_client(sess)
{
}

}  // namespace telnetpp::options::suppress_ga
