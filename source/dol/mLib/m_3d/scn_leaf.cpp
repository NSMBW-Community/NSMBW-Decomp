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

void m3d::scnLeaf_c::setOption(ulong option, ulong value) {
    mpScn->SetScnObjOption(option, value);
}

bool m3d::scnLeaf_c::getOption(ulong option, ulong *value) const {
    return mpScn->GetScnObjOption(option, value);
}

void m3d::scnLeaf_c::setScale(float x, float y, float z) {
    mpScn->SetScale(x, y, z);
}

void m3d::scnLeaf_c::setScale(const nw4r::math::VEC3 &scale) {
    mpScn->SetScale(scale);
}

void m3d::scnLeaf_c::setLocalMtx(const nw4r::math::MTX34 *mtx) {
    mpScn->SetMtx(nw4r::g3d::ScnObj::MTX_LOCAL, mtx);
}

void m3d::scnLeaf_c::getLocalMtx(nw4r::math::MTX34 *mtx) const {
    mpScn->GetMtx(nw4r::g3d::ScnObj::MTX_LOCAL, mtx);
}

void m3d::scnLeaf_c::getViewMtx(nw4r::math::MTX34 *mtx) const {
    mpScn->GetMtx(nw4r::g3d::ScnObj::MTX_VIEW, mtx);
}

void m3d::scnLeaf_c::calc(bool keepEnabledAfter) {
    setOption(nw4r::g3d::ScnObj::OPTID_DISABLE_CALC_WORLD, 0);
    mpScn->G3dProc(nw4r::g3d::G3dObj::G3DPROC_CALC_WORLD, 0, nullptr);
    if (!keepEnabledAfter) {
        setOption(nw4r::g3d::ScnObj::OPTID_DISABLE_CALC_WORLD, 1);
    }
}

void m3d::scnLeaf_c::calcVtx(bool keepEnabledAfter) {
    setOption(nw4r::g3d::ScnObj::OPTID_DISABLE_CALC_VTX, 0);
    mpScn->G3dProc(nw4r::g3d::G3dObj::G3DPROC_CALC_VTX, 0, nullptr);
    if (!keepEnabledAfter) {
        setOption(nw4r::g3d::ScnObj::OPTID_DISABLE_CALC_VTX, 1);
    }
}

void m3d::scnLeaf_c::setPriorityDraw(int prioOpa, int prioXlu) {
    mpScn->SetPriorityDrawOpa(prioOpa);
    mpScn->SetPriorityDrawXlu(prioXlu);
}
