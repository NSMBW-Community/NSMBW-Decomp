#include <dol/mLib/m_3d.hpp>

bool m3d::anmVis_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmVis anmVis, mAllocator_c *allocator, unsigned long *idk) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }
    unsigned long tmp;
    if (idk == nullptr) {
        idk = &tmp;
    }
    nw4r::g3d::AnmObjVisRes::Construct(nullptr, idk, anmVis, mdl);
    if (!createAllocator(allocator, idk)) {
        return false;
    }
    mpObj = nw4r::g3d::AnmObjVisRes::Construct(&mAllocator, &tmp, anmVis, mdl);
    if (!mpObj->Bind(mdl)) {
        remove();
        return false;
    }
    setFrmCtrlDefault(anmVis, FOUR);
    return true;
}

void m3d::anmVis_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmVis anmVis, m3d::playMode_e playMode) {
    nw4r::g3d::ScnMdlSimple::AnmObjType anmType = (nw4r::g3d::ScnMdlSimple::AnmObjType) getType();
    mdl.removeAnm(anmType);
    mpObj->Release();
    mpHeap->free(3);
    unsigned long unused;
    mpObj = nw4r::g3d::AnmObjVisRes::Construct(&mAllocator, &unused, anmVis, mdl.getResMdl());
    mpObj->Bind(mdl.getResMdl());
    setFrmCtrlDefault(anmVis, playMode);
}

void m3d::anmVis_c::setFrmCtrlDefault(nw4r::g3d::ResAnmVis &anmVis, m3d::playMode_e playMode) {
  if (playMode == 4) {
    playMode = (playMode_e) (anmVis.p->x24 == 0);
  }
  fanm_c::set(anmVis.getX20(), playMode, 1.0, -1.0);
}
