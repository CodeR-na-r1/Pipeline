#pragma once

#ifdef __linux__

#include "dlImplementaion/dlLinux.hpp"

#elif _WIN32

#include "dlImplementaion/dlWin.hpp"

#else

#error Platform not supported!

#endif