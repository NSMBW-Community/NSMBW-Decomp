#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_heap.hpp>

size_t m3d::anmMatClr_c::child_c::heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr anmClr, bool calcAligned) {
    size_t size = 0;
    nw4r::g3d::AnmObjMatClrRes::Construct(nullptr, &size, anmClr, mdl, false);
    if (calcAligned) {
        size = nw4r::ut::RoundUp(mHeap::frmHeapCost(size, 0x20));
    }
    return size;
}

bool m3d::anmMatClr_c::child_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr anmClr, mAllocator_c *allocator, size_t *objSize) {
    if (allocator == nullptr) {
        allocator = m3d::internal::l_allocator_p;
    }

    size_t size;
    if (objSize == nullptr) {
        objSize = &size;
    }
    *objSize = heapCost(mdl, anmClr, false);
    if (!createAllocator(allocator, objSize)) {
        return false;
    }

    mpObj = nw4r::g3d::AnmObjMatClrRes::Construct(&mAllocator, nullptr, anmClr, mdl, false);
    if (!mpObj->Bind(mdl)) {
        remove();
        return false;
    }
    setFrmCtrlDefault(anmClr, PLAYMODE_INHERIT);
    return true;
}

void m3d::anmMatClr_c::child_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmClr anmClr, m3d::playMode_e playMode) {
    releaseAnm();
    mpObj = nw4r::g3d::AnmObjMatClrRes::Construct(&mAllocator, nullptr, anmClr, mdl.getResMdl(), false);
    mpObj->Bind(mdl.getResMdl());
    setFrmCtrlDefault(anmClr, playMode);
}

void m3d::anmMatClr_c::child_c::releaseAnm() {
    if (mpObj == nullptr) {
        return;
    }
    mpObj->Release();
    mpHeap->free(3);
    mpObj = nullptr;
}

void m3d::anmMatClr_c::child_c::setFrmCtrlDefault(nw4r::g3d::ResAnmClr &anmClr, m3d::playMode_e playMode) {
    if (playMode == PLAYMODE_INHERIT) {
        playMode = (anmClr.p->mAnimateType == 0) ? FORWARD_ONCE : FORWARD_LOOP;
    }
    fanm_c::set(anmClr.getDuration(), playMode, 1, -1);
}

size_t m3d::anmMatClr_c::heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr anmClr, long count, bool calcAligned) {
    size_t size = 0;
    nw4r::g3d::AnmObjMatClrOverride::Construct(nullptr, &size, mdl, count);
    size += nw4r::ut::RoundUp(count * sizeof(child_c));
    size += nw4r::ut::RoundUp(child_c::heapCost(mdl, anmClr, true)) * count;
    if (calcAligned) {
        size = nw4r::ut::RoundUp(mHeap::frmHeapCost(size, 0x20));
    }
    return size;
}

bool m3d::anmMatClr_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr anmClr, mAllocator_c *allocator, size_t *objSize, long count) {
    if (allocator == nullptr) {
        allocator = m3d::internal::l_allocator_p;
    }

    size_t size = 0;
    if (objSize == nullptr) {
        objSize = &size;
    }
    *objSize = heapCost(mdl, anmClr, count, false);
    if (!createAllocator(allocator, objSize)) {
        return false;
    }

    mpObj = nw4r::g3d::AnmObjMatClrOverride::Construct(&mAllocator, nullptr, mdl, count);
    children = (m3d::anmMatClr_c::child_c *) MEMAllocFromAllocator(&mAllocator, nw4r::ut::RoundUp(count * sizeof(child_c)));

    nw4r::g3d::AnmObjMatClrOverride *matClrOverride = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrOverride>(mpObj);

    child_c *child = &children[0];
    for (int i = 0; i < count; i++) {
        new(child) child_c();
        if (!child->create(mdl, anmClr, &mAllocator, nullptr)) {
            mHeap::destroyFrmHeap(mpHeap);
            return false;
        }
        if (i == 0) {
            nw4r::g3d::AnmObjMatClrRes *clrRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrRes>(child->getObj());
            matClrOverride->Attach(i, clrRes);
        } else {
            child->releaseAnm();
        }
        child++;
    }
    return true;
}

m3d::anmMatClr_c::~anmMatClr_c() {
    anmMatClr_c::remove();
}

void m3d::anmMatClr_c::remove() {
    nw4r::g3d::AnmObjMatClrOverride *matClr = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrOverride>(mpObj);
    if (matClr != nullptr && children != nullptr) {
        int count = matClr->getCount();
        for (int i = 0; i < count; i++) {
            children[i].remove();
        }
        children = nullptr;
    }
    banm_c::remove();
}

void m3d::anmMatClr_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmClr clr, long idx, m3d::playMode_e playMode) {
    nw4r::g3d::AnmObjMatClrOverride *matClr = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrOverride>(mpObj);
    matClr->Detach(idx);
    children[idx].setAnm(mdl, clr, playMode);
    nw4r::g3d::AnmObjMatClrRes *clrRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrRes>(children[idx].getObj());
    matClr->Attach(idx, clrRes);
}

void m3d::anmMatClr_c::releaseAnm(long idx) {
    nw4r::g3d::AnmObjMatClrOverride *matClr = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrOverride>(mpObj);
    matClr->Detach(idx);
    children[idx].releaseAnm();
}

void m3d::anmMatClr_c::play() {
    nw4r::g3d::AnmObjMatClrOverride *matClr = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrOverride>(mpObj);
    int count = matClr->getCount();
    for (int i = 0; i < count; i++) {
        play(i);
    }
}

void m3d::anmMatClr_c::play(long idx) {
    if (children[idx].IsBound()) {
        children[idx].play();
    }
}

float m3d::anmMatClr_c::getFrame(long idx) const {
    return children[idx].getFrame();
}

void m3d::anmMatClr_c::setFrame(float frame, long idx) {
    children[idx].setFrame(frame);
}

float m3d::anmMatClr_c::getRate(long idx) const {
    return children[idx].getRate();
}

void m3d::anmMatClr_c::setRate(float rate, long idx) {
    children[idx].setRate(rate);
}

bool m3d::anmMatClr_c::isStop(long idx) const {
    return children[idx].isStop();
}

bool m3d::anmMatClr_c::checkFrame(float frame, long idx) const {
    return children[idx].checkFrame(frame);
}

void m3d::anmMatClr_c::setPlayMode(m3d::playMode_e playMode, long idx) {
    children[idx].mPlayMode = playMode;
}

float m3d::anmMatClr_c::getFrameMax(long idx) const {
    return children[idx].mFrameMax;
}

float m3d::anmMatClr_c::getFrameStart(long idx) const {
    return children[idx].mFrameStart;
}
