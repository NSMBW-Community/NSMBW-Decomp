#include "egg/core/eggAssertHeap.h"
#include <game/mlib/m_heap.hpp>
#include <constants/sjis_constants.h>

namespace mHeap {

u8 g_DefaultGameHeapId = 1;
const char * const s_GameHeapNames[3] = {
    nullptr,
    GAME_HEAP_1_NAME,
    GAME_HEAP_2_NAME,
};

EGG::Heap *s_SavedCurrentHeap;
EGG::Heap *g_gameHeaps[3];
EGG::Heap *g_archiveHeap;
EGG::Heap *g_commandHeap;
EGG::Heap *g_dylinkHeap;
EGG::Heap *g_assertHeap;

u16 GetOptFlag(AllocOptBit_t opt) {
    u16 ret = OPT_NONE;

    if (opt & OPT_CLEAR_ALLOC) {
        ret = MEM_HEAP_OPT_CLEAR_ALLOC;
    }

    if (opt & OPT_DEBUG_FILL) {
        ret |= MEM_HEAP_OPT_DEBUG_FILL;
    }

    if (opt & OPT_THREAD_SAFE) {
        ret |= MEM_HEAP_OPT_CAN_LOCK;
    }

    return ret;
}

EGG::Heap *setCurrentHeap(EGG::Heap *heap) {
    return heap->becomeCurrentHeap();
}

EGG::ExpHeap *createExpHeap(size_t size, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt) {
    if (parent == nullptr) {
        parent = EGG::Heap::sCurrentHeap;
    }

    if (align < 0x20) {
        align = 0x20;
    }

    if (size != -1) {
        size = expHeapCost(size, align);
    } else {
        size = parent->getAllocatableSize(align);
    }

    void *buffer = parent->alloc(size, align);
    EGG::ExpHeap *heap = nullptr;

    if (buffer != nullptr) {
        u16 flags = GetOptFlag(opt);
        heap = EGG::ExpHeap::create(buffer, size, flags);
        if (heap == nullptr) {
            parent->free(buffer);
        } else if (name != nullptr) {
            heap->mpName = name;
        }
    }

    return heap;
}

size_t expHeapCost(size_t size, ulong align) {
    return size + nw4r::ut::RoundUp<size_t>(sizeof(EGG::ExpHeap) + sizeof(MEMiHeapHead) + sizeof(MEMiExpHeapHead), align);
}

EGG::FrmHeap *createFrmHeap(size_t size, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt) {
    if (parent == nullptr) {
        parent = EGG::Heap::sCurrentHeap;
    }

    if (align < 0x20) {
        align = 0x20;
    }

    if (size != -1) {
        size = frmHeapCost(size, align);
    } else {
        size = parent->getAllocatableSize(align);
    }

    void *buffer = parent->alloc(size, align);
    EGG::FrmHeap *heap = nullptr;

    if (buffer != nullptr) {
        u16 flags = GetOptFlag(opt);
        heap = EGG::FrmHeap::create(buffer, size, flags);
        if (heap == nullptr) {
            parent->free(buffer);
        } else if (name != nullptr) {
            heap->mpName = name;
        }
    }

    return heap;
}

void destroyFrmHeap(EGG::FrmHeap *heap) {
    if (heap != nullptr) {
        heap->destroy();
    }
}

size_t adjustFrmHeap(EGG::FrmHeap *heap) {
    size_t adjustedSize = 0;

    if (heap != nullptr) {
        size_t a = heap->adjust();
        size_t cost = frmHeapCost(0, 4);
        if (a >= cost) {
            adjustedSize = a - cost;
        }
    }

    return adjustedSize;
}

size_t frmHeapCost(size_t size, ulong align) {
    return size + nw4r::ut::RoundUp<size_t>(sizeof(EGG::FrmHeap) + sizeof(MEMiHeapHead) + sizeof(MEMiFrmHeapHead), align);
}

EGG::UnitHeap *createUntHeap(size_t size, ulong count, EGG::Heap *parent, const char *name, ulong align, mHeap::AllocOptBit_t opt) {
    if (parent == nullptr) {
        parent = EGG::Heap::sCurrentHeap;
    }

    if (align < 0x20) {
        align = 0x20;
    }

    size_t totalSize = untHeapCost(size, count, align);
    EGG::UnitHeap *heap = nullptr;
    void *buffer = parent->alloc(totalSize, align);

    if (buffer != nullptr) {
        u16 flags = GetOptFlag(opt);
        heap = EGG::UnitHeap::create(buffer, totalSize, size, align, flags);
        if (heap == nullptr) {
            parent->free(buffer);
        } else if (name != nullptr) {
            heap->mpName = name;
        }
    }

    return heap;
}

size_t untHeapCost(size_t size, ulong count, ulong align) {
    return EGG::UnitHeap::calcHeapSize(size, count, align);
}

EGG::Heap *createHeap(size_t size, EGG::Heap *parent, const char *name) {

    EGG::ExpHeap *heap = EGG::ExpHeap::create(size, parent, MEM_HEAP_OPT_CAN_LOCK);
    if (heap != nullptr) {
        heap->setAllocMode(MEM_EXP_HEAP_ALLOC_FAST);
        if (name != nullptr) {
            heap->mpName = name;
        }
    } else {
        parent->dump();
    }

    return heap;
}

void saveCurrentHeap() {
    s_SavedCurrentHeap = EGG::Heap::sCurrentHeap;
}

void restoreCurrentHeap() {
    s_SavedCurrentHeap->becomeCurrentHeap();
    s_SavedCurrentHeap = nullptr;
}

EGG::FrmHeap *createFrmHeapToCurrent(size_t size, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt) {
    EGG::FrmHeap *heap = createFrmHeap(size, parent, name, align, opt);
    if (heap != nullptr) {
        s_SavedCurrentHeap = setCurrentHeap(heap);
    }

    return heap;
}

EGG::Heap *createGameHeap(int idx, size_t size, EGG::Heap *parent) {
    bool isValid = false;
    if (1U <= idx && idx <= 2U) {
        isValid = true;
    }

    if (!isValid) {
        return nullptr;
    }

    g_gameHeaps[idx] = createHeap(size, parent, s_GameHeapNames[idx]);

    if (idx == g_DefaultGameHeapId) {
        g_gameHeaps[0] = g_gameHeaps[idx];
    }

    return g_gameHeaps[idx];
}

void createGameHeap1(size_t size, EGG::Heap *parent) {
    createGameHeap(1, size, parent);
}

void createGameHeap2(size_t size, EGG::Heap *parent) {
    createGameHeap(2, size, parent);
}

void createArchiveHeap(size_t size, EGG::Heap *parent) {
    g_archiveHeap = createHeap(size, parent, ARCHIVE_HEAP_NAME);
}

void createCommandHeap(size_t size, EGG::Heap *parent) {
    g_commandHeap = createHeap(size, parent, COMMAND_HEAP_NAME);
}

void createDylinkHeap(size_t size, EGG::Heap *parent) {
    g_dylinkHeap = createHeap(size, parent, DYLINK_HEAP_NAME);
}

EGG::Heap *createAssertHeap(EGG::Heap *parent) {
    const char *heapName = ASSERT_HEAP_NAME;
    size_t size = EGG::AssertHeap::getMinSizeForCreate();
    g_assertHeap = EGG::AssertHeap::create(size, parent);
    g_assertHeap->mpName = heapName;
    return g_assertHeap;
}

}