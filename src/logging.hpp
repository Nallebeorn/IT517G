#pragma once
#include <time.h>
#include "fmt/core.h"
#include "fmt/chrono.h"

#define LOG(formatstr, ...) __LogInternal("[{:%H:%M:%S}] {}:{}: " formatstr "\n", __FILE__ + 3, __LINE__, ## __VA_ARGS__)

template<typename ...Args>
void __LogInternal(const char *formatstr, const char *file, int line, const Args ...args)
{
    time_t t = time(0);

    fmt::print(formatstr, fmt::localtime(t), file, line, args...);
}