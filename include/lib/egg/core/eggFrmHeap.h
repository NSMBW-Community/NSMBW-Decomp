#pragma once

#include <lib/egg/core/eggHeap.h>
#include <revolution/MEM.h>

// [TODO: extend this]

namespace EGG {
    class FrmHeap : public Heap {
    public:
        void free(long);
    };
}
