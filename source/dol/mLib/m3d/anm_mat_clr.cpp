#include <dol/mLib/m_3d.hpp>

#include <dol/mLib/m_heap.hpp>

unsigned long m3d::anmMatClr_c::child_c::heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr anmClr, bool b) {
    unsigned long cost = 0;
    nw4r::g3d::AnmObjMatClrRes::Construct(nullptr, &cost, anmClr, mdl, false);
    if (b) {
        cost = (mHeap::frmHeapCost(cost, 0x20) + 0x1f) & 0xffffffe0;
    }
    return cost;
}

bool m3d::anmMatClr_c::child_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr anmClr, mAllocator_c *allocator, unsigned long *idk) {
    if (allocator == nullptr) {
        allocator = m3d::internal::l_allocator_p;
    }
    unsigned long tmp;
    if (idk == nullptr) {
        idk = &tmp;
    }
    *idk = heapCost(mdl, anmClr, false);
    if (!createAllocator(allocator, idk)) {
        return false;
    }
    mpObj = nw4r::g3d::AnmObjMatClrRes::Construct(&mAllocator, nullptr, anmClr, mdl, false);
    if (!mpObj->Bind(mdl)) {
        remove();
        return false;
    }
    setFrmCtrlDefault(anmClr, FOUR);
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
  if (playMode == 4) {
    playMode = (playMode_e) (anmClr.p->x24 == 0);
  }
  fanm_c::set(anmClr.getX20(), playMode, 1.0, -1.0);
}

unsigned long m3d::anmMatClr_c::heapCost(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr anmClr, long l, bool b) {
    unsigned long cost = 0;
    nw4r::g3d::AnmObjMatClrOverride::Construct(nullptr, &cost, mdl, l);
    cost += align32Bit(l * sizeof(child_c));
    cost += align32Bit(child_c::heapCost(mdl, anmClr, true)) * l;
    if (b) {
        cost = align32Bit(mHeap::frmHeapCost(cost, 0x20));
    }
    return cost;
}

bool m3d::anmMatClr_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmClr anmClr, mAllocator_c *allocator, unsigned long *idk, long l) {
    if (allocator == nullptr) {
        allocator = m3d::internal::l_allocator_p;
    }
    unsigned long tmp = 0;
    if (idk == nullptr) {
        idk = &tmp;
    }
    *idk = heapCost(mdl, anmClr, l, false);
    if (!createAllocator(allocator, idk)) {
        return false;
    }
    mpObj = nw4r::g3d::AnmObjMatClrOverride::Construct(&mAllocator, nullptr, mdl, l);
    children = (m3d::anmMatClr_c::child_c *) MEMAllocFromAllocator(&mAllocator, align32Bit(l * sizeof(child_c)));

    nw4r::g3d::AnmObjMatClrOverride *matClr = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrOverride>(mpObj);

    child_c *child = children;
    for (int i = 0; i < l; i++) {
        new(child) child_c();
        if (!child->create(mdl, anmClr, &mAllocator, nullptr)) {
            mHeap::destroyFrmHeap(mpHeap);
            return false;
        }
        if (i == 0) {
            nw4r::g3d::AnmObjMatClrRes *clrRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrRes>(child->mpObj);
            matClr->Attach(i, clrRes);
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

void m3d::anmMatClr_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmClr clr, long i, m3d::playMode_e playMode) {
    nw4r::g3d::AnmObjMatClrOverride *matClr = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrOverride>(mpObj);
    matClr->Detach(i);
    children[i].setAnm(mdl, clr, playMode);
    nw4r::g3d::AnmObjMatClrRes *clrRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrRes>(children[i].mpObj);
    matClr->Attach(i, clrRes);
}

void m3d::anmMatClr_c::releaseAnm(long i) {
    nw4r::g3d::AnmObjMatClrOverride *matClr = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrOverride>(mpObj);
    matClr->Detach(i);
    children[i].releaseAnm();
}

void m3d::anmMatClr_c::play() {
    nw4r::g3d::AnmObjMatClrOverride *matClr = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjMatClrOverride>(mpObj);
    int count = matClr->getCount();
    for (int i = 0; i < count; i++) {
        play(i);
    }
}

void m3d::anmMatClr_c::play(long i) {
    if (children[i].IsBound()) {
        children[i].play();
    }
}

float m3d::anmMatClr_c::getFrame(long i) const {
    return children[i].getFrame();
}

void m3d::anmMatClr_c::setFrame(float frame, long i) {
    children[i].setFrame(frame);
}

float m3d::anmMatClr_c::getRate(long i) const {
    return children[i].getRate();
}

void m3d::anmMatClr_c::setRate(float rate, long i) {
    children[i].setRate(rate);
}

bool m3d::anmMatClr_c::isStop(long i) const {
    return children[i].isStop();
}

bool m3d::anmMatClr_c::checkFrame(float frame, long i) const {
    return children[i].checkFrame(frame);
}

void m3d::anmMatClr_c::setPlayMode(m3d::playMode_e mode, long i) {
    children[i].mPlayMode = mode;
}

float m3d::anmMatClr_c::getFrameMax(long i) const {
    return children[i].mF1;
}

float m3d::anmMatClr_c::getFrameStart(long i) const {
    return children[i].mF2;
}

int m3d::anmMatClr_c::getType() const {
    return 2;
}
