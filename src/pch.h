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
#include "Core/UUID.h"
#include "Core/Log.h"
#include "Core/Base.h"

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif