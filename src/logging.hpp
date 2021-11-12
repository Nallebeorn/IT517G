#pragma once
#include <time.h>
#include "fmt/core.h"
#include "fmt/chrono.h"

/* A clever hack (https://stackoverflow.com/a/40947954) to get a project-root relative filepath.
 * SOURCE_DIR_PATH_LENGTH is a macro defined in the CMake script.
 * The cast (from char[]) to char * is needed to suppress string-plus-int warning
 * */
#define __FILENAME__ ((const char *)__FILE__ + SOURCE_DIR_PATH_LENGTH)

#define LOG(formatstr, ...) __LogInternal("[{:%H:%M:%S}] {}:{}: " formatstr "\n", __FILENAME__, __LINE__, ## __VA_ARGS__)

template<typename ...Args>
void __LogInternal(const char *formatstr, const char *file, int line, const Args ...args)
{
    time_t t = time(0);

    fmt::print(formatstr, fmt::localtime(t), file, line, args...);
}