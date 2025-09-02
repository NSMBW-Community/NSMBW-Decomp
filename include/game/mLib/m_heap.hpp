#pragma once
#include <types.h>
#include <lib/egg/core/eggFrmHeap.h>
#include <lib/egg/core/eggExpHeap.h>
#include <nw4r/ut.h>

namespace mHeap {
    enum AllocOptBit_t {
        OPT_4 = 4
    };

    void restoreCurrentHeap();
    EGG::Heap *setCurrentHeap(EGG::Heap *);
    size_t frmHeapCost(size_t, size_t);
    void destroyFrmHeap(EGG::FrmHeap *);
    unsigned long adjustFrmHeap(EGG::FrmHeap *);
    void saveCurrentHeap();

    EGG::ExpHeap *createExpHeap(size_t, EGG::Heap *, const char *, size_t, mHeap::AllocOptBit_t);

    /// @unofficial
    EGG::FrmHeap *makeFrmHeapAndUpdate(unsigned long size, EGG::Heap *parent, const char *name, u32 align, u32 opt);
    /// @unofficial
    EGG::FrmHeap *makeFrmHeap(unsigned long size, EGG::Heap *parent, const char *name, u32 align, u32 opt);

    /// @unofficial
    EGG::FrmHeap *createFrmHeap(unsigned long size, EGG::Heap *parent, const char *name, u32 align, u32 opt);

    extern EGG::Heap *g_gameHeaps[3];
};
