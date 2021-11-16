#pragma once
#include <ctime>
#include "GLFW/glfw3.h"
#include "fmt/core.h"
#include "fmt/chrono.h"

#define LOG(formatstr, ...) LogInternal("[{:f}] {}:{}: " formatstr "\n", ((const char *)__FILE__ + 3), __LINE__, ## __VA_ARGS__)

template<typename ...Args>
void LogInternal(const char *formatstr, const char *file, int line, const Args ...args)
{
    try
    {
        fmt::print(formatstr, glfwGetTime(), file, line, args...);
    }
    catch (fmt::format_error &e)
    {
        // This feels ironic to do
        fmt::print("(!! LOG ERROR !!) {}:{}: {} :: {}", file, line, e.what(), formatstr);
    }
}