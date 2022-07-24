#include <telnetpp/session.hpp>
#include <telnetpp/options/terminal_type/client.hpp>

struct channel
{
    void write(telnetpp::bytes) {}
    void async_read(std::function<void (telnetpp::bytes)> const &) {}
    bool is_alive() { return true; }
    void close() {}
};

int main()
{
    channel ch;
    telnetpp::session session{ch};
    telnetpp::options::terminal_type::client client{session};
    session.install(client);
    client.activate();
}
