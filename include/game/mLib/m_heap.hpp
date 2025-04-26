#pragma once
#include <types.h>
#include <lib/egg/heap/eggFrmHeap.hpp>
#include <lib/nw4r/ut/inlines.hpp>

namespace mHeap {
    void restoreCurrentHeap();
    EGG::Heap *setCurrentHeap(EGG::Heap *);
    size_t frmHeapCost(size_t, size_t);
    void destroyFrmHeap(EGG::FrmHeap *);
    unsigned long adjustFrmHeap(EGG::FrmHeap *);
    void saveCurrentHeap();

    /// @unofficial
    EGG::FrmHeap *makeFrmHeapAndUpdate(unsigned long size, EGG::Heap *parent, const char *name, u32 align, u32 opt);
    /// @unofficial
    EGG::FrmHeap *makeFrmHeap(unsigned long size, EGG::Heap *parent, const char *name, u32 align, u32 opt);

    /// @unofficial
    EGG::FrmHeap *createFrmHeap(unsigned long size, EGG::Heap *parent, const char *name, u32 align, u32 opt);

    extern EGG::Heap *g_gameHeaps[3];
};
