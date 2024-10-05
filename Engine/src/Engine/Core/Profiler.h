#pragma once

#ifdef DEBUG
#define SOF_ENABLE_PROFILING
#endif

#ifdef SOF_ENABLE_PROFILING
#include <tracy/Tracy.hpp>
#include <glad/gl.h>
#include <tracy/TracyOpenGL.hpp>
#endif

#ifdef SOF_ENABLE_PROFILING
#define SOF_PROFILE_MARK_FRAME FrameMark;
#define SOF_PROFILE_GPU_MARK_CONTEXT TracyGpuContext;
#define SOF_PROFILE_GPU_MARK_SWAP TracyGpuCollect;
#define SOF_PROFILE_GPU(...) TracyGpuZone(__VA_ARGS__)
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
#define SOF_PROFILE_GPU_MARK_CONTEXT
#define SOF_PROFILE_GPU_MARK_SWAP
#define SOF_PROFILE_GPU(...)
#define SOF_PROFILE_FUNC(...)
#define SOF_PROFILE_SCOPE(...)
#define SOF_PROFILE_SCOPE_DYNAMIC(NAME)
#define SOF_PROFILE_THREAD(...)
#endif