#pragma once

#include "telnetpp/element.hpp"

namespace telnetpp::detail {

//* =========================================================================
/// \exclude
//* =========================================================================
template <class Continuation>
constexpr void generate_escaped(telnetpp::bytes data, Continuation &&cont)
{
  telnetpp::bytes::iterator begin = data.begin();
  telnetpp::bytes::iterator current = data.begin();
  telnetpp::bytes::iterator end = data.end();

  // If we come across an 0xFF byte in the data, then it must be repeated.
  // We do this by splitting the span into two, one of which ends with the
  // 0xFF byte and the second that begins with it.  In this way, the byte is
  // duplicated without requiring any extra allocations.
  while (current != end)
  {
    if (*current == telnetpp::iac)
    {
      cont({begin, current + 1});
      begin = current;
    }

    ++current;
  }

  if (begin != end)
  {
    cont({begin, end});
  }
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <class Continuation>
constexpr void generate_command(telnetpp::command cmd, Continuation &&cont)
{
  telnetpp::byte const data[] = {telnetpp::iac, cmd.value()};

  cont(data);
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <class Continuation>
constexpr void generate_negotiation(
    telnetpp::negotiation neg, Continuation &&cont)
{
  telnetpp::byte const data[] = {
      telnetpp::iac, neg.request(), neg.option_code()};

  cont(data);
}

//* =========================================================================
/// \exclude
//* =========================================================================
template <class Continuation>
constexpr void generate_subnegotiation(
    telnetpp::subnegotiation sub, Continuation &&cont)
{
  telnetpp::byte const preamble[] = {telnetpp::iac, telnetpp::sb, sub.option()};

  constexpr telnetpp::byte const postamble[] = {telnetpp::iac, telnetpp::se};

  cont(preamble);
  generate_escaped(sub.content(), cont);
  cont(postamble);
}

}  // namespace telnetpp::detail
