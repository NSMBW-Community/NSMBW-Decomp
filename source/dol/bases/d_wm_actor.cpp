#include <game/bases/d_wm_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_w_camera.hpp>
#include <game/bases/d_info.hpp>

dWmActor_c::dWmActor_c() {}

dWmActor_c::~dWmActor_c() {}

int dWmActor_c::preCreate() {
    return dBaseActor_c::preCreate() != NOT_READY;
}

void dWmActor_c::postCreate(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postCreate(status);
}

int dWmActor_c::preDelete() {
    if (dBaseActor_c::preDelete() == NOT_READY) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

void dWmActor_c::postDelete(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postDelete(status);
}

int dWmActor_c::preExecute() {
    if (dBaseActor_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    if (dGameCom::isGameStop(-1)) {
        return NOT_READY;
    }

    if (dInfo_c::m_instance->field_394) {
        if (mProfName != fProfile::WM_CS_SEQ_MNG
            && mProfName != fProfile::WM_ISLAND
            && mProfName != fProfile::WM_DIRECTOR) {
            return NOT_READY;
        }
    }

    mCullSphere.mPos = mPos;
    
    return SUCCEEDED;
}

void dWmActor_c::postExecute(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postExecute(status);
}

int dWmActor_c::preDraw() {
    if (dBaseActor_c::preDraw() == NOT_READY) {
        return NOT_READY;
    }

    mCullSphere.mPos = mPos;
    
    if (!mCullSphere.isZero() && !dWCamera_c::m_instance->mUnk.intersectSphere(&mCullSphere)) {
        return NOT_READY;
    }
    
    if (dInfo_c::m_instance->field_394) {
        if (mProfName != fProfile::WM_CS_SEQ_MNG
            && mProfName != fProfile::WM_ISLAND
            && mProfName != fProfile::WM_DIRECTOR) {
            return NOT_READY;
        }
    }
    
    return SUCCEEDED;
}

void dWmActor_c::postDraw(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postDraw(status);
}

dWmActor_c *dWmActor_c::construct(ProfileName profName, unsigned long param, const mVec3_c *position, const mAng3_c *rotation) {
    return (dWmActor_c *) dBaseActor_c::construct(profName, param, position, rotation);
}

dWmActor_c *dWmActor_c::construct(ProfileName profName, dBase_c *base, unsigned long param, const mVec3_c *position, const mAng3_c *rotation) {
    return (dWmActor_c *) dBaseActor_c::construct(profName, base, param, position, rotation);
}

void dWmActor_c::setSoftLight_Map(m3d::bmdl_c &mdl) {
    dGameCom::SetSoftLight_Map(mdl, 0);
}

void dWmActor_c::setSoftLight_MapObj(m3d::bmdl_c &mdl) {
    dGameCom::SetSoftLight_MapObj(mdl, 0);
}

void dWmActor_c::setSoftLight_Enemy(m3d::bmdl_c &mdl) {
    dGameCom::SetSoftLight_Enemy(mdl, 0);
}

void dWmActor_c::setSoftLight_Boss(m3d::bmdl_c &mdl) {
    dGameCom::SetSoftLight_Boss(mdl, 0);
}

int dWmActor_c::GetActorType() {
    return ACTOR_MAP_GENERIC;
}
