#pragma once

namespace telnetpp::detail {

template <class T>
struct return_default_constructed
{
    constexpr T operator()()
    {
        return T{};
    }
};

template <>
struct return_default_constructed<void>
{
    constexpr void operator()()
    {
    }
};

}  // namespace telnetpp::detail
