#include <game/bases/d_base_actor.hpp>
#include <game/bases/d_reset.hpp>
#include <game/bases/d_scene.hpp>
#include <lib/nw4r/math/trigonometry.hpp>
#include <lib/nw4r/ut/inlines.hpp>
#include <lib/rvl/mtx/mtx.h>

const mVec3_c *dBaseActor_c::m_tmpCtPosP;
const mAng3_c *dBaseActor_c::m_tmpCtAngleP;
fLiMgBa_c dBaseActor_c::m_actorManage;

dBaseActor_c::dBaseActor_c() :
mLinkActor(this),
mPos(),
mLastPos(),
mAngle(),
mAngle3D()
{
    // Append the actor to the list
    m_actorManage.append(&mLinkActor);

    // Copy position and angle if set
    if (m_tmpCtPosP != nullptr) {
        mPos = *m_tmpCtPosP;
        mLastPos = *m_tmpCtPosP;
    }

    if (m_tmpCtAngleP != nullptr) {
        mAngle = *m_tmpCtAngleP;
        mAngle3D = *m_tmpCtAngleP;
    }

    // Initialize the rest
    mVisible = true;

    mCenterOffs.x = 0.0f;
    mCenterOffs.y = 0.0f;
    mCenterOffs.z = 0.0f;

    mScale.x = 1.0f;
    mScale.y = 1.0f;
    mScale.z = 1.0f;

    fProfile::fProfilePtr_c profile = (*fProfile::sProfileList)[mProfName];
    mActorProperties = profile.mActorProfile->mActorProperties;
}

dBaseActor_c::~dBaseActor_c() {
    m_actorManage.remove(&mLinkActor);
}

int dBaseActor_c::preCreate() {
    return (dBase_c::preCreate() != NOT_READY);
}

void dBaseActor_c::postCreate(fBase_c::MAIN_STATE_e status) {
    dBase_c::postCreate(status);
}

int dBaseActor_c::preDelete() {
    return (dBase_c::preDelete() != NOT_READY);
}

void dBaseActor_c::postDelete(fBase_c::MAIN_STATE_e status) {
    dBase_c::postDelete(status);
}

int dBaseActor_c::preExecute() {
    if (dBase_c::preExecute() == NOT_READY) {
        return NOT_READY;
    } else {

        // Make sure the game isn't being reset
        dReset::Manage_c *manage = dReset::Manage_c::GetInstance();
        return !(
            (manage->mModeInit == dReset::Manage_c::SOFT_RESET ||
            manage->mModeProc == dReset::Manage_c::SOFT_RESET) ||
            (manage->mModeInit == dReset::Manage_c::SAFETY_WAIT ||
            manage->mModeProc == dReset::Manage_c::SAFETY_WAIT)
            );
    }
}

void dBaseActor_c::postExecute(fBase_c::MAIN_STATE_e status) {
    if (status == SUCCESS) {
        mPosDelta = mPos - mLastPos;
        mLastPos = mPos;
    }

    dBase_c::postExecute(status);
}

int dBaseActor_c::preDraw() {
    if (dBase_c::preDraw() == NOT_READY) {
        return NOT_READY;
    } else {
        return mVisible != 0;
    }
}

void dBaseActor_c::postDraw(fBase_c::MAIN_STATE_e status) {
    dBase_c::postDraw(status);
}

void dBaseActor_c::draw2D() {}

void dBaseActor_c::draw2D_lyt2() {}

void dBaseActor_c::setTmpCtData(const mVec3_c *position, const mAng3_c *rotation) {
    m_tmpCtPosP = position;
    m_tmpCtAngleP = rotation;
}

dBaseActor_c *dBaseActor_c::construct(ProfileName profName, unsigned long param, const mVec3_c *position, const mAng3_c *rotation) {
    setTmpCtData(position, rotation);

    // Find the current scene actor and set it as parent
    dBase_c *parent = dBase_c::searchBaseByProfName(dScene_c::m_nowScene, nullptr);
    return (dBaseActor_c *)dBase_c::createBase(profName, parent, param, ACTOR);
}

