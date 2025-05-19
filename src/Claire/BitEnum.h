// Thank you Brian Gianforcaro
#pragma once
#include <type_traits>

#define BIT_ENUM(enum) \
    constexpr enum operator|(enum lhs, enum rhs) { \
        using type = std::underlying_type_t<enum>; \
        return static_cast<enum>(static_cast<type>(lhs) | static_cast<type>(rhs)); \
    } \
    \
    constexpr enum operator&(enum lhs, enum rhs) { \
        using type = std::underlying_type_t<enum>; \
        return static_cast<enum>(static_cast<type>(lhs) & static_cast<type>(rhs)); \
    }
