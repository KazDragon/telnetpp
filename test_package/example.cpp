#include <telnetpp/session.hpp>
#include <telnetpp/options/terminal_type/client.hpp>

int main()
{
    telnetpp::options::terminal_type::client client;
    telnetpp::session session;
    session.install(client);
}
