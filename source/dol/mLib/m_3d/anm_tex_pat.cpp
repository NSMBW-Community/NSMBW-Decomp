#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_heap.hpp>

size_t m3d::anmTexPat_c::child_c::heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexPat anmTexPat, bool calcAligned) {
    size_t size = 0;
    nw4r::g3d::AnmObjTexPatRes::Construct(nullptr, &size, anmTexPat, mdl, false);
    if (calcAligned) {
        size = (mHeap::frmHeapCost(size, 0x20) + 0x1f) & 0xffffffe0;
    }
    return size;
}

bool m3d::anmTexPat_c::child_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexPat anmTexPat, mAllocator_c *allocator, size_t *objSize) {
    if (allocator == nullptr) {
        allocator = m3d::internal::l_allocator_p;
    }

    size_t size;
    if (objSize == nullptr) {
        objSize = &size;
    }
    *objSize = heapCost(mdl, anmTexPat, false);
    if (!createAllocator(allocator, objSize)) {
        return false;
    }

    mpObj = nw4r::g3d::AnmObjTexPatRes::Construct(&mAllocator, nullptr, anmTexPat, mdl, false);
    if (!mpObj->Bind(mdl)) {
        remove();
        return false;
    }
    setFrmCtrlDefault(anmTexPat, PLAYMODE_INHERIT);
    return true;
}

void m3d::anmTexPat_c::child_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmTexPat anmTexPat, m3d::playMode_e playMode) {
    releaseAnm();
    mpObj = nw4r::g3d::AnmObjTexPatRes::Construct(&mAllocator, nullptr, anmTexPat, mdl.getResMdl(), false);
    mpObj->Bind(mdl.getResMdl());
    setFrmCtrlDefault(anmTexPat, playMode);
}

void m3d::anmTexPat_c::child_c::releaseAnm() {
    if (mpObj == nullptr) {
        return;
    }
    mpObj->Release();
    mpHeap->free(3);
    mpObj = nullptr;
}

void m3d::anmTexPat_c::child_c::setFrmCtrlDefault(nw4r::g3d::ResAnmTexPat &anmTexPat, m3d::playMode_e playMode) {
    if (playMode == PLAYMODE_INHERIT) {
        playMode = (anmTexPat.p->mAnimateType == 0) ? FORWARD_ONCE : FORWARD_LOOP;
    }
    fanm_c::set(anmTexPat.getDuration(), playMode, 1, -1);
}

size_t m3d::anmTexPat_c::heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexPat anmTexPat, long count, bool calcAligned) {
    size_t size = 0;
    nw4r::g3d::AnmObjTexPatOverride::Construct(nullptr, &size, mdl, count);
    size += nw4r::ut::RoundUp(count * sizeof(child_c));
    size += nw4r::ut::RoundUp(child_c::heapCost(mdl, anmTexPat, true)) * count;
    if (calcAligned) {
        size = nw4r::ut::RoundUp(mHeap::frmHeapCost(size, 0x20));
    }
    return size;
}

bool m3d::anmTexPat_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmTexPat anmTexPat, mAllocator_c *allocator, size_t *objSize, long count) {
    if (allocator == nullptr) {
        allocator = m3d::internal::l_allocator_p;
    }

    size_t size = 0;
    if (objSize == nullptr) {
        objSize = &size;
    }
    *objSize = heapCost(mdl, anmTexPat, count, false);
    if (!createAllocator(allocator, objSize)) {
        return false;
    }

    mpObj = nw4r::g3d::AnmObjTexPatOverride::Construct(&mAllocator, nullptr, mdl, count);
    children = (m3d::anmTexPat_c::child_c *) MEMAllocFromAllocator(&mAllocator, nw4r::ut::RoundUp(count * sizeof(child_c)));

    nw4r::g3d::AnmObjTexPatOverride *texPatOverride = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexPatOverride>(mpObj);

    child_c *child = &children[0];
    for (int i = 0; i < count; i++) {
        new(child) child_c();
        if (!child->create(mdl, anmTexPat, &mAllocator, nullptr)) {
            mHeap::destroyFrmHeap(mpHeap);
            return false;
        }
        if (i == 0) {
            nw4r::g3d::AnmObjTexPatRes *texPatRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexPatRes>(child->getObj());
            texPatOverride->Attach(i, texPatRes);
        } else {
            child->releaseAnm();
        }
        child++;
    }
    return true;
}

m3d::anmTexPat_c::~anmTexPat_c() {
    anmTexPat_c::remove();
}

void m3d::anmTexPat_c::remove() {
    nw4r::g3d::AnmObjTexPatOverride *texPat = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexPatOverride>(mpObj);
    if (texPat != nullptr && children != nullptr) {
        int count = texPat->getCount();
        for (int i = 0; i < count; i++) {
            children[i].remove();
        }
        children = nullptr;
    }
    banm_c::remove();
}

void m3d::anmTexPat_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmTexPat anmTexPat, long idx, m3d::playMode_e playMode) {
    nw4r::g3d::AnmObjTexPatOverride *texPat = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexPatOverride>(mpObj);
    texPat->Detach(idx);
    children[idx].setAnm(mdl, anmTexPat, playMode);
    nw4r::g3d::AnmObjTexPatRes *texPatRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexPatRes>(children[idx].getObj());
    texPat->Attach(idx, texPatRes);
}

void m3d::anmTexPat_c::releaseAnm(long idx) {
    nw4r::g3d::AnmObjTexPatOverride *texPat = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexPatOverride>(mpObj);
    texPat->Detach(idx);
    children[idx].releaseAnm();
}

void m3d::anmTexPat_c::play() {
    nw4r::g3d::AnmObjTexPatOverride *texPat = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjTexPatOverride>(mpObj);
    int count = texPat->getCount();
    for (int i = 0; i < count; i++) {
        play(i);
    }
}

void m3d::anmTexPat_c::play(long idx) {
    if (children[idx].IsBound()) {
        children[idx].play();
    }
}

float m3d::anmTexPat_c::getFrame(long idx) const {
    return children[idx].getFrame();
}

void m3d::anmTexPat_c::setFrame(float frame, long idx) {
    children[idx].setFrame(frame);
}

float m3d::anmTexPat_c::getRate(long idx) const {
    return children[idx].getRate();
}

void m3d::anmTexPat_c::setRate(float rate, long idx) {
    children[idx].setRate(rate);
}

bool m3d::anmTexPat_c::isStop(long idx) const {
    return children[idx].isStop();
}

bool m3d::anmTexPat_c::checkFrame(float frame, long idx) const {
    return children[idx].checkFrame(frame);
}

void m3d::anmTexPat_c::setPlayMode(m3d::playMode_e mode, long idx) {
    children[idx].mPlayMode = mode;
}

float m3d::anmTexPat_c::getFrameMax(long idx) const {
    return children[idx].mFrameMax;
}
