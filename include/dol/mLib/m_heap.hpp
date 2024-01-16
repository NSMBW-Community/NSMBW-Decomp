#pragma once
#include <types.h>
#include <lib/egg/heap/eggFrmHeap.hpp>

namespace mHeap {
    void restoreCurrentHeap();
    EGG::Heap *setCurrentHeap(EGG::Heap *);
    void destroyFrmHeap(EGG::FrmHeap *);
    unsigned long adjustFrmHeap(EGG::FrmHeap *);

    /// @unofficial
    EGG::FrmHeap *makeFrmHeapAndUpdate(unsigned long size, EGG::Heap *parent, const char *name, u32 align, u32 opt);

    extern EGG::Heap *g_gameHeaps[3];
};
