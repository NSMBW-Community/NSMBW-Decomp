#include <game/mlib/m_heap.hpp>
#include <constants/sjis_constants.h>

u8 mHeap::g_DefaultGameHeapId = GAME_HEAP_MEM1;
const char * const mHeap::s_GameHeapNames[GAME_HEAP_COUNT] = {
    nullptr,
    GAME_HEAP_1_NAME,
    GAME_HEAP_2_NAME,
};

EGG::Heap *mHeap::s_SavedCurrentHeap;

EGG::Heap *mHeap::g_gameHeaps[GAME_HEAP_COUNT];
EGG::Heap *mHeap::g_archiveHeap;
EGG::Heap *mHeap::g_commandHeap;
EGG::Heap *mHeap::g_dylinkHeap;
EGG::Heap *mHeap::g_assertHeap;

u16 mHeap::GetOptFlag(AllocOptBit_t opt) {
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

EGG::Heap *mHeap::setCurrentHeap(EGG::Heap *heap) {
    return heap->becomeCurrentHeap();
}

EGG::ExpHeap *mHeap::createExpHeap(size_t size, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt) {
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
        heap = EGG::ExpHeap::create(buffer, size, GetOptFlag(opt));
        if (heap == nullptr) {
            parent->free(buffer);
        } else if (name != nullptr) {
            heap->mpName = name;
        }
    }

    return heap;
}

size_t mHeap::expHeapCost(size_t size, ulong align) {
    return size + nw4r::ut::RoundUp<size_t>(sizeof(EGG::ExpHeap) + MEM_EXP_HEAP_HEAD_SIZE, align);
}

EGG::FrmHeap *mHeap::createFrmHeap(size_t size, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt) {
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
        heap = EGG::FrmHeap::create(buffer, size, GetOptFlag(opt));
        if (heap == nullptr) {
            parent->free(buffer);
        } else if (name != nullptr) {
            heap->mpName = name;
        }
    }

    return heap;
}

void mHeap::destroyFrmHeap(EGG::FrmHeap *heap) {
    if (heap != nullptr) {
        heap->destroy();
    }
}

size_t mHeap::adjustFrmHeap(EGG::FrmHeap *heap) {
    size_t totalFreeSpace = 0;

    if (heap != nullptr) {
        size_t freeSpace = heap->adjust();
        size_t minCost = frmHeapCost(0, 4);
        if (freeSpace >= minCost) {
            totalFreeSpace = freeSpace - minCost;
        }
    }

    return totalFreeSpace;
}

size_t mHeap::frmHeapCost(size_t size, ulong align) {
    return size + nw4r::ut::RoundUp<size_t>(sizeof(EGG::FrmHeap) + MEM_FRM_HEAP_HEAD_SIZE, align);
}

EGG::UnitHeap *mHeap::createUntHeap(size_t size, ulong count, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt) {
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
        heap = EGG::UnitHeap::create(buffer, totalSize, size, align, GetOptFlag(opt));
        if (heap == nullptr) {
            parent->free(buffer);
        } else if (name != nullptr) {
            heap->mpName = name;
        }
    }

    return heap;
}

size_t mHeap::untHeapCost(size_t size, ulong count, ulong align) {
    return EGG::UnitHeap::calcHeapSize(size, count, align);
}

EGG::Heap *mHeap::createHeap(size_t size, EGG::Heap *parent, const char *name) {
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

void mHeap::saveCurrentHeap() {
    s_SavedCurrentHeap = EGG::Heap::sCurrentHeap;
}

void mHeap::restoreCurrentHeap() {
    s_SavedCurrentHeap->becomeCurrentHeap();
    s_SavedCurrentHeap = nullptr;
}

EGG::FrmHeap *mHeap::createFrmHeapToCurrent(size_t size, EGG::Heap *parent, const char *name, ulong align, AllocOptBit_t opt) {
    EGG::FrmHeap *heap = createFrmHeap(size, parent, name, align, opt);
    if (heap != nullptr) {
        s_SavedCurrentHeap = setCurrentHeap(heap);
    }

    return heap;
}

EGG::Heap *mHeap::createGameHeap(int idx, size_t size, EGG::Heap *parent) {
    if (!isValidGameHeapId(idx)) {
        return nullptr;
    }

    g_gameHeaps[idx] = createHeap(size, parent, s_GameHeapNames[idx]);
    if (idx == g_DefaultGameHeapId) {
        g_gameHeaps[GAME_HEAP_DEFAULT] = g_gameHeaps[idx];
    }

    return g_gameHeaps[idx];
}

EGG::Heap *mHeap::createGameHeap1(size_t size, EGG::Heap *parent) {
    return createGameHeap(GAME_HEAP_MEM1, size, parent);
}

EGG::Heap *mHeap::createGameHeap2(size_t size, EGG::Heap *parent) {
    return createGameHeap(GAME_HEAP_MEM2, size, parent);
}

EGG::Heap *mHeap::createArchiveHeap(size_t size, EGG::Heap *parent) {
    g_archiveHeap = createHeap(size, parent, ARCHIVE_HEAP_NAME);
    return g_archiveHeap;
}

EGG::Heap *mHeap::createCommandHeap(size_t size, EGG::Heap *parent) {
    g_commandHeap = createHeap(size, parent, COMMAND_HEAP_NAME);
    return g_commandHeap;
}

EGG::Heap *mHeap::createDylinkHeap(size_t size, EGG::Heap *parent) {
    g_dylinkHeap = createHeap(size, parent, DYLINK_HEAP_NAME);
    return g_dylinkHeap;
}

EGG::Heap *mHeap::createAssertHeap(EGG::Heap *parent) {
    const char *heapName = ASSERT_HEAP_NAME;
    size_t size = EGG::AssertHeap::getMinSizeForCreate();
    g_assertHeap = EGG::AssertHeap::create(size, parent);
    g_assertHeap->mpName = heapName;
    return g_assertHeap;
}
