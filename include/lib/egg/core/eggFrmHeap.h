#pragma once
#include <lib/egg/core/eggHeap.h>
#include <revolution/MEM.h>

namespace EGG {

// [TODO: extend this]
class FrmHeap : public Heap {
public:
    void free(long);

    static FrmHeap *create(void *buffer, size_t size, u16 flags);
};

} // namespace EGG
