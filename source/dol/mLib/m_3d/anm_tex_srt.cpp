#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_heap.hpp>

size_t m3d::anmTexSrt_c::child_c::heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexSrt anmTexSrt, bool calcAligned) {
    size_t size = 0;
    nw4r::g3d::AnmObjTexSrtRes::Construct(nullptr, &size, anmTexSrt, mdl, false);
    if (calcAligned) {
        size = nw4r::ut::RoundUp(mHeap::frmHeapCost(size, 0x20), 0x20);
    }
    return size;
}

bool m3d::anmTexSrt_c::child_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexSrt anmTexSrt, mAllocator_c *allocator, size_t *objSize) {
    if (allocator == nullptr) {
        allocator = m3d::internal::l_allocator_p;
    }

    size_t size;
    if (objSize == nullptr) {
        objSize = &size;
    }

    *objSize = heapCost(mdl, anmTexSrt, false);
    if (!createAllocator(allocator, objSize)) {
        return false;
    }

    mpObj = nw4r::g3d::AnmObjTexSrtRes::Construct(&mAllocator, nullptr, anmTexSrt, mdl, false);
    if (mpObj == nullptr) {
        return false;
    }

    if (!mpObj->Bind(mdl)) {
        remove();
        return false;
    }

    setFrmCtrlDefault(anmTexSrt, PLAYMODE_INHERIT);
    return true;
}

void m3d::anmTexSrt_c::child_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmTexSrt anmTexSrt, m3d::playMode_e playMode) {
    releaseAnm();
    mpObj = nw4r::g3d::AnmObjTexSrtRes::Construct(&mAllocator, nullptr, anmTexSrt, mdl.getResMdl(), false);
    mpObj->Bind(mdl.getResMdl());
    setFrmCtrlDefault(anmTexSrt, playMode);
}

void m3d::anmTexSrt_c::child_c::releaseAnm() {
    if (mpObj == nullptr) {
        return;
    }
    mpObj->Release();
    mpHeap->free(MEM_FRMHEAP_FREE_ALL);
    mpObj = nullptr;
}

void m3d::anmTexSrt_c::child_c::setFrmCtrlDefault(nw4r::g3d::ResAnmTexSrt &anmTexSrt, m3d::playMode_e playMode) {
    if (playMode == PLAYMODE_INHERIT) {
        playMode = (anmTexSrt.p->mAnimateType == nw4r::g3d::ANM_POLICY_ONCE) ? FORWARD_ONCE : FORWARD_LOOP;
    }
    fanm_c::set(anmTexSrt.getDuration(), playMode, 1.0f, -1.0f);
}

size_t m3d::anmTexSrt_c::heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexSrt anmTexSrt, long count, bool calcAligned) {
    size_t size = 0;
    nw4r::g3d::AnmObjTexSrtOverride::Construct(nullptr, &size, mdl, count);
    size += nw4r::ut::RoundUp(count * sizeof(child_c), 0x20);
    size += nw4r::ut::RoundUp(child_c::heapCost(mdl, anmTexSrt, true), 0x20) * count;
    if (calcAligned) {
        size = nw4r::ut::RoundUp(mHeap::frmHeapCost(size, 0x20), 0x20);
    }
    return size;
}

bool m3d::anmTexSrt_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexSrt anmTexSrt, mAllocator_c *allocator, size_t *objSize, long count) {
    if (allocator == nullptr) {
        allocator = m3d::internal::l_allocator_p;
    }

    size_t size = 0;
    if (objSize == nullptr) {
        objSize = &size;
    }

    *objSize = heapCost(mdl, anmTexSrt, count, false);
    if (!createAllocator(allocator, objSize)) {
        return false;
    }

    mpObj = nw4r::g3d::AnmObjTexSrtOverride::Construct(&mAllocator, nullptr, mdl, count);
    if (mpObj == nullptr) {
        return false;
    }

    children = (m3d::anmTexSrt_c::child_c *) MEMAllocFromAllocator(&mAllocator, nw4r::ut::RoundUp(count * sizeof(child_c), 0x20));
    if (children == nullptr) {
        remove();
        return false;
    }

    nw4r::g3d::AnmObjTexSrtOverride *texSrtOverride = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexSrtOverride>(mpObj);

    child_c *child = &children[0];
    for (int i = 0; i < count; i++) {
        new(child) child_c();
        if (!child->create(mdl, anmTexSrt, &mAllocator, nullptr)) {
            mHeap::destroyFrmHeap(mpHeap);
            EGG::Heap::free(mpHeap, nullptr);
            return false;
        }
        if (i == 0) {
            nw4r::g3d::AnmObjTexSrtRes *texSrtRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexSrtRes>(child->getObj());
            texSrtOverride->Attach(i, texSrtRes);
        } else {
            child->releaseAnm();
        }
        child++;
    }
    return true;
}

m3d::anmTexSrt_c::~anmTexSrt_c() {
    anmTexSrt_c::remove();
}

void m3d::anmTexSrt_c::remove() {
    nw4r::g3d::AnmObjTexSrtOverride *texSrt = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexSrtOverride>(mpObj);
    if (texSrt != nullptr && children != nullptr) {
        int count = texSrt->getCount();
        for (int i = 0; i < count; i++) {
            children[i].remove();
        }
        children = nullptr;
    }
    banm_c::remove();
}

void m3d::anmTexSrt_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmTexSrt anmTexSrt, long idx, m3d::playMode_e playMode) {
    nw4r::g3d::AnmObjTexSrtOverride *texSrt = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexSrtOverride>(mpObj);
    texSrt->Detach(idx);
    children[idx].setAnm(mdl, anmTexSrt, playMode);
    nw4r::g3d::AnmObjTexSrtRes *texSrtRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexSrtRes>(children[idx].getObj());
    texSrt->Attach(idx, texSrtRes);
}

void m3d::anmTexSrt_c::releaseAnm(long idx) {
    nw4r::g3d::AnmObjTexSrtOverride *texSrt = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexSrtOverride>(mpObj);
    texSrt->Detach(idx);
    children[idx].releaseAnm();
}

void m3d::anmTexSrt_c::play() {
    nw4r::g3d::AnmObjTexSrtOverride *texSrt = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexSrtOverride>(mpObj);
    int count = texSrt->getCount();
    for (int i = 0; i < count; i++) {
        play(i);
    }
}

void m3d::anmTexSrt_c::play(long idx) {
    if (children[idx].IsBound()) {
        children[idx].play();
    }
}

float m3d::anmTexSrt_c::getFrame(long idx) const {
    return children[idx].getFrame();
}

void m3d::anmTexSrt_c::setFrame(float frame, long idx) {
    children[idx].setFrame(frame);
}

float m3d::anmTexSrt_c::getRate(long idx) const {
    return children[idx].getRate();
}

void m3d::anmTexSrt_c::setRate(float rate, long idx) {
    children[idx].setRate(rate);
}

bool m3d::anmTexSrt_c::isStop(long idx) const {
    return children[idx].isStop();
}

void m3d::anmTexSrt_c::setPlayMode(m3d::playMode_e mode, long idx) {
    children[idx].mPlayMode = mode;
}

float m3d::anmTexSrt_c::getFrameMax(long idx) const {
    return children[idx].mFrameMax;
}

void m3d::anmTexSrt_c::setFrameStart(float frame, long idx) {
    children[idx].mFrameStart = frame;
}
