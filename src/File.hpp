#pragma once

namespace File
{
char *LoadIntoNewBuffer(const char *filename, size_t *outLength = nullptr);
bool DoesDirectoryExist(const char *path);
}