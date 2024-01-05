#include "connection.hpp"
#include <serverpp/tcp_socket.hpp>
#include <telnetpp/options/echo/server.hpp>
#include <telnetpp/options/naws/client.hpp>
#include <telnetpp/options/suppress_ga/server.hpp>
#include <telnetpp/options/terminal_type/client.hpp>
#include <telnetpp/telnetpp.hpp>

namespace rot13 {

// ==========================================================================
// CONNECTION::IMPLEMENTATION STRUCTURE
// ==========================================================================
struct connection::impl
{
  // ======================================================================
  // CONSTRUCTOR
  // ======================================================================
  explicit impl(serverpp::tcp_socket &&socket) : socket_(std::move(socket))
  {
    // Set up handlers for Telnet options functionality:

    // When the window size of the client changes, announce it to a
    // listener
    telnet_naws_client_.on_window_size_changed.connect(
        [this](auto &&width, auto &&height)
        { this->on_window_size_changed(width, height); });

    // When the Terminal Type option becomes active, request the client's
    // terminal type.
    telnet_terminal_type_client_.on_state_changed.connect(
        [this]()
        {
          if (telnet_terminal_type_client_.active())
          {
            telnet_terminal_type_client_.request_terminal_type();
          }
        });

    // When we receive the client's terminal type, announce it to a
    // listener
    telnet_terminal_type_client_.on_terminal_type.connect(
        [this](auto &&type)
        {
          std::string user_type(type.begin(), type.end());
          this->on_terminal_type_detected(user_type);
        });

    // Install all of the options we support in the Telnet Session object.
    telnet_session_.install(telnet_echo_server_);
    telnet_session_.install(telnet_suppress_ga_server_);
    telnet_session_.install(telnet_naws_client_);
    telnet_session_.install(telnet_terminal_type_client_);

    // Activating ECHO stops the terminal from locally echoing what is
    // typed, and SUPPRESS-GA on top of that enables "character at a time"
    // mode, as opposed to line mode.
    telnet_echo_server_.activate();
    telnet_suppress_ga_server_.activate();

    // For fun, activate NAWS so that we get information about when the
    // terminal size changes, and also TERMINAL-TYPE so that we can see
    // what the client has used to connect.
    telnet_naws_client_.activate();
    telnet_terminal_type_client_.activate();
  }

  // ======================================================================
  // IS_ALIVE
  // ======================================================================
  [[nodiscard]] bool is_alive() const
  {
    return socket_.is_alive();
  }

  // ======================================================================
  // CLOSE
  // ======================================================================
  void close()
  {
    socket_.close();
  }

  // ======================================================================
  // RAW_WRITE
  // ======================================================================
  void raw_write(telnetpp::bytes data)
  {
    // Send unescaped data directly to the socket.
    this->socket_.write(data);
  }

  // ======================================================================
  // WRITE
  // ======================================================================
  void write(telnetpp::element const &data)
  {
    // Send general data via the Telnet Session so that Telnet control
    // characters can be escaped properly.
    telnet_session_.write(data);
  }

  // ======================================================================
  // ASYNC_READ
  // ======================================================================
  void async_read(
      std::function<void(serverpp::bytes)> const &data_continuation,
      std::function<void()> const &read_complete_continuation)
  {
    // Requests that we read some data from the telnet session,
    // Because Telnet contains both in- and out-of band data, one
    // read may invoke the data continuation several times.  The end
    // of the current read is indicated by sending us empty data.
    telnet_session_.async_read(
        [=](telnetpp::bytes data)
        {
          if (data.empty())
          {
            read_complete_continuation();
          }
          else
          {
            data_continuation(data);
          }
        });
  }

  // ======================================================================
  // ON_WINDOW_SIZE_CHANGED
  // ======================================================================
  void on_window_size_changed(std::uint16_t width, std::uint16_t height) const
  {
    if (on_window_size_changed_)
    {
      on_window_size_changed_(width, height);
    }
  }

  // ======================================================================
  // ON_TERMINAL_TYPE_DETECTED
  // ======================================================================
  void on_terminal_type_detected(std::string const &type)
  {
    terminal_type_ = type;
    announce_terminal_type();
  }

  // ======================================================================
  // ANNOUNCE_TERMINAL_TYPE
  // ======================================================================
  void announce_terminal_type()
  {
    for (auto const &callback : terminal_type_requests_)
    {
      callback(terminal_type_);
    }

    terminal_type_requests_.clear();
  }

  serverpp::tcp_socket socket_;

  telnetpp::session telnet_session_{socket_};
  telnetpp::options::echo::server telnet_echo_server_{telnet_session_};
  telnetpp::options::suppress_ga::server telnet_suppress_ga_server_{
      telnet_session_};
  telnetpp::options::naws::client telnet_naws_client_{telnet_session_};
  telnetpp::options::terminal_type::client telnet_terminal_type_client_{
      telnet_session_};

  std::function<void(std::uint16_t, std::uint16_t)> on_window_size_changed_;

  std::string terminal_type_;
  std::vector<std::function<void(std::string)>> terminal_type_requests_;
};

// ==========================================================================
// CONSTRUCTOR
// ==========================================================================
connection::connection(serverpp::tcp_socket &&new_socket)
  : pimpl_(std::make_unique<impl>(std::move(new_socket)))
{
}

// ==========================================================================
// MOVE CONSTRUCTOR
// ==========================================================================
connection::connection(connection &&other) noexcept = default;

// ==========================================================================
// DESTRUCTOR
// ==========================================================================
connection::~connection() = default;

// ==========================================================================
// MOVE ASSIGNMENT
// ==========================================================================
connection &connection::operator=(connection &&other) noexcept = default;

// ==========================================================================
// IS_ALIVE
// ==========================================================================
bool connection::is_alive() const
{
  return pimpl_->is_alive();
}

// ==========================================================================
// CLOSE
// ==========================================================================
void connection::close()
{
  pimpl_->close();
}

// ==========================================================================
// ASYNC_READ
// ==========================================================================
void connection::async_read(
    std::function<void(serverpp::bytes)> const &data_continuation,
    std::function<void()> const &read_complete_continuation)
{
  pimpl_->async_read(data_continuation, read_complete_continuation);
}

// ==========================================================================
// WRITE
// ==========================================================================
void connection::write(serverpp::bytes data)
{
  pimpl_->write(data);
}

// ==========================================================================
// ASYNC_GET_TERMINAL_TYPE
// ==========================================================================
void connection::async_get_terminal_type(
    std::function<void(std::string const &)> const &continuation)
{
  pimpl_->terminal_type_requests_.emplace_back(continuation);
}

// ==========================================================================
// ON_WINDOW_SIZE_CHANGED
// ==========================================================================
void connection::on_window_size_changed(
    std::function<void(std::uint16_t, std::uint16_t)> const &continuation)
{
  pimpl_->on_window_size_changed_ = continuation;
}

}  // namespace rot13
