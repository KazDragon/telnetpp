#pragma once

#include <serverpp/core.hpp>
#include <functional>
#include <memory>

namespace serverpp {
class tcp_socket;
}

namespace rot13 {

//* =========================================================================
/// \brief An connection to a socket that abstracts away details about the
/// protocols used.
/// \par
/// In particular, upon creation, the connection immediately begins
/// negotiation of four options:
///  * Echo - which stops the connecting terminal from echoing its own input
///  * Suppress Go-Ahead - which, in combination with Echo, causes the
///    terminal to enter "character-at-a-time mode", meaning that input is
///    sent per keystroke rather than per line.
///  * Negotiate About Window Size - which causes the terminal to send
///    messages about its window size.
///  * Terminal Type - which can be used to interrogate the terminal about
///    which terminal it actually is.
/// \par
/// These options are handled transparently with normal operation of the
/// connection.
//* =========================================================================
class connection  // NOLINT
{
 public:
  //* =====================================================================
  /// \brief Create a connection object that uses the passed socket as
  /// a communications point, and calls the passed function whenever data
  /// is received.
  //* =====================================================================
  explicit connection(serverpp::tcp_socket &&socket);

  //* =====================================================================
  /// \brief Move constructor
  //* =====================================================================
  connection(connection &&other) noexcept;

  //* =====================================================================
  /// \brief Destructor.
  //* =====================================================================
  ~connection();

  //* =====================================================================
  /// \brief Move assignment
  //* =====================================================================
  connection &operator=(connection &&other) noexcept;

  //* =====================================================================
  /// \brief Returns whether the endpoint of the connection is still
  ///        alive.
  //* =====================================================================
  [[nodiscard]] bool is_alive() const;

  //* =====================================================================
  /// \brief Closes the connection.
  //* =====================================================================
  void close();

  //* =====================================================================
  /// \brief Asynchronously reads from the connection.
  ///
  /// A single read may yield zero or more callbacks to the data
  /// continuation.  This is because parts or all of the data may be
  /// consumed by Telnet handling.  Therefore, a second continuation is
  /// provided to show that the requested read has been completed and a
  /// new read request may be issued.
  //* =====================================================================
  void async_read(
      std::function<void(serverpp::bytes)> const &data_continuation,
      std::function<void()> const &read_complete_continuation);

  //* =====================================================================
  /// \brief Writes to the connection.
  //* =====================================================================
  void write(serverpp::bytes data);

  //* =====================================================================
  /// \brief Requests terminal type of the connection, calling the
  ///        supplied continuation with the results.
  //* =====================================================================
  void async_get_terminal_type(
      std::function<void(std::string const &)> const &continuation);

  //* =====================================================================
  /// \brief Set a function to be called when the window size changes.
  //* =====================================================================
  void on_window_size_changed(
      std::function<void(std::uint16_t, std::uint16_t)> const &continuation);

 private:
  struct impl;
  std::unique_ptr<impl> pimpl_;
};

}  // namespace rot13
