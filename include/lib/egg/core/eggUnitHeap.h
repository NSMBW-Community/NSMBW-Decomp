#pragma once
#include <revolution/MEM/mem_unitHeap.h>
#include <lib/egg/core/eggHeap.h>

namespace EGG {

class UnitHeap : public Heap, MEMiUntHeapHead {
public:
    static UnitHeap* create(void* block, size_t size, size_t unitSize, long align, u16 flag);
    static size_t calcHeapSize(size_t unitSize, ulong count, long align);
};

} // namespace EGG
