#include <game/bases/d_a_iceball.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_eff_actor_manager.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_cd.hpp>
#include <game/bases/d_bg.hpp>
#include <game/framework/f_profile.hpp>
#include <game/framework/f_profile_name.hpp>
#include <game/sLib/s_GlobalData.hpp>
#include <constants/sound_list.h>


ACTOR_PROFILE(ICEBALL, daIceBall_c, 2);

int daIceBall_c::sm_IceBallCount[4];
int daIceBall_c::sm_IceBallAliveCount[4];

template <>
const daIceBall_c::GlobalData_t sGlobalData_c<daIceBall_c>::mData = {
    0.75f,
    1.05f,
    0.6f,
    120.0f
};

const sBcSensorPoint l_iceball_foot = { SENSOR_IS_POINT, 0, -0x3000 };
const sBcSensorPoint l_iceball_head = { SENSOR_IS_POINT, 0, 0x3000 };
const sBcSensorPoint l_iceball_wall = { SENSOR_IS_POINT, 0x3000, 0 };

const sCcDatNewF l_fball_cc_data = {
    0.0f, 0.0f,
    6.0f, 6.0f,
    CC_KIND_TAMA, CC_ATTACK_ICEBALL,
    BIT_FLAG(CC_KIND_PLAYER_ATTACK) | BIT_FLAG(CC_KIND_ENEMY) |
    BIT_FLAG(CC_KIND_BALLOON) | BIT_FLAG(CC_KIND_ITEM) | BIT_FLAG(CC_KIND_TAMA) | BIT_FLAG(CC_KIND_KILLER),
    BIT_FLAG(CC_ATTACK_ICE_BREAK) | BIT_FLAG(CC_ATTACK_KOOPA_FIRE) | BIT_FLAG(CC_ATTACK_YOSHI_EAT),
    CC_STATUS_NONE,
    daIceBall_c::ccCallback_Iceball
};

const float l_cull_data[4] = { 0.0f, 0.0f, 8.0f, 8.0f };

STATE_DEFINE(daIceBall_c, FireMove);
STATE_DEFINE(daIceBall_c, Move);
STATE_DEFINE(daIceBall_c, EatIn);
STATE_DEFINE(daIceBall_c, EatNow);
STATE_DEFINE(daIceBall_c, Kill);

int daIceBall_c::create() {
    mPlayerNo = ACTOR_PARAM(PlayerNo);

    bool isCreateOK = CheckIceballLimit(mPlayerNo, ACTOR_PARAM(LimitMode));

    mAliveTimer = 60;

    sm_IceBallCount[mPlayerNo]++;
    sm_IceBallAliveCount[mPlayerNo]++;

    if (!isCreateOK) {
        deleteRequest();
        return fBase_c::CANCELED;
    }

    mCc.mAmiLine = ACTOR_PARAM(AmiLine);
    mBc.mAmiLine = ACTOR_PARAM(AmiLine);
    mRc.mLineKind = ACTOR_PARAM(AmiLine);

    mLayer = ACTOR_PARAM(Layer);
    mCc.mLayer = ACTOR_PARAM(Layer);
    mBc.mLayer = ACTOR_PARAM(Layer);

    mLiquidType = dBc_c::checkWater(mPos.x, mPos.y, mLayer, &mLiquidHeight);
    mDirection = ACTOR_PARAM(Direction);

    float groundHeight = 0.0f;
    if (checkInitLine(groundHeight)) {
        mPos.y = groundHeight + 4.0f;
    }

    if (checkInitVanish()) {
        setDeleteEffect();

        dAudio::SoundEffectID_t(SE_OBJ_PNGN_ICEBALL_DISAPP).playMapSound(mPos, 0);
        deleteRequest();
        return fBase_c::CANCELED;
    }

    mCenterOffs.set(0.0f, 0.0f, 0.0f);
    mEatBehaviour = EAT_TYPE_ICEBALL;
    mActorProperties |= 0x80;
    mAreaNo = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile)->getAreaNo(&mPos);

    mCc.set(this, (sCcDatNewF *) &l_fball_cc_data);
    mCc.mLayer = mLayer;
    mCc.entry();

    mLightMask.init(&mAllocator, 3);

    float ceilingHeight;
    mVec3_c ceilCheckPos(mPos.x, mPos.y + 4.0f, mPos.z);

    if (dBc_c::checkTenjou(&ceilCheckPos, &ceilingHeight, mLayer, 1)) {
        if (mPos.y > ceilingHeight - 6.0f) {
            mPos.y = ceilingHeight - 6.0f;
        }
    }

    mBc.set(this, l_iceball_foot, l_iceball_head, l_iceball_wall);

    mHitEntity = false;
    mStartPos = mPos;
    mStateMgr.changeState(StateID_FireMove);

    return fBase_c::SUCCEEDED;
}