void dBaseActor_c::draw2DActorOnLyt1() {
    dBaseActor_c *currActor = nullptr;

    while (currActor = (dBaseActor_c *)fManager_c::searchBaseByGroupType(fBase_c::ACTOR, currActor), currActor != nullptr) {
        currActor->draw2D();
    }
}

void dBaseActor_c::draw2DActorOnLyt2() {
    dBaseActor_c *currActor = nullptr;

    while (currActor = (dBaseActor_c *)fManager_c::searchBaseByGroupType(fBase_c::ACTOR, currActor), currActor != nullptr) {
        currActor->draw2D_lyt2();
    }
}

dBaseActor_c *dBaseActor_c::construct(ProfileName profName, dBase_c *parent, unsigned long param, const mVec3_c *position, const mAng3_c *rotation) {
    setTmpCtData(position, rotation);
    return (dBaseActor_c *)dBase_c::createBase(profName, parent, param, ACTOR);
}

void dBaseActor_c::calcSpeed() {
    float sin = mAngle3D.y.sin();
    float cos = mAngle3D.y.cos();

    // Distribute mSpeedF on the X and Z axes according to the actor's rotation and use the regular Y speed
    // [Defining newZ is required for matching]
    float newZ = mSpeedF * cos;
    mSpeed.y = nw4r::ut::Max<float>(mSpeed.y + mAccelY, mMaxFallSpeed);
    mSpeed.x = mSpeedF * sin;
    mSpeed.z = newZ;
}

void dBaseActor_c::makeMtx() {
    mMatrix.trans(mPos.x, mPos.y, mPos.z);
    mMatrix.YrotM(mAngle.y);
}

void dBaseActor_c::posMove(mVec3_c &delta) {
    mPos += delta;
}

void dBaseActor_c::posMove() {
    posMove(mSpeed);
}

void dBaseActor_c::calcSpeedXY() {
    calcSpeedF();
    mSpeed.x = mSpeedF;
    calcFallSpeed();
}

void dBaseActor_c::calcSpeedX() {
    float newSpeed = mSpeed.x;

    // If the X speed hasn't reached the max, increase it until it reaches the limit
    // Else decrease it until the limit
    if (mSpeed.x < mSpeedMax.x) {
        newSpeed += mAccelF;
        if (newSpeed > mSpeedMax.x) {
            newSpeed = mSpeedMax.x;
        }
    } else if (mSpeed.x > mSpeedMax.x) {
        newSpeed -= mAccelF;
        if (newSpeed < mSpeedMax.x) {
            newSpeed = mSpeedMax.x;
        }
    }

    mSpeed.x = newSpeed;
}

void dBaseActor_c::calcSpeedY() {
    // [Since Y values are assumed negative, the comparison is the other way]
    float newSpeed = mSpeed.y + mAccelY;
    if (newSpeed < mSpeedMax.y) {
        newSpeed = mSpeedMax.y;
    }

    mSpeed.y = newSpeed;
}

void dBaseActor_c::calcSpeedF() {
    // If the speed hasn't exceeded the limit, increase it until the limit is reached
    // Else decrease it until the limit
    if (mSpeedF < mMaxSpeedF) {
        mSpeedF = nw4r::ut::Min<float>(mSpeedF + mAccelF, mMaxSpeedF);
    } else if (mSpeedF > mMaxSpeedF) {
        mSpeedF = nw4r::ut::Max<float>(mSpeedF - mAccelF, mMaxSpeedF);
    }
}

void dBaseActor_c::calcFallSpeed() {
    // If the speed has exceeded the limit, decrease it until the limit is reached
    // Else increase it until the limit
    if (mSpeed.y < mMaxFallSpeed) {
        mSpeed.y = nw4r::ut::Min<float>(mSpeed.y - mAccelY, mMaxFallSpeed);
    } else if (mSpeed.y > mMaxFallSpeed) {
        mSpeed.y = nw4r::ut::Max<float>(mSpeed.y + mAccelY, mMaxFallSpeed);
    }
}

mVec3_c dBaseActor_c::getCenterPos() const {
    return mPos + mCenterOffs;
}

int dBaseActor_c::GetProfNameActorNum(ProfileName profile) {
    return dBaseActor_c::m_actorManage.countNodeByProfName(profile);
}
