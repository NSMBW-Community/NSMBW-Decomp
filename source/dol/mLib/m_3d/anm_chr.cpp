#include <game/mLib/m_3d.hpp>
#include <lib/nw4r/g3d/anm_obj_chr.hpp>

bool m3d::anmChr_c::create(nw4r::g3d::ResMdl mdl, nw4r::g3d::ResAnmChr anmChr, mAllocator_c *allocator, size_t *objSize) {
    if (allocator == nullptr) {
        allocator = internal::l_allocator_p;
    }

    size_t size;
    if (objSize == nullptr) {
        objSize = &size;
    }

    nw4r::g3d::AnmObjChrRes::Construct(nullptr, objSize, anmChr, mdl, false);
    if (!createAllocator(allocator, objSize)) {
        return false;
    }

    mpObj = nw4r::g3d::AnmObjChrRes::Construct(&mAllocator, &size, anmChr, mdl, false);
    if (!mpObj->Bind(mdl)) {
        remove();
        return false;
    }

    setFrmCtrlDefault(anmChr, PLAYMODE_INHERIT);
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
    int nodeIdx;
    float weight;
    if (n != nullptr) {
        chrNode = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrNode>(n);

        nodeIdx = 0;
        while (nodeIdx < chrNode->getCount()) {
            if (chrNode->arr[nodeIdx] != mpObj) {
                nodeIdx++;
            } else {
                break;
            }
        }

        weight = chrNode->GetWeight(nodeIdx);
        chrNode->Detach(nodeIdx);
    }

    mpObj->Release();
    mpHeap->free(3);

    size_t size;
    mpObj = nw4r::g3d::AnmObjChrRes::Construct(&mAllocator, &size, anmChr, mdl.getResMdl(), false);
    mpObj->Bind(mdl.getResMdl());
    setFrmCtrlDefault(anmChr, playMode);
    if (n == nullptr) {
        return;
    }

    // Newly constructed object is at the last position
    nw4r::g3d::AnmObjChrRes *chrRes = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::AnmObjChrRes>(mpObj);
    chrNode->SetWeight(nodeIdx, weight);
    chrNode->Attach(nodeIdx, chrRes);
}

void m3d::anmChr_c::setFrmCtrlDefault(nw4r::g3d::ResAnmChr &anmChr, m3d::playMode_e playMode) {
    if (playMode == PLAYMODE_INHERIT) {
        playMode = (anmChr.p->mAnimateType == 0) ? FORWARD_ONCE : FORWARD_LOOP;
    }
    fanm_c::set(anmChr.getDuration(), playMode, 1, -1);
}
