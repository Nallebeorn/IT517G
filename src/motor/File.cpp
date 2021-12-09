#include "File.hpp"

#include <cstring>
#include <stdio.h>
#include "Mem.hpp"
#include "logging.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

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

    size_t fileLen    = GetFileSize(file);
    size_t bufferSize = fileLen + 1; // Need null terminator at end
    char *buffer      = (char *)malloc(bufferSize);

    if (buffer)
    {
        fread(buffer, 1, fileLen, file);
        buffer[bufferSize - 1] = '\0';
    }

    if (outLength)
    {
        *outLength = bufferSize;
    }

    fclose(file);

    return buffer;
}

char *File::LoadIntoScratch(const char *filename, size_t *outLength)
{
    FILE *file = fopen(filename, "rb");

    if (!file)
    {
        LOG("Couldn't load file %s", filename);
        return nullptr;
    }

    size_t fileLen    = GetFileSize(file);
    size_t bufferSize = fileLen + 1; // Need null terminator at end
    auto *buffer      = (char *)Mem::AllocScratch(bufferSize);
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

char *File::GetWorkingDirectory()
{
    DWORD length = GetCurrentDirectoryA(0, nullptr);
    auto buffer  = (char *)Mem::AllocScratch(length);
    GetCurrentDirectoryA(length, buffer);
    return buffer;
}

char *File::ReplaceFilename(const char *sourcePath, const char *newFilename)
{
    size_t newFilenameLen = std::strlen(newFilename);
    size_t length = std::strlen(sourcePath) + newFilenameLen + 1; // +1 for path separator (/)
    char *outString = (char *)Mem::AllocScratch(length + 1); // +1 for null terminator
    const char *lastSlash = std::strrchr(sourcePath, '/');
    if (lastSlash == nullptr)
    {
        return (char *)newFilename;
    }
    size_t directoryLength = lastSlash - sourcePath;
    std::strncpy(outString, sourcePath, directoryLength);
    outString[directoryLength] = '/';
    std::strncpy(outString + directoryLength + 1, newFilename, newFilenameLen);

    LOG("Turned path %s into %s", sourcePath, outString);

    return outString;
}

#endif