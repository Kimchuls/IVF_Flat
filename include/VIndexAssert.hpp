#ifndef VINDEX_ASSERT_HPP
#define VINDEX_ASSERT_HPP
#include <cstdio>
#include <cstring>
#include "VIndexException.hpp"

#define VINDEX_THROW_FMT(FMT, ...)                           \
    do                                                       \
    {                                                        \
        std::string __s;                                     \
        int __size = snprintf(nullptr, 0, FMT, __VA_ARGS__); \
        __s.resize(__size + 1);                              \
        snprintf(&__s[0], __s.size(), FMT, __VA_ARGS__);     \
        throw vindex::VINDEXException(                       \
            __s, __PRETTY_FUNCTION__, __FILE__, __LINE__);   \
    } while (false)

#define VINDEX_THROW_IF_NOT_MSG(X, MSG)                       \
    do                                                        \
    {                                                         \
        if (!(X))                                             \
        {                                                     \
            VINDEX_THROW_FMT("Error: '%s' failed: " MSG, #X); \
        }                                                     \
    } while (false)

#endif