#pragma once

#include <functional>
#include <type_traits>
#include <utility>

namespace telnetpp { namespace detail {
    
constexpr void hash_combine(std::size_t &/*seed*/)
{
}

template <typename Field, typename... Fields>
constexpr void hash_combine(
    std::size_t &seed, 
    Field &&field,
    Fields &&...fields)
{
    using field_hash_type = 
        typename std::remove_const<
            typename std::remove_reference<Field>::type
        >::type;
        
    std::hash<field_hash_type> hasher;
    seed ^= hasher(field) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, std::forward<Fields>(fields)...);
}

}}

#define TELNETPP_MAKE_HASHABLE(type, ...) \
    namespace std {\
        template<> struct hash<type> {\
            std::size_t operator()(const type &t) const {\
                std::size_t ret = 0;\
                telnetpp::detail::hash_combine(ret, __VA_ARGS__);\
                return ret;\
            }\
        };\
    }
    
#define TELNETPP_MAKE_INTRUSIVE_HASH(type) \
    namespace std {\
        template<> struct hash<type> {\
            std::size_t operator()(const type &t) const {\
                std::size_t ret = 0;\
                t.hash_combine(ret);\
                return ret;\
            }\
        };\
    }
