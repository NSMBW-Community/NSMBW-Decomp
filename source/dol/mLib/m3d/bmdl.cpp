#include <game/mLib/m_3d.hpp>

#include <lib/nw4r/g3d/scn_mdl.hpp>
#include <game/mLib/m_mtx.hpp>

m3d::bmdl_c::~bmdl_c() {
    remove();
}

bool m3d::bmdl_c::getNodeWorldMtx(unsigned long i, nw4r::math::MTX34 *mtx) const {
    nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);
    
    return scnMdl->GetScnMtxPos(mtx, nw4r::g3d::ScnObj::TYPE_UNK, i);
}

bool m3d::bmdl_c::getNodeWorldMtxMultVecZero(unsigned long i, nw4r::math::VEC3 &vec) const {
    mMtx_c mtx;

    if (!getNodeWorldMtx(i, &mtx)) {
        return false;
    }

    mtx.multVecZero(vec);
    return true;
}

void m3d::bmdl_c::setAnm(m3d::banm_c &anm) {
    if (anm.getType() == 5) {
        nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
        scnMdl->SetAnmObj(anm.getObj(), nw4r::g3d::ScnMdl::SIX);
    } else {
        nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);

        if (anm.getType() == 0) {
            mpAnm = &anm;
        }
        
        scnMdl->SetAnmObj(anm.getObj(), nw4r::g3d::ScnMdl::SIX);
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
    return scnMdl->mpResMdl;
}

nw4r::g3d::ResMat *m3d::bmdl_c::getResMat(unsigned long v) const {
    nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);
    nw4r::g3d::ResMdl mdl = scnMdl->mpResMdl;
    return mdl.GetResMat(v);
}

void m3d::bmdl_c::removeAnm(nw4r::g3d::ScnMdlSimple::AnmObjType objType) {
    nw4r::g3d::ScnMdlSimple *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdlSimple>(mpScn);

    if (objType == nw4r::g3d::ScnMdl::ZERO) {
        mpAnm = nullptr;
    }
    
    scnMdl->RemoveAnmObj(objType);
}


void m3d::bmdl_c::setTevColor(unsigned long idx, _GXTevRegID regID, _GXColor color, bool b) {
    nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
    if (scnMdl != nullptr) {
        nw4r::g3d::ScnMdl::CopiedMatAccess cma(scnMdl, idx);
        nw4r::g3d::ResMatTevColor tevColor = cma.GetResMatTevColor(b);
        
        tevColor.GXSetTevColor(regID, color);
        tevColor.DCStore(false);
    } else {
        nw4r::g3d::ResMat *resMat = getResMat(idx);

        nw4r::g3d::ResMatTevColor tevColor = resMat->GetResMatTevColor();

        tevColor.GXSetTevColor(regID, color);
        tevColor.DCStore(false);
    }
}

void m3d::bmdl_c::setTevColorAll(_GXTevRegID regID, _GXColor color, bool b) {
    nw4r::g3d::ResMdl resMdl = getResMdl();
    nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
    if (scnMdl != nullptr) {
        for (unsigned long i = 0; i < resMdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ScnMdl::CopiedMatAccess cma(scnMdl, i);
            nw4r::g3d::ResMatTevColor tevColor = cma.GetResMatTevColor(b);
            
            tevColor.GXSetTevColor(regID, color);
            tevColor.DCStore(false);
        }
    } else {
        for (unsigned long i = 0; i < resMdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ResMat *resMat = resMdl.GetResMat(i);
    
            nw4r::g3d::ResMatTevColor tevColor = resMat->GetResMatTevColor();
    
            tevColor.GXSetTevColor(regID, color);
            tevColor.DCStore(false);
        }
    }
}

void m3d::bmdl_c::setTevKColor(unsigned long idx, _GXTevKColorID colID, _GXColor color, bool b) {
    nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
    if (scnMdl != nullptr) {
        nw4r::g3d::ScnMdl::CopiedMatAccess cma(scnMdl, idx);
        nw4r::g3d::ResMatTevColor tevColor = cma.GetResMatTevColor(b);
        
        tevColor.GXSetTevKColor(colID, color);
        tevColor.DCStore(false);
    } else {
        nw4r::g3d::ResMat *resMat = getResMat(idx);

        nw4r::g3d::ResMatTevColor tevColor = resMat->GetResMatTevColor();

        tevColor.GXSetTevKColor(colID, color);
        tevColor.DCStore(false);
    }
}

void m3d::bmdl_c::setTevKColorAll(_GXTevKColorID colID, _GXColor color, bool b) {
    nw4r::g3d::ResMdl resMdl = getResMdl();
    nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mpScn);
    if (scnMdl != nullptr) {
        for (unsigned long i = 0; i < resMdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ScnMdl::CopiedMatAccess cma(scnMdl, i);
            nw4r::g3d::ResMatTevColor tevColor = cma.GetResMatTevColor(b);
            
            tevColor.GXSetTevKColor(colID, color);
            tevColor.DCStore(false);
        }
    } else {
        for (unsigned long i = 0; i < resMdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ResMat *resMat = resMdl.GetResMat(i);
    
            nw4r::g3d::ResMatTevColor tevColor = resMat->GetResMatTevColor();
    
            tevColor.GXSetTevKColor(colID, color);
            tevColor.DCStore(false);
        }
    }
}

void m3d::bmdl_c::remove() {
    mpAnm = nullptr;
    scnLeaf_c::remove();
}

int m3d::bmdl_c::getType() const {
    return 0;
}
