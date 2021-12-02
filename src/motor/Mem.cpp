#include "Mem.hpp"
#include "logging.hpp"

#include <assert.h>
#include <cstddef>
#include <cstdio>
#include <cstdlib>

static Mem::Arena scratchArena;

static bool isInitialized;

void Mem::Init()
{
    if (!isInitialized)
    {
        scratchArena.Initialize(32 * 1024 * 1024);
        isInitialized = true;
    }
}

void Mem::Arena::Initialize(size_t size)
{
    buffer = (uint8 *)calloc(size, 1);

    if (buffer == nullptr)
    {
        printf("Memory arena initialization of %zu bytes failed.", size);
        abort();
    }

    cursor = buffer;
    maxCursor = cursor;
    capacity = size;
}

void *Mem::Arena::Allocate(size_t size)
{
    assert(buffer != nullptr);

    constexpr size_t alignment = alignof(std::max_align_t);
    void *returnPtr = cursor;
    returnPtr = Mem::AlignUp(returnPtr, alignment);
    cursor = (uint8 *)returnPtr + size;

    if (cursor >= buffer + capacity)
    {
        printf("Arena buffer overflow when allocating %zu bytes!", size);
        abort();
    }

    if (cursor > maxCursor)
    {
        maxCursor = cursor;
    }

    return returnPtr;
}

void Mem::Arena::Reset()
{
    this->cursor = this->buffer;
}

size_t Mem::Arena::GetCurrentSize() const
{
    return cursor - buffer;
}

size_t Mem::Arena::GetPeakSize() const
{
    return maxCursor - buffer;
}

size_t Mem::Arena::GetCapacity() const
{
    return capacity;
}

void *Mem::AlignUp(void *ptr, size_t alignment)
{
    assert((alignment & (alignment - 1)) == 0); // alignment must be power of two
    auto num = (intptr_t)ptr;
    auto alignedNum = (num + alignment) & ~(alignment - 1);
    return (void *)alignedNum;
}

void *Mem::AllocScratch(size_t size)
{
    return scratchArena.Allocate(size);
}

void Mem::ResetScratch()
{
    scratchArena.Reset();
}

void Mem::PrintScratchStats()
{
    printf("Scratch memory arena peak usage: %zu/%zu bytes (%.2f/%.2fMiB) = %.1f%%",
           scratchArena.GetPeakSize(), scratchArena.GetCapacity(),
           scratchArena.GetPeakSize() / (1024.0 * 1024.0), scratchArena.GetCapacity() / (1024.0 * 1024.0),
           (double)scratchArena.GetPeakSize() / (double)scratchArena.GetCapacity());
}
