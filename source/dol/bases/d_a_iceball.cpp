#include <game/bases/d_a_iceball.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_s_stage.hpp>
#include <constants/sound_list.h>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <game/bases/d_effactor_mng.hpp>
#include <game/bases/d_bg.hpp>
#include <game/framework/f_profile.hpp>
#include <game/framework/f_profile_name.hpp>


ACTOR_PROFILE(ICEBALL, daIceBall_c, 2);

int daIceBall_c::sm_IceBallCount[4] = {0, 0, 0, 0};
int daIceBall_c::sm_IceBallAliveCount[4] = {0, 0, 0, 0};

const float lbl_802F5000[4] = {0.75f, 1.05f, 0.6f, 120.0f};

const dBcSensor_c l_iceball_foot = { 0.0f, 0, -0x3000 };
const dBcSensor_c l_iceball_head = { 0.0f, 0, 0x3000 };
const dBcSensor_c l_iceball_wall = { 0.0f, 0x3000, 0 };

const sCcDatNewF l_fball_cc_data = {
    0.0f, 0.0f,
    6.0f, 6.0f,
    6, 2,
    0xFA,
    0x8050,
    0,
    daIceBall_c::ccCallback_Iceball
};

const float l_cull_data[4] = { 0.0f, 0.0f, 8.0f, 8.0f };

STATE_DEFINE(daIceBall_c, FireMove);
STATE_DEFINE(daIceBall_c, Move);
STATE_DEFINE(daIceBall_c, EatIn);
STATE_DEFINE(daIceBall_c, EatNow);
STATE_DEFINE(daIceBall_c, Kill);

int daIceBall_c::create() {

    mPlayerNo = mParam & 3;

    bool v9 = CheckIceballLimit(mPlayerNo, (mParam >> 16) & 3);

    m_3d0 = 60;

    sm_IceBallCount[mPlayerNo]++;
    sm_IceBallAliveCount[mPlayerNo]++;

    if (! v9) {
        deleteRequest();
        return fBase_c::CANCELED;
    }

    mCc.mNonCollideMask = (mParam >> 12) & 3;
    mBc.mNonCollideMask = (mParam >> 12) & 3;
    mRc.mNonCollideMask = (mParam >> 12) & 3;

    mLayer = (mParam >> 8) & 3;
    mCc.mLayer = (mParam >> 8) & 3;
    mBc.mLayer = (mParam >> 8) & 3;

    mLiquidType = dBc_c::checkWater(mPos.x, mPos.y, mLayer, &mLiquidHeight);
    mDirection = (dActor_c::ActorDirection)((mParam >> 4) & 1);

    float v0 = 0.0f;
    if (checkInitLine(v0)) {
        mPos.y = v0 + 4.0f;
    }

    if (checkInitVanish()) {
        setDeleteEffect();

        dAudio::playMapSound(SE_OBJ_PNGN_ICEBALL_DISAPP, mPos, 0);
        deleteRequest();
        return fBase_c::CANCELED;
    }

    mCenterOffs.set(0.0f, 0.0f, 0.0f);
    mEatSpitType = 5;
    mActorProperties |= 0x80;
    mAreaNo = dCd_c::m_instance->getFileP(dScStage_c::m_instance->currCourse)->getAreaNo(&mPos);

    mCc.set(this, (sCcDatNewF *) &l_fball_cc_data);
    mCc.mLayer = mLayer;
    mCc.entry();

    mLightMask.init(&mAllocator, 3);

    float v2;
    mVec3_c v3(mPos.x, mPos.y + 4.0f, mPos.z);

    if (dBc_c::findGround(&v3, &v2, mLayer, 1)) {
        if (mPos.y > v2 - 6.0f) {
            mPos.y = v2 - 6.0f;
        }
    }

    mBc.set(this, &l_iceball_foot, &l_iceball_head, &l_iceball_wall);

    m_3d4 = 0;
    mStartPos = mPos;
    mStateMgr.changeState(StateID_FireMove);

    return fBase_c::SUCCEEDED;
}

int daIceBall_c::execute() {
    if (m_3d4 && !isState(StateID_Kill)) {
        mStateMgr.changeState(StateID_Kill);
    }
    mStateMgr.executeState();
    lightProc();

    return fBase_c::SUCCEEDED;
}

int daIceBall_c::draw() {
    mLightMask.draw();
    return fBase_c::SUCCEEDED;
}

void daIceBall_c::deleteReady() { }

