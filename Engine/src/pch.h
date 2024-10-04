#pragma once

#ifdef PLATFORM_WINDOWS
#ifndef NOMINMAX
// See github.com/skypjack/entt/wiki/Frequently-Asked-Questions#warning-c4003-the-min-the-max-and-the-macro
#define NOMINMAX
#endif
#endif

#include <cmath>
#include <assert.h>
#include <iostream>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <cassert>
#include <stdexcept>
#include <limits>
#include <fstream>
#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <sstream>
#include <cstring>
#include <cstdint>
#include <vector>
#include <initializer_list>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <array>
#include <stack>
#include <queue>
#include <stdio.h>
#include <numbers>

#include <chrono>
#include <future>
#include <thread>
#include <semaphore>

#include <glm/glm.hpp>
#include "Engine/Core/UUID.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Base.h"
#include "Engine/Core/Profiler.h"
#include <spdlog/fmt/fmt.h>

#ifdef PLATFORM_WINDOWS
#include <Windows.h>
#endif
