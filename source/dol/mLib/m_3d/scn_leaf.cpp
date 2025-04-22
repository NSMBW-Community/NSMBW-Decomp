#include <game/mLib/m_3d.hpp>

m3d::scnLeaf_c::scnLeaf_c() {
    mpScn = nullptr;
}

m3d::scnLeaf_c::~scnLeaf_c() {
    scnLeaf_c::remove();
}

void m3d::scnLeaf_c::remove() {
    if (mpScn == nullptr) {
        return;
    }
    mpScn->Destroy();
    mpScn = nullptr;
}

void m3d::scnLeaf_c::entry() {
    m3d::pushBack(mpScn);
}

void m3d::scnLeaf_c::setOption(unsigned long sel, unsigned long opt) {
    mpScn->SetScnObjOption(sel, opt);
}

bool m3d::scnLeaf_c::getOption(unsigned long sel, unsigned long *opt) const {
    return mpScn->GetScnObjOption(sel, opt);
}

void m3d::scnLeaf_c::setScale(float x, float y, float z) {
    mpScn->SetScale(x, y, z);
}

void m3d::scnLeaf_c::setScale(const nw4r::math::VEC3 &scale) {
    mpScn->SetScale(scale);
}

void m3d::scnLeaf_c::setLocalMtx(const nw4r::math::MTX34 *mtx) {
    mpScn->SetMtx(nw4r::g3d::ScnObj::TYPE_LOCAL, mtx);
}

void m3d::scnLeaf_c::getLocalMtx(nw4r::math::MTX34 *mtx) const {
    mpScn->GetMtx(nw4r::g3d::ScnObj::TYPE_LOCAL, mtx);
}

void m3d::scnLeaf_c::getViewMtx(nw4r::math::MTX34 *mtx) const {
    mpScn->GetMtx(nw4r::g3d::ScnObj::TYPE_VIEW, mtx);
}

void m3d::scnLeaf_c::calc(bool keepEnabledAfter) {
    setOption(2, 0);
    mpScn->G3dProc(1, 0, nullptr);
    if (!keepEnabledAfter) {
        setOption(2, 1);
    }
}

void m3d::scnLeaf_c::calcVtx(bool keepEnabledAfter) {
    setOption(4, 0);
    mpScn->G3dProc(3, 0, nullptr);
    if (!keepEnabledAfter) {
        setOption(4, 1);
    }
}

void m3d::scnLeaf_c::setPriorityDraw(int prioOpa, int prioXlu) {
    mpScn->SetPriorityDrawOpa(prioOpa);
    mpScn->SetPriorityDrawXlu(prioXlu);
}
