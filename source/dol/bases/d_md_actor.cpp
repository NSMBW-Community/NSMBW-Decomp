#include <game/bases/d_md_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_w_camera.hpp>
#include <game/bases/d_info.hpp>

dMdActor_c::dMdActor_c() {}

dMdActor_c::~dMdActor_c() {}

int dMdActor_c::preCreate() {
    return dBaseActor_c::preCreate() != NOT_READY;
}

void dMdActor_c::postCreate(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postCreate(status);
}

int dMdActor_c::preDelete() {
    if (dBaseActor_c::preDelete() == NOT_READY) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

void dMdActor_c::postDelete(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postDelete(status);
}

int dMdActor_c::preExecute() {
    if (dBaseActor_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    if (dGameCom::isGameStop(-1)) {
        return NOT_READY;
    }

    if (dInfo_c::m_instance->mDisable3DScenes) {
        return NOT_READY;
    }

    mCullSphere.mPos = mPos;
    
    return SUCCEEDED;
}

void dMdActor_c::postExecute(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postExecute(status);
}

int dMdActor_c::preDraw() {
    if (dBaseActor_c::preDraw() == NOT_READY) {
        return NOT_READY;
    }

    mCullSphere.mPos = mPos;
    
    if (!mCullSphere.isZero() && !dWCamera_c::m_instance->mViewClip.intersectSphere(&mCullSphere)) {
        return NOT_READY;
    }
    
    if (dInfo_c::m_instance->mDisable3DScenes) {
        return NOT_READY;
    }
    
    return SUCCEEDED;
}

void dMdActor_c::postDraw(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postDraw(status);
}

dMdActor_c *dMdActor_c::construct(ProfileName profName, dBase_c *base, unsigned long param, const mVec3_c *position, const mAng3_c *rotation) {
    return (dMdActor_c *) dBaseActor_c::construct(profName, base, param, position, rotation);
}

int dMdActor_c::GetActorType() {
    return ACTOR_MAP_GENERIC;
}
