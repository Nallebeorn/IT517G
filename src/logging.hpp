#pragma once
#include <ctime>
#include <cstdio>
#include "GLFW/glfw3.h"

#define LOG(formatstr, ...) std::printf("[%7.3f] %s:%d: " formatstr "\n", glfwGetTime(), ((const char *)__FILE__ + 3), __LINE__, ## __VA_ARGS__)