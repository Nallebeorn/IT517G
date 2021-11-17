#include <cstdio>

#ifdef _WIN32
#include <windows.h>

#endif
#include "File.hpp"
#include "logging.hpp"

static long GetFileSize(std::FILE *file)
{
    long oldPos = ftell(file);
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, oldPos, SEEK_SET);
    return size;
}

char *File::LoadIntoNewBuffer(const char *filename, size_t *outLength)
{
    FILE *file = fopen(filename, "rb");

    if (!file)
    {
        LOG("Couldn't load file %s", filename);
        return nullptr;
    }

    long fileLen = GetFileSize(file);
    size_t bufferSize = fileLen + 1; // Need null terminator at end
    char *buffer = (char *)malloc(bufferSize);
    fread(buffer, 1, fileLen, file);
    buffer[bufferSize - 1] = '\0';

    if (outLength)
    {
        *outLength = bufferSize;
    }

    fclose(file);

    return buffer;
}

#ifdef _WIN32

bool File::DoesDirectoryExist(const char *path)
{
    DWORD attributes = GetFileAttributesA(path);

    if (attributes == INVALID_FILE_ATTRIBUTES)
    {
        return false;
    }

    return (attributes & FILE_ATTRIBUTE_DIRECTORY);
}

#endif