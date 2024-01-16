#pragma once

#include <lib/egg/heap/eggHeap.hpp>

// [TODO: extend this]

namespace EGG {
    class FrmHeap : public Heap {
    public:
        void free(long);
    };
}
