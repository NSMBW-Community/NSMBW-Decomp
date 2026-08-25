#include <game/bases/d_a_bullet.hpp>
#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_eff_actor_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <constants/sound_list.h>

STATE_VIRTUAL_DEFINE(daBullet_c, EatIn);
STATE_VIRTUAL_DEFINE(daBullet_c, EatNow);
STATE_VIRTUAL_DEFINE(daBullet_c, SpiteMove);
STATE_VIRTUAL_DEFINE(daBullet_c, Reflect);
STATE_VIRTUAL_DEFINE(daBullet_c, HitReflect);
STATE_VIRTUAL_DEFINE(daBullet_c, HitStar);
STATE_VIRTUAL_DEFINE(daBullet_c, HitShell);
STATE_VIRTUAL_DEFINE(daBullet_c, HitYoshiBullet);

const float daBullet_c::smc_DEAD_FALL_GRAVITY = -0.1875f;
const float daBullet_c::smc_DEAD_FALL_YMAXSPEED = -4.5f;
const float daBullet_c::smc_DIR_PRM[2] = {
    1.0f,
    -1.0f
};

int daBullet_c::create() {
    allocate();
    mActorProperties |= 0xA0;
    initialize();

    return SUCCEEDED;
}

void daBullet_c::initialize() {}

void daBullet_c::allocate() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);
    createMdl();
    mAllocator.adjustFrmHeap();
}

void daBullet_c::createMdl() {}

int daBullet_c::preExecute() {
    if (dActor_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    if (mHitType != HIT_NONE) {
        removeCc();

        switch (mHitType) {
            case HIT_REFLECT:
                mStateMgr.changeState(StateID_HitReflect);
                break;

            case HIT_STAR:
                mStateMgr.changeState(StateID_HitStar);
                break;

            case HIT_SHELL:
                mStateMgr.changeState(StateID_HitShell);
                break;

            case HIT_YOSHI_BULLET:
                mStateMgr.changeState(StateID_HitYoshiBullet);
                break;
        }
    }

    return SUCCEEDED;
}

int daBullet_c::execute() {
    mStateMgr.executeState();

    if (!mHasSplashed) {
        mHasSplashed = splashProc();
    }

    cullingProc();
    return SUCCEEDED;
}

void daBullet_c::cullingProc() {
    ActorScrOutCheck(SKIP_NONE);
}

int daBullet_c::draw() {
    return SUCCEEDED;
}

void daBullet_c::deleteReady() {}

int daBullet_c::doDelete() {
    if (mAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        removeMdl();
    }

    return SUCCEEDED;
}

void daBullet_c::removeMdl() {}

void daBullet_c::collisionCallback(dCc_c *self, dCc_c *other) {
    dActor_c *actor = other->getOwner();
    daPlBase_c *player = (daPlBase_c *) actor;
    daBullet_c *bullet = (daBullet_c *) self->getOwner();
    STAGE_ACTOR_KIND_e kind = (STAGE_ACTOR_KIND_e) actor->mKind;

    if (kind == STAGE_ACTOR_PLAYER) {
        if (bullet->checkPlayerDamage(self, other)) {
            self->mInfo |= CC_NO_HIT;
        } else if (!player->isNoDamage()) {
            bullet->setDamage_Player(actor);
        }
    } else if (kind == STAGE_ACTOR_YOSHI) {
        u8 plrNo = actor->getPlrNo();
        if (plrNo >= PLAYER_COUNT) {
            return;
        }

        if (other->mCcData.mAttack == CC_ATTACK_YOSHI_EAT) {
            return;
        }

        if (bullet->checkYoshiDamage(self, other)) {
            self->mInfo |= CC_NO_HIT;
        } else if (!player->isNoDamage()) {
            bullet->setDamage_Player(actor);
        }
    } else {
        if (other->mCcData.mAttack == CC_ATTACK_SHELL && bullet->hitProc_Shell(other)) {
            self->mInfo |= CC_NO_HIT;
        } else if (other->mCcData.mAttack == CC_ATTACK_YOSHI_BULLET && bullet->hitProc_YoshiBullet(other)) {
            self->mInfo |= CC_NO_HIT;
        }
    }
}

void daBullet_c::revengeCallback(dCc_c *self, dCc_c *other) {
    if (!(other->mCcData.mVsDamage & (1 << CC_ATTACK_YOSHI_BULLET))) {
        return;
    }

    dActor_c *actor = other->getOwner();
    daBullet_c* bullet = (daBullet_c *) self->getOwner();

    bool canHit = true;

    if (actor->mKind == STAGE_ACTOR_PLAYER) {
        if (bullet->getPlrNo() == actor->getPlrNo()) {
            canHit = false;
        }
    } else if (actor->mKind == STAGE_ACTOR_YOSHI) {
        if (bullet->getPlrNo() == actor->getPlrNo()) {
            canHit = false;
        }
    } else if (actor->mActorProperties & 0x80) {
        canHit = false;
    }

    if (canHit) {
        self->mInfo |= CC_NO_HIT;
        bullet->hitProc_Reflect(other);
        bullet->revengeHitSE();
    }
}

void daBullet_c::revengeHitSE() {}

bool daBullet_c::checkPlayerDamage(dCc_c *self, dCc_c *other) {
    if (other->mCcData.mAttack == CC_ATTACK_STAR) {
        if (hitProc_Star(other)) {
            return true;
        }
    }

    return false;
}

bool daBullet_c::checkYoshiDamage(dCc_c *self, dCc_c *other) {
    if (other->mCcData.mAttack == CC_ATTACK_STAR) {
        if (hitProc_Star(other)) {
            return true;
        }
    }

    return false;
}

void daBullet_c::setDeadMove(const mVec3_c &speed, short angle) {
    mSpeed = speed;

    mSpeedMax.set(speed.x, smc_DEAD_FALL_YMAXSPEED, 0.0f);

    mAccelY = smc_DEAD_FALL_GRAVITY;

    if (mDirection == mDeadMoveDirection) {
        mDeadRollDelta.set(angle, 0, 0);
    } else {
        mDeadRollDelta.set(-angle, 0, 0);
    }
}

bool daBullet_c::hitProc_Star(dCc_c *other) {
    dActor_c *owner = other->getOwner();

    if (mPos.x >= owner->mPos.x) {
        mDeadMoveDirection = DIR_LR_R;
    } else {
        mDeadMoveDirection = DIR_LR_L;
    }

    float mag = 1.75f + 0.35f * std::fabs(owner->mSpeed.x);
    mVec3_c speed(mag * smc_DIR_PRM[mDeadMoveDirection], 3.75f, 0.0f);

    setDeadMove(speed, 0x1000);

    mVec3_c effectPos(mCc.getCollPosX(), mCc.getCollPosY(), 5500.0f);
    mEf::createEffect("Wm_en_hit", 0, &effectPos, nullptr, nullptr);

    dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN_NO_SCORE, mPos, 0);

    mHitType = HIT_STAR;

    return true;
}

