#pragma once
#include <lib/egg/core/eggHeap.h>

namespace EGG {

// [TODO: extend this]
class ExpHeap : public Heap {
public:
    void setAllocMode(u16 mode);

    static EGG::ExpHeap *create(void *buffer, size_t size, u16 flags);
    static EGG::ExpHeap *create(size_t, EGG::Heap *, u16);
};

} // namespace EGG