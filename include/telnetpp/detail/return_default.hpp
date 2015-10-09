#ifndef TELNETPP_DETAIL_RETURN_DEFAULT_HPP_
#define TELNETPP_DETAIL_RETURN_DEFAULT_HPP_

namespace telnetpp { namespace detail {
    
template <class T>
struct return_default_constructed;

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

}}

#endif