int daIceBall_c::doDelete() {
    if (sm_IceBallCount[mPlayerNo] > 0) {
        sm_IceBallCount[mPlayerNo]--;
    }

    if (m_3d0 && (sm_IceBallAliveCount[mPlayerNo] > 0)) {
        sm_IceBallAliveCount[mPlayerNo]--;
    }

    return fBase_c::SUCCEEDED;
}

bool daIceBall_c::checkInitVanish() {
    if (mBc.checkWall(nullptr)) {
        return true;
    }

    dAcPy_c *player = daPyMng_c::getPlayer(mPlayerNo);
    mVec3_c a(player->mPos.x, mPos.y, mPos.z);
    mVec3_c b = mPos;
    float c = 0.0f;

    return dBc_c::findWall(&a, &b, &c, mLayer, 1, nullptr);
}

bool daIceBall_c::checkInitLine(float &a) {
    mVec3_c pos(mPos.x, mPos.y + 10.0f, mPos.z);
    float v0 = 0.0f;

    bool v1 = dBc_c::checkGround(&pos, &v0, mLayer, 1, -1);
    if (v1 && (v0 < pos.y) && (v0 >= mPos.y - 3.0f)) {
        a = v0;
        return true;
    }

    return false;
}

void daIceBall_c::lightProc() {
    mLightMask.set(mPos, lbl_802F5000[3]);
    mLightMask.execute();
}

void daIceBall_c::setEatTongue(dActor_c * actor) {
    this->removeCc();

    mStateMgr.changeState(StateID_EatIn);
}

bool daIceBall_c::bgCheck() {
    if (mRc.check(mBc.checkFoot(), 0, 0)) {
        mBc.mFlags |= dBc_c::SENSOR_8000;
    }

    mBc.checkWall(nullptr);
    mBc.checkHead(0);

    if (mBc.mFlags & 0x3c1fe03f) {
        return true;
    }

    return false;
}

void daIceBall_c::setDeleteEffect() {
    EffectManager_c::SetIceBallMissshitEffect(&mPos);
}

void daIceBall_c::ccCallback_Iceball(dCc_c * self, dCc_c * other) {
    if ((other->mCcData.mAttackCategoryInteract & (1 << dCc_c::ATTACK_ICE)) == 0) {
        return;
    }

    daIceBall_c *iceball = (daIceBall_c *)self->mpOwner;
    self->mFlag |= 2;
    dActor_c *thing = (dActor_c *)other->mpOwner;

    if (thing->mKind == fBase_c::ENTITY) {
        if (thing->mProfName == fProfile::EN_MARUTA) {
            if (iceball->mSpeed.y >= 0.0f) {
                return;
            }

            if (iceball->isState(StateID_FireMove)) {
                iceball->mStateMgr.changeState(daIceBall_c::StateID_Move);
                return;
            }
        }

        iceball->m_3d4 = 1;
    } else if (other->mCcData.mAttackCategory == dCc_c::ATTACK_KOOPA_FIRE) {
        if ((thing->mProfName == fProfile::KOOPA_FIRE) && ((int)(thing->mParam & 0xF) == 1)) {
            dAudio::SndObjctCmnMap_c* v0 = dAudio::g_pSndObjMap;
            nw4r::math::VEC2 y = dAudio::cvtSndObjctPos(iceball->mPos);
            v0->startSound(SE_OBJ_PNGN_ICEBALL_DISAPP, y, 0);
        }

        iceball->setDeleteEffect();
        iceball->m_3d4 = 1;
    } else if (other->mCcData.mCategory == dCc_c::CAT_ENTITY) {
        iceball->setDeleteEffect();
        iceball->m_3d4 = 1;
    }

    return;
}

void daIceBall_c::chgZpos() {
    if (mLayer == 0) {
        if (dBc_c::checkWireNet(mPos.x, mPos.y, mLayer)) {
            if (mStartPos.z >= 0.0f) {
                mPos.z = 2000.0f;
            } else {
                mPos.z = mStartPos.z;
            }
        } else {
            mPos.z = 2000.0f;
        }
    } else if (mLayer == 1) {
        mPos.z = -1800.0f;
    }
}

bool daIceBall_c::cullCheck() {
    return (int)dActor_c::otherCullCheck(mPos, l_cull_data, BoundingBox(64.0f, 32.0f), mAreaNo);
}

void daIceBall_c::eatMove(dActor_c * actor) {
    dActor_c::eatMove(actor);
    mPos.y += 2.0f;
}

