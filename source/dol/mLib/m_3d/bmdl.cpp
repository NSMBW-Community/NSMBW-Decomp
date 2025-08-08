#include <game/mLib/m_3d.hpp>

#include <nw4r/g3d.h>
#include <game/mLib/m_mtx.hpp>

m3d::bmdl_c::~bmdl_c() {
    remove();
}

bool m3d::bmdl_c::getNodeWorldMtx(ulong idx, nw4r::math::MTX34 *mtx) const {
    nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);
    return scnMdl->GetScnMtxPos(mtx, nw4r::g3d::ScnObj::MTX_WORLD, idx);
}

bool m3d::bmdl_c::getNodeWorldMtxMultVecZero(ulong idx, nw4r::math::VEC3 &vec) const {
    mMtx_c mtx;

    if (!getNodeWorldMtx(idx, &mtx)) {
        return false;
    }

    mtx.multVecZero(vec);
    return true;
}

void m3d::bmdl_c::setAnm(m3d::banm_c &anm) {
    if (anm.getType() == banm_c::TYPE_ANM_OTHER) {
        nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
        scnMdl->SetAnmObj(anm.getObj(), nw4r::g3d::ScnMdl::ANMOBJTYPE_NOT_SPECIFIED);
    } else {
        nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);
        if (anm.getType() == banm_c::TYPE_ANM_CHR) {
            mpAnm = &anm;
        }

        scnMdl->SetAnmObj(anm.getObj(), nw4r::g3d::ScnMdl::ANMOBJTYPE_NOT_SPECIFIED);
    }
}

void m3d::bmdl_c::play() {
    if (mpAnm == nullptr) {
        return;
    }

    mpAnm->play();
}

nw4r::g3d::ResMdl m3d::bmdl_c::getResMdl() const {
    nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);
    return scnMdl->GetResMdl();
}

nw4r::g3d::ResMat m3d::bmdl_c::getResMat(size_t idx) const {
    nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);
    nw4r::g3d::ResMdl mdl = scnMdl->GetResMdl();
    return mdl.GetResMat(idx);
}

void m3d::bmdl_c::removeAnm(nw4r::g3d::ScnMdlSimple::AnmObjType objType) {
    nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);
    if (objType == nw4r::g3d::ScnMdl::ANMOBJTYPE_CHR) {
        mpAnm = nullptr;
    }

    scnMdl->RemoveAnmObj(objType);
}

void m3d::bmdl_c::setTevColor(ulong idx, _GXTevRegID regID, _GXColor color, bool markDirty) {
    nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
    if (scnMdl != nullptr) {
        nw4r::g3d::ScnMdl::CopiedMatAccess cma(scnMdl, idx);
        nw4r::g3d::ResMatTevColor tevColor = cma.GetResMatTevColor(markDirty);

        tevColor.GXSetTevColor(regID, color);
        tevColor.DCStore(false);
    } else {
        nw4r::g3d::ResMat resMat = getResMat(idx);
        nw4r::g3d::ResMatTevColor tevColor = resMat.GetResMatTevColor();

        tevColor.GXSetTevColor(regID, color);
        tevColor.DCStore(false);
    }
}

void m3d::bmdl_c::setTevColorAll(_GXTevRegID regID, _GXColor color, bool markDirty) {
    nw4r::g3d::ResMdl resMdl = getResMdl();
    nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
    if (scnMdl != nullptr) {
        for (ulong i = 0; i < resMdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ScnMdl::CopiedMatAccess cma(scnMdl, i);
            nw4r::g3d::ResMatTevColor tevColor = cma.GetResMatTevColor(markDirty);

            tevColor.GXSetTevColor(regID, color);
            tevColor.DCStore(false);
        }
    } else {
        for (size_t i = 0; i < resMdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ResMat resMat = resMdl.GetResMat(i);
            nw4r::g3d::ResMatTevColor tevColor = resMat.GetResMatTevColor();

            tevColor.GXSetTevColor(regID, color);
            tevColor.DCStore(false);
        }
    }
}

void m3d::bmdl_c::setTevKColor(ulong idx, _GXTevKColorID colID, _GXColor color, bool markDirty) {
    nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
    if (scnMdl != nullptr) {
        nw4r::g3d::ScnMdl::CopiedMatAccess cma(scnMdl, idx);
        nw4r::g3d::ResMatTevColor tevColor = cma.GetResMatTevColor(markDirty);

        tevColor.GXSetTevKColor(colID, color);
        tevColor.DCStore(false);
    } else {
        nw4r::g3d::ResMat resMat = getResMat(idx);
        nw4r::g3d::ResMatTevColor tevColor = resMat.GetResMatTevColor();

        tevColor.GXSetTevKColor(colID, color);
        tevColor.DCStore(false);
    }
}

void m3d::bmdl_c::setTevKColorAll(_GXTevKColorID colID, _GXColor color, bool markDirty) {
    nw4r::g3d::ResMdl resMdl = getResMdl();
    nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
    if (scnMdl != nullptr) {
        for (ulong i = 0; i < resMdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ScnMdl::CopiedMatAccess cma(scnMdl, i);
            nw4r::g3d::ResMatTevColor tevColor = cma.GetResMatTevColor(markDirty);

            tevColor.GXSetTevKColor(colID, color);
            tevColor.DCStore(false);
        }
    } else {
        for (size_t i = 0; i < resMdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ResMat resMat = resMdl.GetResMat(i);
            nw4r::g3d::ResMatTevColor tevColor = resMat.GetResMatTevColor();

            tevColor.GXSetTevKColor(colID, color);
            tevColor.DCStore(false);
        }
    }
}

void m3d::bmdl_c::remove() {
    mpAnm = nullptr;
    scnLeaf_c::remove();
}