int daIceBall_c::execute() {
    if (mHitEntity && !isState(StateID_Kill)) {
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

void daIceBall_c::deleteReady() {}

int daIceBall_c::doDelete() {
    if (sm_IceBallCount[mPlayerNo] > 0) {
        sm_IceBallCount[mPlayerNo]--;
    }

    if (mAliveTimer != 0 && sm_IceBallAliveCount[mPlayerNo] > 0) {
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

    return dBc_c::checkWall(&a, &b, &c, mLayer, 1, nullptr);
}

bool daIceBall_c::checkInitLine(float &groundHeight) {
    mVec3_c pos(mPos.x, mPos.y + 10.0f, mPos.z);

    float height = 0.0f;
    if (dBc_c::checkGround(&pos, &height, mLayer, 1, -1)) {
        if (height < pos.y && height >= mPos.y - 3.0f) {
            groundHeight = height;
            return true;
        }
    }

    return false;
}

void daIceBall_c::lightProc() {
    mLightMask.set(mPos.x, mPos.y, mPos.z, sGlobalData_c<daIceBall_c>::mData.mLightRadius);
    mLightMask.execute();
}

void daIceBall_c::setEatTongue(dActor_c *actor) {
    this->removeCc();

    mStateMgr.changeState(StateID_EatIn);
}

bool daIceBall_c::bgCheck() {
    if (mRc.check(mBc.checkFoot(), 0, 0)) {
        mBc.mFlags |= dBc_c::FLAG_15;
    }

    mBc.checkWall(nullptr);
    mBc.checkHead(0);

    if (mBc.isCollision()) {
        return true;
    }

    return false;
}

void daIceBall_c::setDeleteEffect() {
    EffectManager_c::SetIceBallMissshitEffect(&mPos);
}

void daIceBall_c::ccCallback_Iceball(dCc_c *self, dCc_c *other) {
    if ((other->mCcData.mVsDamage & (1 << CC_ATTACK_ICEBALL)) == 0) {
        return;
    }

    self->mInfo |= CC_NO_HIT;

    daIceBall_c *thisIceball = (daIceBall_c *) self->getOwner();
    dActor_c *otherActor = other->getOwner();

    if (otherActor->mKind == fBase_c::ENEMY) {
        if (otherActor->mProfName == fProfile::EN_MARUTA) {
            if (thisIceball->mSpeed.y >= 0.0f) {
                return;
            }

            if (thisIceball->isState(StateID_FireMove)) {
                thisIceball->mStateMgr.changeState(StateID_Move);
                return;
            }
        }

        thisIceball->mHitEntity = true;
    } else if (other->mCcData.mAttack == CC_ATTACK_KOOPA_FIRE) {
        if ((otherActor->mProfName == fProfile::KOOPA_FIRE) && ((int)(otherActor->mParam & 0xF) == 1)) {
            dAudio::SndObjctCmnMap_c *map = dAudio::g_pSndObjMap;
            dAudio::SoundEffectID_t(SE_OBJ_PNGN_ICEBALL_DISAPP).playObjSound(map, thisIceball->mPos, 0);
        }

        thisIceball->setDeleteEffect();
        thisIceball->mHitEntity = true;
    } else if (other->mCcData.mKind == CC_KIND_ENEMY) {
        thisIceball->setDeleteEffect();
        thisIceball->mHitEntity = true;
    }
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
    return dActor_c::screenCullCheck(mPos, (const sRangeDataF &) l_cull_data, sRangeDataF(64.0f, 64.0f, 32.0f, 32.0f), mAreaNo);
}

void daIceBall_c::eatMove(dActor_c *actor) {
    dActor_c::eatMove(actor);
    mPos.y += 2.0f;
}

bool daIceBall_c::checkDeleteBg() {
    if (mBc.isWallL() | mBc.isWallR() | mBc.isHead()) {
        return true;
    }
    if ((mBc.mFlags & dBc_c::FLAG_15) && (mRc.isRideFlag(0x200) & 0xFFFF)) {
        return true;
    }
    if (mBc.getFootAttr() == 3) {
        return true;
    }
    if (mBc.getSakaAngle(mDirection) >= 0x2D16) {
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

    bool destroyIceball = false;
    switch (mLiquidType) {
        case dBc_c::WATER_CHECK_WATER:
            if (prevLiquid == dBc_c::WATER_CHECK_NONE) {
                waterSplash(mLiquidHeight);
            }
            break;
        case dBc_c::WATER_CHECK_WATER_BUBBLE:
            if (prevLiquid == dBc_c::WATER_CHECK_NONE) {
                waterSplash(mPos.y);
            }
            break;
        case dBc_c::WATER_CHECK_YOGAN:
            if (prevLiquid == dBc_c::WATER_CHECK_NONE) {
                yoganSplash(mLiquidHeight);
            }
            destroyIceball = true;
            break;
        case dBc_c::WATER_CHECK_POISON:
            if (prevLiquid == dBc_c::WATER_CHECK_NONE) {
                poisonSplash(mLiquidHeight);
            }
            destroyIceball = true;
            break;
        default:
            break;
    }

    return destroyIceball;
}

void daIceBall_c::waterSplash(float height) {
    mVec3_c pos(mPos.x, height, 6500.0f);
    dAudio::SoundEffectID_t(SE_OBJ_FIREBALL_SPLASH).playMapSound(pos, 0);

    u32 splashFlags;
    if (pos.y == mPos.y) {
        splashFlags = mLayer << 16 | 1;
    } else {
        splashFlags = mLayer << 16 | 2;
    }

    dEffActorMng_c::m_instance->createWaterSplashEff(pos, splashFlags, -1, mVec3_c(1.0f, 1.0f, 1.0f));
    dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 8);
}

void daIceBall_c::yoganSplash(float height) {
    mVec3_c pos(mPos.x, height, 6500.0f);
    dAudio::SoundEffectID_t(SE_OBJ_CMN_SPLASH_LAVA).playMapSound(pos, 0);

    u32 splashFlags = mLayer << 16 | 4;
    mVec3_c scale(0.6f, 0.6f, 0.6f);

    dEffActorMng_c::m_instance->createWaterSplashEff(pos, splashFlags, -1, scale);
    dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 18);
}

void daIceBall_c::poisonSplash(float height) {
    mVec3_c pos(mPos.x, height, 6500.0f);
    dAudio::SoundEffectID_t(SE_OBJ_CMN_SPLASH_POISON).playMapSound(pos, 0);

    u32 splashFlags = mLayer << 16 | 6;
    mVec3_c scale(0.6f, 0.6f, 0.6f);

    dEffActorMng_c::m_instance->createWaterSplashEff(pos, splashFlags, -1, scale);
    dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 25);
}

void daIceBall_c::initializeState_FireMove() {
    static const float x_speeds[] = {3.6f, -3.6f};

    mAccelY = -0.4375f;
    mMaxFallSpeed = -4.0f;
    mSpeed.set(x_speeds[mDirection], -3.0f, 0.0f);
}

void daIceBall_c::finalizeState_FireMove() {}

void daIceBall_c::executeState_FireMove() {
    EffectManager_c::SetIceBallEffect(&mPos);

    if (mAliveTimer != 0 && --mAliveTimer == 0 && sm_IceBallAliveCount[mPlayerNo] > 0) {
        sm_IceBallAliveCount[mPlayerNo]--;
    }

    calcFallSpeed();
    posMove();
    chgZpos();
    bgCheck();

    // Jump back up if we hit the ground
    if (mBc.mFlags & dBc_c::FLAG_FOOT) {
        mStateMgr.changeState(StateID_Move);
        return;
    }

    // Destroy if we hit lava or poison
    if (waterlineCheck()) {
        setDeleteEffect();
        deleteRequest();
        return;
    }

    if (checkDeleteBg()) {
        dAudio::SoundEffectID_t(SE_OBJ_PNGN_ICEBALL_DISAPP).playMapSound(mPos, 0);
        setDeleteEffect();
        deleteRequest();
    } else if (cullCheck()) {
        deleteRequest();
    }
}

void daIceBall_c::initializeState_Move() {
    static const float cs_speed_x[] = {3.0f, -3.0f};
    static const float cs_max_speed_x[] = {1.5f, -1.5f};

    float playerXSpeed = daPyMng_c::getPlayer(mPlayerNo)->mSpeed.x;
    float moveBoostThreshold = sGlobalData_c<daIceBall_c>::mData.moveBoostThreshold;
    float moveBoostScale = sGlobalData_c<daIceBall_c>::mData.mMoveBoostScale;

    float speedBoost;
    if (playerXSpeed > moveBoostThreshold) {
        float baseBoost = moveBoostScale * moveBoostThreshold;
        speedBoost = baseBoost + sGlobalData_c<daIceBall_c>::mData.calcExtraBoost(playerXSpeed - moveBoostThreshold);
    } else if (playerXSpeed < -moveBoostThreshold) {
        float baseBoost = -moveBoostScale * moveBoostThreshold;
        speedBoost = baseBoost + sGlobalData_c<daIceBall_c>::mData.calcExtraBoost(playerXSpeed + moveBoostThreshold);
    } else {
        speedBoost = moveBoostScale * playerXSpeed;
    }

    mSpeed.set(speedBoost + cs_speed_x[mDirection], 3.7f, 0.0f);
    mAccelY = -0.15f;
    mSpeedMax.x = cs_max_speed_x[mDirection];
    mMaxFallSpeed = -3.0f;
    mAccelF = 0.04f;
}

void daIceBall_c::finalizeState_Move() {}

void daIceBall_c::executeState_Move() {
    EffectManager_c::SetIceBallEffect(&mPos);

    if (mAliveTimer != 0 && --mAliveTimer == 0 && sm_IceBallAliveCount[mPlayerNo] > 0) {
        sm_IceBallAliveCount[mPlayerNo]--;
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
        dAudio::SoundEffectID_t(SE_OBJ_PNGN_ICEBALL_DISAPP).playMapSound(mPos, 0);
        setDeleteEffect();
        deleteRequest();
        return;
    }

    if (mRc.getRide()) {
        dAudio::SoundEffectID_t(SE_OBJ_PNGN_ICEBALL_DISAPP).playMapSound(mPos, 0);
        setDeleteEffect();
        deleteRequest();
        return;
    }

    if (cullCheck()) {
        deleteRequest();
    }
}

void daIceBall_c::initializeState_EatIn() {}

void daIceBall_c::finalizeState_EatIn() {}

void daIceBall_c::executeState_EatIn() {
    EffectManager_c::SetIceBallEffect(&mPos);
    if (mEatState == EAT_STATE_EATEN) {
        mStateMgr.changeState(StateID_EatNow);
    }
}

void daIceBall_c::initializeState_EatNow() {}

void daIceBall_c::finalizeState_EatNow() {}

void daIceBall_c::executeState_EatNow() {}

void daIceBall_c::initializeState_Kill() {
    removeCc();
    deleteRequest();
}

void daIceBall_c::finalizeState_Kill() {}

void daIceBall_c::executeState_Kill() {}

bool daIceBall_c::CheckIceballLimit(int playerId, int limitMode) {
    if (daIceBall_c::sm_IceBallCount[playerId] < 6) {
        if (limitMode == 1) {
            return true;
        }

        if (daIceBall_c::sm_IceBallAliveCount[playerId] < 2) {
            return true;
        }
    }

    return false;
}