bool daIceBall_c::checkDeleteBg() {
    u32 mask1 = 0x3f;
    u32 mask2 = dBc_c::SENSOR_4000000 | dBc_c::SENSOR_8000000 | dBc_c::SENSOR_10000000 | dBc_c::SENSOR_20000000;
    u32 x = (mBc.mFlags & mask1) | (mBc.mFlags & mask2);
    if (x) {
        return true;
    }
    if ((mBc.mFlags & dBc_c::SENSOR_8000) && (mRc.isRideFlag(0x200) & 0xFFFF)) {
        return true;
    }
    if ((u16)mBc.getFootAttr() == 3) {
        return true;
    }
    if ((s16)mBc.getSakaAngle(mDirection) >= 0x2D16) {
        return true;
    }
    if (mRc.getRide() && (mRc.getRide()->mFlags & 0x1000)) {
        return true;
    }
    return false;
}

bool daIceBall_c::waterlineCheck() {
    int prevLiquid = mLiquidType;
    mLiquidType = dBc_c::checkWater(mPos.x, mPos.y, mLayer, &mLiquidHeight);

    bool ret = false;
    switch (mLiquidType) {
        case 1: {
            if (prevLiquid == 0) {
                waterSplash(mLiquidHeight);
            }
            break;
        }
        case 2: {
            if (prevLiquid == 0) {
                waterSplash(mPos.y);
            }
            break;
        }
        case 3: {
            if (prevLiquid == 0) {
                yoganSplash(mLiquidHeight);
            }
            ret = true;
            break;
        }
        case 4: {
            if (prevLiquid == 0) {
                poisonSplash(mLiquidHeight);
            }
            ret = true;
            break;
        }
    }

    return ret;
}

void daIceBall_c::waterSplash(float height) {
    mVec3_c pos(mPos.x, height, 6500.0f);
    dAudio::SndObjctCmnMap_c *x = dAudio::g_pSndObjMap;
    x->startSound(SE_OBJ_FIREBALL_SPLASH, dAudio::cvtSndObjctPos(pos), 0);

    u32 v0;
    if (pos.y == mPos.y) {
        v0 = mLayer << 16 | 1;
    } else {
        v0 = mLayer << 16 | 2;
    }

    dEffActorMng_c * z = dEffActorMng_c::m_instance;

    z->createWaterSplashEff(pos, v0, -1, mVec3_c(1.0f, 1.0f, 1.0f));
    dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 8);
}

void daIceBall_c::yoganSplash(float height) {
    mVec3_c pos(mPos.x, height, 6500.0f);
    mVec3_c pos2;
    dAudio::SndObjctCmnMap_c *x = dAudio::g_pSndObjMap;
    x->startSound(SE_OBJ_CMN_SPLASH_LAVA, dAudio::cvtSndObjctPos(pos), 0);

    u32 v0 = mLayer << 16 | 4;

    dEffActorMng_c * z = dEffActorMng_c::m_instance;

    pos2.x = 0.6f;
    pos2.y = 0.6f;
    pos2.z = 0.6f;

    z->createWaterSplashEff(pos, v0, -1, pos2);
    dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 18);
}

void daIceBall_c::poisonSplash(float height) {
    mVec3_c pos(mPos.x, height, 6500.0f);
    mVec3_c pos2;
    dAudio::SndObjctCmnMap_c *x = dAudio::g_pSndObjMap;
    x->startSound(SE_OBJ_CMN_SPLASH_POISON, dAudio::cvtSndObjctPos(pos), 0);

    u32 v0 = mLayer << 16 | 6;

    dEffActorMng_c * z = dEffActorMng_c::m_instance;

    pos2.x = 0.6f;
    pos2.y = 0.6f;
    pos2.z = 0.6f;

    z->createWaterSplashEff(pos, v0, -1, pos2);
    dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 25);
}

// ----------------
// StateID_FireMove
// ----------------

void daIceBall_c::initializeState_FireMove() {
    static const float x_speeds[] = {3.6f, -3.6f};
    float x = x_speeds[mDirection];

    mAccelY = -0.4375f;
    mMaxFallSpeed = -4.0f;
    mSpeed.x = x;
    mSpeed.y = -3.0f;
    mSpeed.z = 0.0f;
}

