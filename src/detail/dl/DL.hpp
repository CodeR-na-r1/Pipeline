#pragma once

#ifdef __linux__

#include "dlImplementaion/DlLinux.hpp"

#elif _WIN32

#include "dlImplementaion/DlWin.hpp"

#else

#error Platform not supported!

#endif