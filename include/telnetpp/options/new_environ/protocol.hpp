#pragma once

#include "telnetpp/core.hpp"

#include <optional>

namespace telnetpp::options::new_environ {

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
    variable_type type;
    telnetpp::byte_storage name;
};

using requests = std::span<request const>;

//* =========================================================================
/// \brief A response that is received from the remote server.
//* =========================================================================
struct response
{
    variable_type type;
    telnetpp::byte_storage name;
    std::optional<telnetpp::byte_storage> value;
};

using responses = std::span<response const>;

}  // namespace telnetpp::options::new_environ
