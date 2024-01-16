#pragma once
#include <lib/rvl/mem/MEM.h>
#include <lib/egg/heap/eggFrmHeap.hpp>

class mAllocator_c : public MEMAllocator {
public:
    mAllocator_c();
    virtual ~mAllocator_c();
    virtual void alloc(unsigned long size);
    virtual void free(void *ptr);
    bool attach(EGG::Heap *heap, int);

    EGG::FrmHeap *mpHeap;
    u32 unk2;
};
