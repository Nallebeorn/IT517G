#pragma once
#include <cstdio>
#include <ctime>
#include "GLFW/glfw3.h"

// AC - ANSI Colors
// Regular text
#define AC_BLACK "\x1b[0;30m"
#define AC_RED "\x1b[0;31m"
#define AC_GREEN "\x1b[0;32m"
#define AC_YELLOW "\x1b[0;33m"
#define AC_BLUE "\x1b[0;34m"
#define AC_MAGENTA "\x1b[0;35m"
#define AC_CYAN "\x1b[0;36m"
#define AC_WHITE "\x1b[0;37m"

//Reset
#define AC_RESET "\x1b[0m"

#define LOG(formatstr, ...) std::printf("[%7.3f] %s:%d: " formatstr "\n" AC_RESET, glfwGetTime(), ((const char *)__FILE__ + 3), __LINE__, ##__VA_ARGS__)