bool daBullet_c::hitProc_Shell(dCc_c *other) {
    dActor_c *owner = other->getOwner();

    if (owner->mSpeed.x >= 0.0f) {
        mDeadMoveDirection = DIR_LR_R;
    } else {
        mDeadMoveDirection = DIR_LR_L;
    }

    float mag = 1.75f + 0.35f * std::fabs(owner->mSpeed.x);
    mVec3_c speed(mag * smc_DIR_PRM[mDeadMoveDirection], 3.75f, 0.0f);

    setDeadMove(speed, 0x1000);

    mVec3_c effectPos(mCc.getCollPosX(), mCc.getCollPosY(), 5500.0f);
    mEf::createEffect("Wm_en_hit", 0, &effectPos, nullptr, nullptr);

    dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN_NO_SCORE, mPos, 0);

    mHitType = HIT_SHELL;

    return true;
}

bool daBullet_c::hitProc_YoshiBullet(dCc_c *other) {
    dActor_c *owner = other->getOwner();

    if (owner->mSpeed.x >= 0.0f) {
        mDeadMoveDirection = DIR_LR_R;
    } else {
        mDeadMoveDirection = DIR_LR_L;
    }

    float mag = 1.75f + 0.35f * std::fabs(owner->mSpeed.x);
    mVec3_c speed(mag * smc_DIR_PRM[mDeadMoveDirection], 3.75f, 0.0f);

    setDeadMove(speed, 0xC00);

    mVec3_c effectPos(mCc.getCollPosX(), mCc.getCollPosY(), 5500.0f);
    mEf::createEffect("Wm_en_hit", 0, &effectPos, nullptr, nullptr);

    dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN_NO_SCORE, mPos, 0);

    mHitType = HIT_YOSHI_BULLET;

    return true;
}

bool daBullet_c::hitProc_Reflect(dCc_c *other) {
    mDeadMoveDirection = mDirection ^ 1;

    mVec3_c speed(1.75f * smc_DIR_PRM[mDeadMoveDirection], 2.75f, 0.0f);

    setDeadMove(speed, 0x600);
    mHitType = HIT_REFLECT;

    return true;
}

