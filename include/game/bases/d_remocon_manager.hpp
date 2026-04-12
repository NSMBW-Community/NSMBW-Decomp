#pragma once

#include <lib/egg/core/eggHeap.h>

class dRemoconMng_c {
public:
    class dConnect_c {
    public:
        static bool m_isBoot;
    };

    static void create(EGG::Heap *heap);
    static void execute();
};
