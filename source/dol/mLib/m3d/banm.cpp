#include <dol/mLib/m_3d.hpp>

#include <dol/mLib/m_heap.hpp>
#include <constants/sjis_constants.h>

m3d::banm_c::~banm_c() {
    banm_c::remove();
}

void m3d::banm_c::remove() {
    if (mpObj != nullptr) {
        mpObj->DetachFromParent();
        mpObj->Destroy();
        mpObj = nullptr;
    }
    if (mpHeap != nullptr) {
        mHeap::destroyFrmHeap(mpHeap);
        mpHeap = nullptr;
    }
}

u32 hash_10B20216_005CCF30;

bool m3d::banm_c::createAllocator(mAllocator_c *allocator, unsigned long *size) {
    size_t aligned = align32Bit(mHeap::frmHeapCost(*size, 0x20)) - mHeap::frmHeapCost(0, 0x20);
    *size = align32Bit(mHeap::frmHeapCost(aligned, 0x20));

    mpHeap = mHeap::makeFrmHeap(aligned, allocator->mpHeap, M3D_BANM_HEAP_NAME, hash_10B20216_005CCF30, 0);

    mAllocator.attach(mpHeap, 0x20);
    
    return true;
}

bool m3d::banm_c::IsBound() const {
    if (mpObj == nullptr) {
        return false;
    }
    return mpObj->TestAnmFlag(nw4r::g3d::AnmObj::IS_BOUND);
}

float m3d::banm_c::getFrame() const {
    return mpObj->GetFrame();
}

void m3d::banm_c::setFrameOnly(float frame) {
    mpObj->SetFrame(frame);
}

float m3d::banm_c::getRate() const {
    return mpObj->GetUpdateRate();
}

void m3d::banm_c::setRate(float frame) {
    mpObj->SetUpdateRate(frame);
}