void daIceBall_c::executeState_FireMove() {
    EffectManager_c::SetIceBallEffect(&mPos);

    if (m_3d0 != 0) {
        if (--m_3d0 == 0) {
            u32 v0 = mPlayerNo;
            if (0 < sm_IceBallAliveCount[v0]) {
                sm_IceBallAliveCount[v0]--;
            }
        }
    }

    calcFallSpeed();
    posMove();
    chgZpos();
    bgCheck();

    if (mBc.mFlags & 0x1fe000) {
        mStateMgr.changeState(StateID_Move);
        return;
    }

    if (waterlineCheck()) {
        setDeleteEffect();
        deleteRequest();
        return;
    }

    if (checkDeleteBg()) {
        dAudio::SndObjctCmnMap_c * v1 = dAudio::g_pSndObjMap;
        v1->startSound(SE_OBJ_PNGN_ICEBALL_DISAPP, dAudio::cvtSndObjctPos(mPos), 0);
        setDeleteEffect();
        deleteRequest();
    } else if (cullCheck()) {
        deleteRequest();
    }
}

void daIceBall_c::finalizeState_FireMove() { }

// ----------------
// StateID_Move
// ----------------

void daIceBall_c::initializeState_Move() {

    static const float x_speed_a[] = {3.0f, -3.0f};
    static const float x_speed_b[] = {1.5f, -1.5f};

    dAcPy_c * v0 = daPyMng_c::getPlayer(mPlayerNo);
    float v1 = v0->mSpeed.x;

    float v2 = lbl_802F5000[0];
    float v3 = lbl_802F5000[1];

    float resX;
    if (v1 > v2) {
        resX = (v3 * v2) + (v1 - v2) * lbl_802F5000[2];
    } else if (v1 < -v2) {
        resX = (-v3 * v2) + (v1 + v2) * lbl_802F5000[2];
    } else {
        resX = v3 * v1;
    }

    mSpeed.x = resX + x_speed_b[mDirection];
    mSpeed.y = 3.7f;
    mSpeed.z = 0.0f;
    mAccelY = -0.15f;
    mSpeedMax.x = x_speed_a[mDirection];
    mMaxFallSpeed = -3.0f;
    mAccelF = 0.04f;
}

void daIceBall_c::executeState_Move() {
    EffectManager_c::SetIceBallEffect(&mPos);

    if (m_3d0 != 0) {
        if (--m_3d0 == 0) {
            u32 v0 = mPlayerNo;
            if (0 < sm_IceBallAliveCount[v0]) {
                sm_IceBallAliveCount[v0]--;
            }
        }
    }

    calcSpeedX();
    calcFallSpeed();
    posMove();
    chgZpos();

    if (waterlineCheck()) {
        setDeleteEffect();
        deleteRequest();
        return;
    }

    if (bgCheck()) {
        dAudio::SndObjctCmnMap_c * v1 = dAudio::g_pSndObjMap;
        v1->startSound(SE_OBJ_PNGN_ICEBALL_DISAPP, dAudio::cvtSndObjctPos(mPos), 0);
        setDeleteEffect();
        deleteRequest();
        return;
    }

    if (mRc.getRide()) {
        dAudio::SndObjctCmnMap_c * v1 = dAudio::g_pSndObjMap;
        v1->startSound(SE_OBJ_PNGN_ICEBALL_DISAPP, dAudio::cvtSndObjctPos(mPos), 0);
        setDeleteEffect();
        deleteRequest();
        return;
    }

    if (cullCheck()) {
        deleteRequest();
    }
}

void daIceBall_c::finalizeState_Move() { }

// ----------------
// StateID_EatIn
// ----------------

void daIceBall_c::initializeState_EatIn() { }

void daIceBall_c::executeState_EatIn() {
    EffectManager_c::SetIceBallEffect(&mPos);
    if (mEatState == 2) {
        mStateMgr.changeState(StateID_EatNow);
    }
}

void daIceBall_c::finalizeState_EatIn() { }

// ----------------
// StateID_EatNow
// ----------------

void daIceBall_c::initializeState_EatNow() { }

void daIceBall_c::executeState_EatNow() { }

void daIceBall_c::finalizeState_EatNow() { }

// ----------------
// StateID_Kill
// ----------------

void daIceBall_c::initializeState_Kill() {
    removeCc();
    deleteRequest();
}

void daIceBall_c::executeState_Kill() { }

void daIceBall_c::finalizeState_Kill() { }

// ----------------

bool daIceBall_c::CheckIceballLimit(int playerId, int v0) {
    if (daIceBall_c::sm_IceBallCount[playerId] < 6) {
        if (v0 == 1) {
            return true;
        }

        if (daIceBall_c::sm_IceBallAliveCount[playerId] < 2) {
            return true;
        }
    }

    return false;
}

