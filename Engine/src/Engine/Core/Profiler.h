#pragma once

#ifdef DEBUG
#define SOF_ENABLE_PROFILING
#endif

#ifdef SOF_ENABLE_PROFILING
#include <tracy/Tracy.hpp>
#endif

#ifdef SOF_ENABLE_PROFILING
#define SOF_PROFILE_MARK_FRAME FrameMark;
// NOTE: Use SOF_PROFILE_FUNC ONLY at the top of a function
//				Use SOF_PROFILE_SCOPE / SOF_PROFILE_SCOPE_DYNAMIC for an inner scope
#define SOF_PROFILE_FUNC(...) ZoneScoped##__VA_OPT__(N(__VA_ARGS__))
#define SOF_PROFILE_SCOPE(...) SOF_PROFILE_FUNC(__VA_ARGS__)
#define SOF_PROFILE_SCOPE_DYNAMIC(NAME) \
    ZoneScoped;                         \
    ZoneName(NAME, strlen(NAME))
#define SOF_PROFILE_THREAD(...) tracy::SetThreadName(__VA_ARGS__)
#else
#define SOF_PROFILE_MARK_FRAME
#define SOF_PROFILE_FUNC(...)
#define SOF_PROFILE_SCOPE(...)
#define SOF_PROFILE_SCOPE_DYNAMIC(NAME)
#define SOF_PROFILE_THREAD(...)
#endif