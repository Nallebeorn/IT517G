#pragma once
#include "typedefs.hpp"

namespace Mem
{
    class Arena
    {
      public:
        void Initialize(size_t capacity);

        void *Allocate(size_t size);

        void Reset();
        size_t GetCurrentSize() const;
        size_t GetPeakSize() const;
        size_t GetCapacity() const;

      private:
        u8 *buffer;
        u8 *cursor;
        size_t capacity;
        u8 *maxCursor;
    };

    void Init();
    void *AlignUp(void *ptr, size_t alignment);

    void *AllocScratch(size_t size);
    void ResetScratch();
    void PrintScratchStats();
}