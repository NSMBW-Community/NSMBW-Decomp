#pragma once

#include <lib/egg/heap/eggHeap.hpp>
#include <lib/rvl/mem/MEMFrmHeap.h>

// [TODO: extend this]

namespace EGG {
    class FrmHeap : public Heap {
    public:
        void free(long);
    };
}
