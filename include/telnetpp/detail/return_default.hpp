#pragma once

namespace telnetpp::detail {

template <class T>
struct return_default_constructed
{
  T operator()()
  {
    return T{};
  }
};

template <>
struct return_default_constructed<void>
{
  void operator()()
  {
  }
};

}  // namespace telnetpp::detail
