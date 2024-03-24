#pragma once

#include <functional>
#include <type_traits>
#include <utility>

namespace telnetpp::detail {

constexpr void hash_combine(std::size_t & /*seed*/)
{
}

template <typename Field, typename... Fields>
constexpr void hash_combine(
    std::size_t &seed, Field &&field, Fields &&...fields)
{
    using field_hash_type = std::remove_const_t<std::remove_reference_t<Field>>;

    std::hash<field_hash_type> hasher;
    seed ^= hasher(field) + 0x9E3779B9 + (seed << 6) + (seed >> 2);
    hash_combine(seed, std::forward<Fields>(fields)...);
}

}  // namespace telnetpp::detail

#define TELNETPP_MAKE_HASHABLE(type, ...)                     \
    namespace std {                                           \
    template <>                                               \
    struct hash<type>                                         \
    {                                                         \
        std::size_t operator()(const type &t) const           \
        {                                                     \
            std::size_t ret = 0;                              \
            telnetpp::detail::hash_combine(ret, __VA_ARGS__); \
            return ret;                                       \
        }                                                     \
    };                                                        \
    }

#define TELNETPP_MAKE_INTRUSIVE_HASH(type)          \
    namespace std {                                 \
    template <>                                     \
    struct hash<type>                               \
    {                                               \
        std::size_t operator()(const type &t) const \
        {                                           \
            std::size_t ret = 0;                    \
            t.hash_combine(ret);                    \
            return ret;                             \
        }                                           \
    };                                              \
    }
