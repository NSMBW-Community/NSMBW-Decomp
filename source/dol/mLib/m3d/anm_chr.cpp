#include <dol/mLib/m_3d.hpp>

bool m3d::anmChr_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmChr anmChr, mAllocator_c *allocator, unsigned long *idk) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }
    unsigned long tmp;
    if (idk == nullptr) {
        idk = &tmp;
    }
    nw4r::g3d::AnmObjChrRes::Construct(nullptr, idk, anmChr, mdl, false);
    if (!createAllocator(allocator, idk)) {
        return false;
    }
    mpObj = nw4r::g3d::AnmObjChrRes::Construct(&mAllocator, &tmp, anmChr, mdl, false);
    if (!mpObj->Bind(mdl)) {
        remove();
        return false;
    }
    setFrmCtrlDefault(anmChr, FOUR);
    return true;
}

void m3d::anmChr_c::setAnm(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmChr anmChr, m3d::playMode_e playMode) {
    nw4r::g3d::ScnMdlSimple::AnmObjType anmType = (nw4r::g3d::ScnMdlSimple::AnmObjType) getType();
    mdl.removeAnm(anmType);
    setAnmAfter(mdl, anmChr, playMode);
}

void m3d::anmChr_c::setAnmAfter(m3d::bmdl_c &mdl, nw4r::g3d::ResAnmChr anmChr, m3d::playMode_e playMode) {
    nw4r::g3d::AnmObj *n = mpObj->getNode();
    nw4r::g3d::AnmObjChrNode *chrNode;
    int count;
    float weight;
    if (n != nullptr) {
        chrNode = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrNode>(n);

        count = 0;
        //int end = chrNode->getCount();
        while (count < chrNode->getCount()) {
            if (chrNode->arr[count] != mpObj) {
                count++;
            } else {
                break;
            }
        }
    
        weight = chrNode->GetWeight(count);
        chrNode->Detach(count);
    }
    mpObj->Release();
    mpHeap->free(3);
    unsigned long unused;
    mpObj = nw4r::g3d::AnmObjChrRes::Construct(&mAllocator, &unused, anmChr, mdl.getResMdl(), false);
    mpObj->Bind(mdl.getResMdl());
    setFrmCtrlDefault(anmChr, playMode);
    if (n == nullptr) {
        return;
    }
    nw4r::g3d::AnmObjChrRes *chrRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrRes>(mpObj);
    chrNode->SetWeight(count, weight);
    chrNode->Attach(count, chrRes);
}

void m3d::anmChr_c::setFrmCtrlDefault(nw4r::g3d::ResAnmChr &anmChr, m3d::playMode_e playMode) {
  if (playMode == 4) {
    playMode = (playMode_e) (anmChr.p->x24 == 0);
  }
  fanm_c::set(anmChr.getX20(), playMode, 1.0, -1.0);
}
