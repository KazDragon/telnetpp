#pragma once

#include "telnetpp/client_option.hpp"
#include "telnetpp/options/new_environ/detail/protocol.hpp"
#include "telnetpp/options/new_environ/detail/stream.hpp"
#include <numeric>
#include <string>

namespace telnetpp { namespace options { namespace new_environ {

//* =========================================================================
/// \brief An enumeration of the type of variables that NEW_ENVIRON handles.
//* =========================================================================
enum class variable_type
{
    var,
    uservar
};

//* =========================================================================
/// \brief A request that is made of the remote server.
//* =========================================================================
struct request
{
    variable_type          type;
    telnetpp::byte_storage name;
};

using requests = gsl::span<request const>;

//* =========================================================================
/// \brief A response that is received from the remote server.
//* =========================================================================
struct response
{
    variable_type                           type;
    telnetpp::byte_storage                  name;
    boost::optional<telnetpp::byte_storage> value;
};

using responses = gsl::span<response const>;

//* =========================================================================
/// \brief An implementation of the client side of the Telnet New-Environ
/// option.
//* =========================================================================
class TELNETPP_EXPORT client 
  : public telnetpp::client_option
{
public:
    //* =====================================================================
    /// CONSTRUCTOR
    //* =====================================================================
    client();

    //* =====================================================================
    /// \brief Requests that a particular set of environment variables be
    /// transmitted by the client.
    //* =====================================================================
    template <typename Continuation>
    void request_variables(requests const &reqs, Continuation &&cont)
    {
        auto request_content = std::accumulate(
            reqs.begin(),
            reqs.end(),
            byte_storage{detail::send},
            [](byte_storage &content, request const &req) -> byte_storage &
            {
                content.push_back(type_to_byte(req.type));
                detail::append_escaped(content, req.name);
                return content;
            });

        cont(telnetpp::subnegotiation{code(), request_content});
    }

    //* =====================================================================
    /// \brief Signal called whenever an environment variable is updated.
    /// \param rsp the response the response from the remote
    /// \param cont a continuation to pass any Telnet response that may
    ///        occur as a result of receiving this response.
    //* =====================================================================
    boost::signals2::signal<
        void (response const &rsp, continuation const &cont)
    > on_variable_changed;

private:
    //* =====================================================================
    /// \brief Called when a subnegotiation is received while the option is
    /// active.  Override for option-specific functionality.
    //* =====================================================================
    void handle_subnegotiation(
        telnetpp::bytes data,
        continuation const &cont) override;

    //* =====================================================================
    /// \brief Converts a variable type to its byte representation.
    //* =====================================================================
    static constexpr telnetpp::byte type_to_byte(variable_type type)
    {
         return type == variable_type::var
              ? telnetpp::options::new_environ::detail::var
              : telnetpp::options::new_environ::detail::uservar;
    }

};

}}}
