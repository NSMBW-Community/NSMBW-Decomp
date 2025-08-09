#pragma once

#include <lib/egg/core/eggHeap.h>

// [TODO: extend this]

namespace EGG {
    class ExpHeap : public Heap {
    public:
        static EGG::ExpHeap *create(size_t, EGG::Heap *, u16);
    };
}
