#pragma once

#include <lib/egg/core/eggHeap.h>
#include <lib/rvl/mem/MEMFrmHeap.h>

// [TODO: extend this]

namespace EGG {
    class FrmHeap : public Heap {
    public:
        void free(long);
    };
}
