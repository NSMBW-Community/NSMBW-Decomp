#pragma once
#include <types.h>
#include <lib/egg/core/eggHeap.h>
#include <revolution/MEM.h>

namespace EGG {
    class Allocator : public MEMAllocator {
    public:
        Allocator(Heap *h, long l);

        virtual ~Allocator();
        virtual void *alloc(unsigned long);
        virtual void free(void *);

    public:
        EGG::Heap *mpHeap;
        int mAlign;
    };
}
