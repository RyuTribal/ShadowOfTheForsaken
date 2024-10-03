#pragma once

#include "Engine/Core/Base.h"
#include "Engine/Core/Log.h"

#ifdef PLATFORM_WINDOWS
#define SOF_DEBUG_BREAK __debugbreak()
#elif defined(SOF_COMPILER_CLANG)
#define SOF_DEBUG_BREAK __builtin_debugtrap()
#else
#define SOF_DEBUG_BREAK
#endif

#ifdef DEBUG
#define SOF_ENABLE_ASSERTS
#endif

#define SOF_ENABLE_VERIFY

#ifdef SOF_ENABLE_ASSERTS
#ifdef SOF_COMPILER_CLANG
#define SOF_ASSERT_MESSAGE_INTERNAL(...) ::SOF::Log::PrintAssertMessage("Assertion Failed", ##__VA_ARGS__)
#define SOF_ASSERT_MESSAGE_INTERNAL(...) ::SOF::Log::PrintAssertMessage("Assertion Failed", ##__VA_ARGS__)
#else
#define SOF_ASSERT_MESSAGE_INTERNAL(...) ::SOF::Log::PrintAssertMessage("Assertion Failed" __VA_OPT__(, ) __VA_ARGS__)
#define SOF_ASSERT_MESSAGE_INTERNAL(...) ::SOF::Log::PrintAssertMessage("Assertion Failed" __VA_OPT__(, ) __VA_ARGS__)
#endif

#define SOF_ASSERT(condition, ...)                    \
    {                                                 \
        if (!(condition)) {                           \
            SOF_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); \
            SOF_DEBUG_BREAK;                          \
        }                                             \
    }
#define SOF_ASSERT(condition, ...)                    \
    {                                                 \
        if (!(condition)) {                           \
            SOF_ASSERT_MESSAGE_INTERNAL(__VA_ARGS__); \
            SOF_DEBUG_BREAK;                          \
        }                                             \
    }
#else
#define SOF_ASSERT(condition, ...)
#define SOF_ASSERT(condition, ...)
#endif

#ifdef SOF_ENABLE_VERIFY
#ifdef SOF_COMPILER_CLANG
#define SOF_VERIFY_MESSAGE_INTERNAL(...) ::SOF::Log::PrintAssertMessage("Verify Failed", ##__VA_ARGS__)
#define SOF_VERIFY_MESSAGE_INTERNAL(...) ::SOF::Log::PrintAssertMessage("Verify Failed", ##__VA_ARGS__)
#else
#define SOF_VERIFY_MESSAGE_INTERNAL(...) ::SOF::Log::PrintAssertMessage("Verify Failed" __VA_OPT__(, ) __VA_ARGS__)
#define SOF_VERIFY_MESSAGE_INTERNAL(...) ::SOF::Log::PrintAssertMessage("Verify Failed" __VA_OPT__(, ) __VA_ARGS__)
#endif

#define SOF_VERIFY(condition, ...)                    \
    {                                                 \
        if (!(condition)) {                           \
            SOF_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); \
            SOF_DEBUG_BREAK;                          \
        }                                             \
    }
#define SOF_VERIFY(condition, ...)                    \
    {                                                 \
        if (!(condition)) {                           \
            SOF_VERIFY_MESSAGE_INTERNAL(__VA_ARGS__); \
            SOF_DEBUG_BREAK;                          \
        }                                             \
    }
#else
#define SOF_VERIFY(condition, ...)
#define SOF_VERIFY(condition, ...)
#endif
