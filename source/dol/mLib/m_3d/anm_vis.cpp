#include <game/mLib/m_3d.hpp>

bool m3d::anmVis_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmVis anmVis, mAllocator_c *allocator, size_t *objSize) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }

    size_t size;
    if (objSize == nullptr) {
        objSize = &size;
    }

    nw4r::g3d::AnmObjVisRes::Construct(nullptr, objSize, anmVis, mdl);
    if (!createAllocator(allocator, objSize)) {
        return false;
    }

    mpObj = nw4r::g3d::AnmObjVisRes::Construct(&mAllocator, &size, anmVis, mdl);
    if (!mpObj->Bind(mdl)) {
        remove();
        return false;
    }
    setFrmCtrlDefault(anmVis, PLAYMODE_INHERIT);
    return true;
}

void m3d::anmVis_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmVis anmVis, m3d::playMode_e playMode) {
    nw4r::g3d::ScnMdlSimple::AnmObjType anmType = (nw4r::g3d::ScnMdlSimple::AnmObjType) getType();
    mdl.removeAnm(anmType);
    mpObj->Release();
    mpHeap->free(MEM_FRMHEAP_FREE_ALL);

    size_t size;
    mpObj = nw4r::g3d::AnmObjVisRes::Construct(&mAllocator, &size, anmVis, mdl.getResMdl());
    mpObj->Bind(mdl.getResMdl());
    setFrmCtrlDefault(anmVis, playMode);
}

void m3d::anmVis_c::setFrmCtrlDefault(nw4r::g3d::ResAnmVis &anmVis, m3d::playMode_e playMode) {
    if (playMode == PLAYMODE_INHERIT) {
        playMode = (anmVis.p->mAnimateType == nw4r::g3d::ANM_POLICY_ONCE) ? FORWARD_ONCE : FORWARD_LOOP;
    }
    fanm_c::set(anmVis.getDuration(), playMode, 1.0f, -1.0f);
}