void daBullet_c::setEatTongue(dActor_c *eatingActor) {
    removeCc();
    mStateMgr.changeState(StateID_EatIn);
}

bool daBullet_c::setEatSpitOut(dActor_c *eatingActor) {
    mDirection = eatingActor->mDirection;
    mPlayerNo = eatingActor->getPlrNo();
    setSpitOutMove(eatingActor);

    mCc.mCcData.mKind = CC_KIND_TAMA;
    mCc.mCcData.mAttack = CC_ATTACK_YOSHI_BULLET;
    mCc.mCcData.mVsKind =
        BIT_FLAG(CC_KIND_PLAYER) |
        BIT_FLAG(CC_KIND_PLAYER_ATTACK) |
        BIT_FLAG(CC_KIND_YOSHI) |
        BIT_FLAG(CC_KIND_ENEMY);
    mCc.mCcData.mVsDamage = 0;
    mCc.mCcData.mCallback = revengeCallback;

    reviveCc();

    mPos.z = 5750.0f;
    mStateMgr.changeState(StateID_SpiteMove);

    return true;
}

void daBullet_c::setSpitOutMove(dActor_c *eatingActor) {}

bool daBullet_c::splashProc() {
    bool result = false;

    float height = 0.0f;
    switch (dBc_c::checkWater(mPos.x, mPos.y + 4.0f, mLayer, &height)) {
        case dBc_c::WATER_CHECK_WATER:
        case dBc_c::WATER_CHECK_WATER_BUBBLE:
            waterSplashEffect(mVec3_c(mPos.x, height, 6500.0f), 1.0f);
            result = true;
            break;
        default:
            break;
    }

    return result;
}

void daBullet_c::waterSplashEffect(const mVec3_c &pos, float scale) {
    u32 splashFlags = (mLayer << 16) | 1;
    mVec3_c splashPos = pos;
    mVec3_c splashScale(scale, scale, scale);

    dEffActorMng_c::m_instance->createWaterSplashEff(splashPos, splashFlags, -1, splashScale);

    dAudio::g_pSndObjMap->startSound(SE_OBJ_CMN_SPLASH, pos, 0);

    dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 6);
}

void daBullet_c::setDamage_Player(dActor_c *actor) {
    daPlBase_c *player = (daPlBase_c *) actor;
    player->setDamage(this, daPlBase_c::DAMAGE_DEFAULT);
}

void daBullet_c::initializeState_EatIn() {}

void daBullet_c::finalizeState_EatIn() {}

void daBullet_c::executeState_EatIn() {
    if (mEatState == EAT_STATE_EATEN) {
        mStateMgr.changeState(StateID_EatNow);
    }
}

void daBullet_c::initializeState_EatNow() {}

void daBullet_c::finalizeState_EatNow() {}

void daBullet_c::executeState_EatNow() {}

void daBullet_c::initializeState_SpiteMove() {}

void daBullet_c::finalizeState_SpiteMove() {}

void daBullet_c::executeState_SpiteMove() {
    calcSpeedX();
    calcSpeedY();
    posMove();
    spitRoll();
    moveSE();
}

void daBullet_c::spitRoll() {}

void daBullet_c::moveSE() {}

void daBullet_c::initializeState_Reflect() {}

void daBullet_c::finalizeState_Reflect() {}

void daBullet_c::executeState_Reflect() {
    calcSpeedY();
    posMove();
}

void daBullet_c::initializeState_HitReflect() {}

void daBullet_c::finalizeState_HitReflect() {}

void daBullet_c::executeState_HitReflect() {
    calcSpeedY();
    posMove();
    deadRoll();
}

void daBullet_c::deadRoll() {
    mAngle.x += (u16) mDeadRollDelta.x;
    mAngle.y += (u16) mDeadRollDelta.y;
    mAngle.z += (u16) mDeadRollDelta.z;
}

void daBullet_c::initializeState_HitStar() {}

void daBullet_c::finalizeState_HitStar() {}

void daBullet_c::executeState_HitStar() {
    calcSpeedY();
    posMove();
    deadRoll();
}

void daBullet_c::initializeState_HitShell() {}

void daBullet_c::finalizeState_HitShell() {}

void daBullet_c::executeState_HitShell() {
    calcSpeedY();
    posMove();
    deadRoll();
}

void daBullet_c::initializeState_HitYoshiBullet() {}

void daBullet_c::finalizeState_HitYoshiBullet() {}

void daBullet_c::executeState_HitYoshiBullet() {
    calcSpeedY();
    posMove();
    deadRoll();
}
