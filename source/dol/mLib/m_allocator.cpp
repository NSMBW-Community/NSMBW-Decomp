#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/mLib/m_allocator.hpp>
#include <game/mLib/m_heap.hpp>
#include <lib/egg/core/eggFrmHeap.h>

mAllocatorDummyHeap_c *mAllocatorDummyHeap_c::m_instance;

void *mAllocatorDummyHeap_c::AllocatorAllocForDummyHeap(MEMAllocator *, size_t) { return nullptr; }
void mAllocatorDummyHeap_c::AllocatorFreeForDummyHeap(MEMAllocator *, void *) {}

mAllocatorDummyHeap_c::mAllocatorDummyHeap_c() {
    m_instance = this;
}

mAllocatorDummyHeap_c::~mAllocatorDummyHeap_c() {}

int mAllocatorDummyHeap_c::getHeapKind() const {
    return 0;
}

void mAllocatorDummyHeap_c::initAllocator(EGG::Allocator *allocator, long alignment) {
    const static MEMAllocatorFuncs sAllocatorFunc = {
        AllocatorAllocForDummyHeap,
        AllocatorFreeForDummyHeap
    };
    allocator->funcs = &sAllocatorFunc;
    allocator->heap = mHeapHandle;
    allocator->heapParam1 = alignment;
    allocator->heapParam2 = 0;
}

void *mAllocatorDummyHeap_c::alloc(size_t bytes, long alignment) {
    return nullptr;
}

void mAllocatorDummyHeap_c::free(void *ptr) {}

void mAllocatorDummyHeap_c::destroy() {}

size_t mAllocatorDummyHeap_c::resizeForMBlock(void *ptr, size_t newSize) {
    return 0;
}

u32 mAllocatorDummyHeap_c::getTotalFreeSize() {
    return 0;
}

size_t mAllocatorDummyHeap_c::getAllocatableSize(long alignment) {
    return 0;
}

size_t mAllocatorDummyHeap_c::adjust() {
    return 0;
}

mAllocatorDummyHeap_c *mAllocatorDummyHeap_c::getInstance() {
    return m_instance;
}

mAllocator_c::mAllocator_c() : EGG::Allocator(mAllocatorDummyHeap_c::getInstance(), 4) {
}

mAllocator_c::~mAllocator_c() {}

bool mAllocator_c::attach(EGG::Heap *heap, int alignment) {
    if (heap == nullptr) {
        heap = EGG::Heap::sCurrentHeap;
    }
    mAlign = alignment;
    mpHeap = heap;
    heap->initAllocator(this, alignment);
    return true;
}

void *mAllocator_c::alloc(size_t size) {
    return EGG::Allocator::alloc(size);
}

void mAllocator_c::free(void *ptr) {
    EGG::Allocator::free(ptr);
}

void mAllocator_c::init(EGG::Heap *heap) {
    new(heap, 4) mAllocatorDummyHeap_c();
}

mHeapAllocator_c::mHeapAllocator_c() {}

mHeapAllocator_c::~mHeapAllocator_c() {
    destroyHeap();
}

bool mHeapAllocator_c::createFrmHeap(size_t size, EGG::Heap *parent, const char *name, ulong align, mHeap::AllocOptBit_t opt) {
    destroyHeap();
    EGG::FrmHeap *heap = mHeap::createFrmHeap(size, parent, name, align, opt);
    if (heap == nullptr) {
        return false;
    }

    attach(heap, align);
    return true;
}

void mHeapAllocator_c::destroyHeap() {
    EGG::Heap *dummyHeap = mAllocatorDummyHeap_c::getInstance();
    if (mpHeap != dummyHeap) {
        mpHeap->destroy();
        mpHeap = dummyHeap;
    }
}

size_t mHeapAllocator_c::adjustFrmHeap() {
    EGG::Heap *dummyHeap = mAllocatorDummyHeap_c::getInstance();
    if (mpHeap == dummyHeap) {
        return 0;
    }

    EGG::FrmHeap *heap = EGG::Heap::dynamicCastToFrm(mpHeap);
    return mHeap::adjustFrmHeap(heap);
}

bool mHeapAllocator_c::createFrmHeapToCurrent(size_t size, EGG::Heap *parent, const char *name, ulong align, mHeap::AllocOptBit_t opt) {
    if (!createFrmHeap(size, parent, name, align, opt)) {
        return false;
    }

    mHeap::saveCurrentHeap();
    mHeap::setCurrentHeap(mpHeap);
    return true;
}

size_t mHeapAllocator_c::adjustFrmHeapRestoreCurrent() {
    mHeap::restoreCurrentHeap();
    EGG::FrmHeap *heap = EGG::Heap::dynamicCastToFrm(mpHeap);
    return mHeap::adjustFrmHeap(heap);
}
