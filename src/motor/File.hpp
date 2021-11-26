#pragma once

namespace Mem
{
class Arena;
}

namespace File
{
char *LoadIntoNewBuffer(const char *filename, size_t *outLength = nullptr);
char *LoadIntoScratch(const char *filename, size_t *outLength = nullptr);
bool DoesDirectoryExist(const char *path);
}