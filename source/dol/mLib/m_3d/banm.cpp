#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_heap.hpp>
#include <constants/sjis_constants.h>

u32 m3d::banm_c::sHeapAlignment;

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

bool m3d::banm_c::createAllocator(mAllocator_c *allocator, size_t *size) {
    size_t aligned = nw4r::ut::RoundUp(mHeap::frmHeapCost(*size, 0x20), 0x20) - mHeap::frmHeapCost(0, 0x20);
    *size = nw4r::ut::RoundUp(mHeap::frmHeapCost(aligned, 0x20), 0x20);

    mpHeap = mHeap::makeFrmHeap(aligned, allocator->mpHeap, M3D_BANM_HEAP_NAME, sHeapAlignment, 0);
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
