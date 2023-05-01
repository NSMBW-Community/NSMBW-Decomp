#pragma once
#include <lib/rvl/mem/MEM.h>

class Allocator;

// [TODO: extend this]

namespace EGG {
    class Heap {
    public:
        virtual ~Heap();
        virtual int getHeapKind() const = 0;
        virtual void initAllocator(Allocator* allocator, s32 align) = 0;
        virtual void *alloc(unsigned long, int) = 0;
        virtual void free(void *) = 0;
        virtual void destroy() = 0;

        static void *alloc(unsigned long, int, EGG::Heap *);
        static void free(void *, EGG::Heap *);

        char mPad[0xc];
        MEMiHeapHeader *mpHeapHead;
    };
}
