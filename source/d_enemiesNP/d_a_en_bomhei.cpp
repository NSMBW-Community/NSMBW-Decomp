#include <cmath>
#include <game/bases/d_a_en_bomhei.hpp>
#include <game/bases/d_a_fireball_base.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_yoshi.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_res_mng.hpp>
#include <constants/sound_list.h>

ACTOR_PROFILE(EN_BOMHEI, daEnBomhei_c, 0x12);

STATE_DEFINE(daEnBomhei_c, Walk);
STATE_DEFINE(daEnBomhei_c, Sleep);
STATE_VIRTUAL_DEFINE(daEnBomhei_c, Carry);
STATE_DEFINE(daEnBomhei_c, Slide);
STATE_DEFINE(daEnBomhei_c, Kick);
STATE_DEFINE(daEnBomhei_c, KickBom);
STATE_DEFINE(daEnBomhei_c, Wakidashi);
STATE_DEFINE(daEnBomhei_c, Explode);
STATE_DEFINE(daEnBomhei_c, Turn);
STATE_DEFINE(daEnBomhei_c, CannonHop_Upper);
STATE_DEFINE(daEnBomhei_c, CannonHop_Under);
STATE_DEFINE(daEnBomhei_c, AfterIce);
STATE_DEFINE(daEnBomhei_c, InIceLump);
STATE_VIRTUAL_DEFINE(daEnBomhei_c, EatOut);

const float daEnBomhei_c::smc_WalkSpeed = 0.5f;
const float daEnBomhei_c::smc_SlideSpeedX = 4.0f;
const float daEnBomhei_c::smc_KickSpeedX = 2.5f;
const float daEnBomhei_c::smc_KickSpeedY = 2.0f;
const int daEnBomhei_c::smc_ExplodeWaitFrames = 240;

const sBcSensorPoint l_bomhei_head = { 0, 0x0, 0x10000 };
const sBcSensorLine l_bomhei_foot = { 1, -0x3000, 0x3000, 0 };
const sBcSensorPoint l_bomhei_wall = { 0, 0x6000, 0x6000 };
const sBcSensorPoint l_bomhei_4 = { 0x40000, 0, 0x10000 };
const sBcSensorPoint l_bomhei_5 = { 0x40000, 0, 0 };
const sBcSensorPoint l_bomhei_6 = { 0x40000, 0x6000, 0x6000 };
const sBcSensorLine l_bomhei_7 = { 0x82000001, -0x6000, 0x6000, 0x10000 };
const u32 l_unused1[] = { 0, 0 };
const sBcSensorLine l_bomhei_8 = { 0x82000001, -0x6000, 0x6000, 0 };
const u32 l_unused2[] = { 0, 0 };
const sBcSensorLine l_bomhei_9 = { 0x82000001, 0xE000, 0x3000, 0x8000 };
const u32 l_unused3[] = { 0, 0 };

int unused() {
    return l_unused1[0] + l_unused2[0] + l_unused3[0];
}

const sCcDatNewF l_bomhei_cc = {
    {0.0f, 8.0f},
    {8.0f, 8.0f},
    CC_KIND_ENEMY,
    CC_ATTACK_NONE,
    BIT_FLAG(CC_KIND_PLAYER) | BIT_FLAG(CC_KIND_PLAYER_ATTACK) | BIT_FLAG(CC_KIND_YOSHI) |
    BIT_FLAG(CC_KIND_ENEMY) | BIT_FLAG(CC_KIND_ITEM) | BIT_FLAG(CC_KIND_TAMA),
    (u32) ~BIT_FLAG(CC_ATTACK_NONE) & ~BIT_FLAG(CC_ATTACK_YOSHI_MOUTH) & ~BIT_FLAG(CC_ATTACK_SAND_PILLAR),
    CC_STATUS_NONE,
    dEn_c::normal_collcheck,
};

const short l_turn_angleY_add[] = { 0x400, -0x400 };

int daEnBomhei_c::create() {
    mdlSetup();
    mScale.set(1.0f, 1.0f, 1.0f);
    mSpeedMax.set(0.0f, -4.0f, 0.0f);
    mVisibleAreaOffset.set(0.0f, 8.0f);
    mVisibleAreaSize.set(16.0f, 16.0f);
    mCenterOffs.set(0.0f, 8.0f, 0.0f);

    mBc.set(this, l_bomhei_foot, l_bomhei_head, l_bomhei_wall);
    mBc.m_4c = mPos.x;
    mCc.set(this, (sCcDatNewF *) &l_bomhei_cc);
    mCc.entry();

    mEatBehaviour = EAT_TYPE_EAT_PERMANENT;
    mFlags = EN_IS_HARD;

    if (ACTOR_PARAM(SpawnMode) != SPAWN_MODE_ICE_LUMP) {
        dEnemyMng_c::m_instance->mBomheiCount++;
    }
    initialStateSet();

    return SUCCEEDED;
}

void daEnBomhei_c::initialStateSet() {
    if (ACTOR_PARAM(SpawnMode) == SPAWN_MODE_WAKIDASHI) {
        changeState(StateID_Wakidashi);
    } else if (ACTOR_PARAM(SpawnMode) == SPAWN_MODE_ICE_LUMP) {
        changeState(StateID_InIceLump);
    } else if (ACTOR_PARAM(SpawnMode) == SPAWN_MODE_CANNON_HOP) {
        int spawnDir = ACTOR_PARAM(CannonHopSpawnDir);
        mCannonHopDir = spawnDir;
        mDirection = spawnDir & 1;
        mAngle.y = l_base_angleY[mDirection];
        if (spawnDir == 0 || spawnDir == 1) {
            // up-right or up-left
            changeState(StateID_CannonHop_Upper);
        } else {
            // down-right or down-left
            changeState(StateID_CannonHop_Under);
        }
    } else {
        mDirection = getPl_LRflag(mPos);
        mAngle.y = l_base_angleY[mDirection];
        changeState(StateID_Walk);
    }
}

void daEnBomhei_c::mdlSetup() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[0], nullptr, 0x20);

    mFile = dResMng_c::m_instance->getRes("bombhei", "g3d/bombhei.brres");
    nw4r::g3d::ResMdl resMdl = mFile.GetResMdl("bombhei");
    mModel.create(resMdl, &mAllocator, 0x128, 1, nullptr);

    setSoftLight_Enemy(mModel);
    nw4r::g3d::ResAnmChr anmChr = mFile.GetResAnmChr("walk");
    mAnm.create(resMdl, anmChr, &mAllocator, nullptr);

    nw4r::g3d::ResAnmClr anmClr = mFile.GetResAnmClr("bombhei");
    mAnmMat.create(resMdl, anmClr, &mAllocator, nullptr, 1);

    mModel.setAnm(mAnmMat);
    mAnmMat.setPlayMode(m3d::FORWARD_ONCE, 0);
    mAnmMat.setFrame(0.0f, 0);

    postMdlSetup();
    mAllocator.adjustFrmHeap();
}

void daEnBomhei_c::postMdlSetup() {}

int daEnBomhei_c::execute() {
    mStateMgr.executeState();
    if (!isState(StateID_InIceLump)) {
        if (!isState(StateID_Ice)) {
            if (mExplodeWaitTimer > 0) {
                mAnmMat.play();
                dAudio::SoundEffectID_t(SE_EMY_BH_HIBANA).holdEmySound(mUniqueID, mPos, 0);
            }
            if (mUnkTimer > 0) {
                mUnkTimer--;
            }
            if (HasamareBgCheck() || EnLavaCheck(mPos)) {
                changeState(StateID_Explode);
            }
        }
        ActorScrOutCheck(0);
    }
    return SUCCEEDED;
}

int daEnBomhei_c::preDraw() {
    if (dEn_c::preDraw() == NOT_READY) {
        return NOT_READY;
    }

    if (isState(StateID_Explode)) {
        return NOT_READY;
    }

    return SUCCEEDED;
}

int daEnBomhei_c::draw() {
    mModel.entry();
    return SUCCEEDED;
}

void daEnBomhei_c::modelUpdate() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    if (isState(StateID_Carry)) {
        pos = calcCarryPos(mLiftPos);
        mPos.x = pos.x;
        mPos.y = pos.y;
    }

    changePosAngle(&pos, &angle, 1);
    mMatrix.trans(pos.x, pos.y, pos.z);
    mMatrix.YrotM(angle.y);

    mMatrix.concat(mMtx_c::createTrans(0.0f, 8.0f, 0.0f));
    mMatrix.XrotM(angle.x);
    mMatrix.concat(mMtx_c::createTrans(0.0f, -8.0f, 0.0f));

    mMatrix.concat(mMtx_c::createTrans(0.0f, 16.0f, 0.0f));
    mMatrix.ZrotM(angle.z);
    mMatrix.concat(mMtx_c::createTrans(0.0f, -16.0f, 0.0f));

    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(mScale);
    mModel.calc(false);

    calcIgnitionPos();
}

void daEnBomhei_c::deleteReady() {}

int daEnBomhei_c::doDelete() {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && fManager_c::searchBaseByID(player->mCarryActorID) == this) {
            player->cancelCarry(this);
        }
    }

    if (ACTOR_PARAM(SpawnMode) != SPAWN_MODE_ICE_LUMP) {
        dEnemyMng_c::m_instance->mBomheiCount--;
        if (dEnemyMng_c::m_instance->mBomheiCount < 0) {
            dEnemyMng_c::m_instance->mBomheiCount = 0;
        }
    }

    return SUCCEEDED;
}

void daEnBomhei_c::finalUpdate() {
    modelUpdate();
}

bool daEnBomhei_c::ActorDrawCullCheck() {
    if (isState(StateID_Carry)) {
        return false;
    }
    return dActor_c::ActorDrawCullCheck();
}

void daEnBomhei_c::calcIgnitionPos() {
    nw4r::g3d::ResNode resNode = mModel.getResMdl().GetResNode("skl_root");
    mMtx_c mtx;
    mModel.getNodeWorldMtx(resNode.GetID(), &mtx);
    mtx.multVecZero(mIgnitionPos);
}

void daEnBomhei_c::updateCarryCc() {
    bool dir = mAngle.y < 0 ? 1 : 0;

    static const float sc_carryOffsetX[] = { -8.0f, 8.0f };

    mVec3_c centerPos(
        getCenterPos().x + sc_carryOffsetX[dir],
        getCenterPos().y,
        getCenterPos().z
    );

    mVec3_c v2(
        centerPos.x + l_EnMuki[dir] * 16.0f,
        centerPos.y,
        centerPos.z
    );

    mCc.mCcData.mOffset.set(mCcOffsetX, mCcOffsetY);
    mCc.mCcData.mSize.set(mCcWidth, mCcHeight);
    float f = 0.0f;
    if (dBc_c::checkWall(&centerPos, &v2, &f, mLayer, 1, nullptr)) {
        float f1 = (centerPos.x + f) * 0.5f;
        float f2 = (centerPos.x - f) * 0.5f;
        mCc.mCcData.mOffset.set(f1 - mPos.x, 8.0f);
        mCc.mCcData.mSize.set(std::fabs(f2), 8.0f);
    }
}

void daEnBomhei_c::setSpinLiftUpActor(dActor_c *carryingActor) {
    int plrNo = *carryingActor->getPlrNo();
    mPlayerNo = plrNo;
    mCarryingPlayer = plrNo;
    mLiftPos.set(0.0f, 0.0f, 0.0f);
    changeState(StateID_Carry);
}

void daEnBomhei_c::Normal_VsPlHitCheck(dCc_c *self, dCc_c *other) {
    dAcPy_c *player = (dAcPy_c *) other->mpOwner;
    if (fManager_c::searchBaseByID(player->mCarryActorID) == this) {
        return;
    }

    u8 newDir = self->mCollOffsetX[CC_KIND_PLAYER] >= 0.0f ? 0 : 1;
    u8 plrNo = *player->getPlrNo();
    if (isState(StateID_Sleep)) {
        if (carry_check(player)) {
            mCarryingPlayer = plrNo;
            mLiftPos.set(0.0f, -5.0f, 6.0f);
            changeState(StateID_Carry);
        } else {
            mDirection = newDir;
            mNoHitPlayer.mTimer[plrNo] = 32;
            dAudio::SoundEffectID_t(SE_EMY_KAME_KERU).playEmySound(getCenterPos(), dAudio::getRemotePlayer(plrNo));
            mPlayerNo = plrNo;
            mBc.mOwningPlayerNo = plrNo;
            changeState(StateID_Kick);
        }
    } else if (isState(StateID_Slide) || isState(StateID_Kick)) {
        mDirection = newDir;
        mNoHitPlayer.mTimer[plrNo] = 32;
        dAudio::SoundEffectID_t(SE_EMY_KAME_KERU).playEmySound(getCenterPos(), dAudio::getRemotePlayer(plrNo));
        mPlayerNo = plrNo;
        mBc.mOwningPlayerNo = plrNo;
        changeState(StateID_Kick);
    } else {
        int checkRes = Enfumi_check(self, other, 0);
        if (checkRes == 0) {
            if (!isState(StateID_Wakidashi) && !isState(StateID_Carry)) {
                dEn_c::Normal_VsPlHitCheck(self, other);
            }
        } else if (checkRes == 1) {
            mPlayerNo = plrNo;
            mBc.mOwningPlayerNo = plrNo;
            if (isState(StateID_Carry)) {
                changeState(StateID_Kick);
            } else {
                vf28c();
            }
        } else if (checkRes == 3) {
            mDirection = newDir;
            mPlayerNo = plrNo;
            mBc.mOwningPlayerNo = plrNo;
            if (mExplodeWaitTimer < 0) {
                mExplodeWaitTimer = smc_ExplodeWaitFrames;
                mAnmMat.setFrame(0.0f, 0);
            }
            vf29c();
            changeState(StateID_Slide);
        }
    }
}

void daEnBomhei_c::Normal_VsYoshiHitCheck(dCc_c *self, dCc_c *other) {
    daYoshi_c *yoshi = (daYoshi_c *) other->mpOwner;
    u8 newDir = self->mCollOffsetX[CC_KIND_YOSHI] >= 0.0f ? 0 : 1;
    u8 plrNo = *yoshi->getPlrNo();

    if (Enfumi_check(self, other, 0) == 0) {
        if (isState(StateID_Sleep) || isState(StateID_Slide) || isState(StateID_Kick)) {
            if (plrNo < PLAYER_COUNT) {
                mDirection = newDir;
                mPlayerNo = plrNo;
                mNoHitPlayer.mTimer[plrNo] = 32;
                mBc.mOwningPlayerNo = plrNo;
                dAudio::SoundEffectID_t(SE_EMY_KAME_KERU).playEmySound(getCenterPos(), dAudio::getRemotePlayer(plrNo));
                changeState(StateID_Kick);
            }
        } else {
            dEn_c::Normal_VsYoshiHitCheck(self, other);
        }
    } else if (!isState(StateID_Explode)) {
        setDeathInfo_YoshiFumi(yoshi);
    }
}

void daEnBomhei_c::Normal_VsEnHitCheck(dCc_c *self, dCc_c *other) {
    u16 flag = other->mCcData.mStatus;
    dActor_c *enemy = other->getOwner();

    if (flag & CC_STATUS_9) {
        self->mInfo |= CC_NO_HIT;
        if (!isState(StateID_Explode)) {
            changeState(StateID_Explode);
        }
    } else if (self->mCcData.mAttack == CC_ATTACK_SHELL && (flag & CC_STATUS_8) && hitCallback_Shell(self, other)) {
        other->mInfo |= CC_NO_HIT;
        self->mInfo |= CC_NO_HIT;
    } else if (
        isState(StateID_Carry) && (other->mCcData.mVsDamage & BIT_FLAG(CC_ATTACK_SHELL)) &&
        enemy->mProfName != fProfile::EN_HATENA_BALLOON && hitCallback_Shell(self, other)
    ) {
        other->mInfo |= CC_NO_HIT;
    } else if (isState(StateID_Walk)) {
        float offsetX = self->mCollOffsetX[CC_KIND_ENEMY];

        if ((mDirection == DIR_LR_L && offsetX > 0.0f) || (mDirection == DIR_LR_R && offsetX < 0.0f)) {
            changeState(StateID_Turn);
        }
    } else {
        dEn_c::Normal_VsEnHitCheck(self, other);
    }
}

void daEnBomhei_c::vf28c() {
    if (isState(StateID_Wakidashi)) {
        float f = l_EnMuki[mDirection ^ 1];
        mBc.checkWall(&f);
    }

    mSpeed.set(0.0f, 0.0f, 0.0f);
    vf29c();

    if (mExplodeWaitTimer < 0) {
        mExplodeWaitTimer = smc_ExplodeWaitFrames;
        mAnmMat.setFrame(0.0f, 0);
        changeState(StateID_Sleep);
    }
}

void daEnBomhei_c::block_hit_init() {
    mVec3_c efPos(mVec2_c(mPos.x, mPos.y), 5500.0f);

    hitdamageEffect(efPos);
    dAudio::SoundEffectID_t(SE_EMY_DOWN).playEmySound(mPos, 0);

    mDirection = mDeathFallDirection;
    mSpeed.set(l_base_fall_speed_x[mDirection], 3.0f, 0.0f);
    vf29c();

    if (mExplodeWaitTimer < 0) {
        mExplodeWaitTimer = smc_ExplodeWaitFrames;
        mAnmMat.setFrame(0.0f, 0);
        changeState(StateID_Sleep);
    }
}

bool daEnBomhei_c::hitCallback_Shell(dCc_c *self, dCc_c *other) {
    dActor_c *shell = other->getOwner();
    u8 dir = getTrgToSrcDir_Main(
        getCenterX(),
        shell->getCenterX()
    );
    int plrNo = acmShellPlayerNo(shell);
    shellDamageEffect(self, shell);
    int comboScore = -1;

    if ((u32) plrNo < PLAYER_COUNT) {
        int shortCombo = 0;
        if (mCombo.mType == 2) {
            shortCombo = 1;
        }
        shell->slideComboSE(shell->mComboMultiplier, shortCombo);
        shell->mComboMultiplier++;
        if (shell->mComboMultiplier >= 8) {
            shell->mComboMultiplier = 8;
        }
        comboScore = mCombo.getComboScore(shell->mComboMultiplier);
    } else {
        dAudio::SoundEffectID_t(SE_EMY_DOWN).playEmySound(mPos, 0);
    }

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        comboScore,
        -1,
        dir,
        (u8) plrNo
    };

    return true;
}

bool daEnBomhei_c::hitCallback_Fire(dCc_c *self, dCc_c *other) {
    dActor_c *fire = other->getOwner();
    u8 dir = !(fire->mSpeed.x >= 0.0f);
    mDirection = dir;
    mAngle.y = l_base_angleY[dir];
    mSpeed.x = l_EnMuki[dir] * smc_WalkSpeed;
    mSpeed.y = 1.5f;
    mSpeedMax.x = 0.0f;
    dAudio::SoundEffectID_t(SE_EMY_DOWN).playEmySound(mPos, 0);

    if (--mFireCoinCount >= 0) {
        dActorMng_c::m_instance->createUpCoin(getCenterPos(), dir, 1, 0);
    }
    vf29c();

    if (mExplodeWaitTimer < 0) {
        mExplodeWaitTimer = smc_ExplodeWaitFrames;
        mAnmMat.setFrame(0.0f, 0);
        changeState(StateID_Sleep);
    }

    return true;
}

bool daEnBomhei_c::hitCallback_Ice(dCc_c *self, dCc_c *other) {
    if (mIceMng.mActive) {
        return true;
    }

    dActor_c *player = (dActor_c *) other->getOwner();
    if (player->mSpeed.x >= 0.0f) {
        mIceDirection = DIR_LR_R;
    } else {
        mIceDirection = DIR_LR_L;
    }

    mAnmMat.setFrame(0.0f, 0);

    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *pl = daPyMng_c::getPlayer(i);
        if (pl != nullptr && fManager_c::searchBaseByID(pl->mCarryActorID) == this) {
            pl->cancelCarry(this);
        }
    }
    mIceMng.mPlrNo = *player->getPlrNo();
    changeState(StateID_Ice);

    mExplodeWaitTimer = -1;
    mPlayerNo = -1;

    return true;
}

void daEnBomhei_c::returnState_Ice() {
    mPlayerNo = -1;
    changeState(StateID_AfterIce);
}

bool daEnBomhei_c::hitCallback_HipAttk(dCc_c *self, dCc_c *other) {
    dActor_c *otherActor = other->getOwner();
    u8 plrNo = *otherActor->getPlrNo();

    if (plrNo >= PLAYER_COUNT) {
        return true;
    }
    if (mNoHitPlayer.mTimer[plrNo] != 0) {
        return true;
    }

    if (mPos.x >= otherActor->mPos.x) {
        mDirection = DIR_LR_R;
    } else {
        mDirection = DIR_LR_L;
    }

    mPlayerNo = plrNo;
    dAudio::SoundEffectID_t(SE_EMY_DOWN).playEmySound(mPos, 0);
    hipatkEffect(otherActor->mPos);
    mNoHitPlayer.mTimer[mPlayerNo] = 16;
    if (mExplodeWaitTimer < 0) {
        mExplodeWaitTimer = smc_ExplodeWaitFrames;
        mAnmMat.setFrame(0.0f, 0);
    }
    vf29c();
    changeState(StateID_Slide);

    return true;
}

bool daEnBomhei_c::hitCallback_YoshiHipAttk(dCc_c *self, dCc_c *other) {
    if (!isState(StateID_Explode)) {
        dAudio::SoundEffectID_t(SE_EMY_YOSHI_HPDP).playEmySound(mPos, 0);
        setDeathInfo_YoshiFumi(other->getOwner());
        return true;
    }

    return false;
}

bool daEnBomhei_c::createIceActor() {
    mVec3_c pos, size;
    if (isState(StateID_Walk) || isState(StateID_Turn)) {
        pos.set(mPos.x, mPos.y - 2.0f, mPos.z);
        size.set(1.25f, 1.25f, 1.25f);
    } else {
        pos.set(mPos.x, mPos.y - 2.0f, 2.0f + mPos.z);
        size.set(1.25f, 1.25f, 1.5f);
    }

    dIceInfo iceInfo[] = {
        {
            0,
            pos,
            size
        }
    };
    return mIceMng.createIce(iceInfo, ARRAY_SIZE(iceInfo));
}

bool daEnBomhei_c::EtcDamageCheck(dCc_c *self, dCc_c *other) {
    if (dEn_c::EtcDamageCheck(self, other)) {
        return true;
    }

    dActor_c *otherActor = other->getOwner();
    daEnBomhei_c *selfActor = (daEnBomhei_c *) self->getOwner();
    if (other->mCcData.mStatus & CC_STATUS_9) {
        self->mInfo |= CC_NO_HIT;
        if (!selfActor->isState(StateID_Explode)) {
            selfActor->changeState(StateID_Explode);
        }
        return true;
    } else if (otherActor->mProfName == fProfile::PAKKUN_FIREBALL || otherActor->mProfName == fProfile::BROS_FIREBALL) {
        if (selfActor->mExplodeWaitTimer < 0) {
            selfActor->mExplodeWaitTimer = smc_ExplodeWaitFrames;
            selfActor->mAnmMat.setFrame(0.0f, 0);
            selfActor->changeState(StateID_Sleep);
        }
        daFireBall_Base_c *fireball = (daFireBall_Base_c *) otherActor;
        fireball->kill();
        return true;
    }

    return false;
}

void daEnBomhei_c::bcSet1() {
    mBc.set(this, l_bomhei_5, l_bomhei_4, l_bomhei_6);
    mBc.mOwningPlayerNo = mPlayerNo;
}

void daEnBomhei_c::bcSet2() {
    mBc.set(this, l_bomhei_foot, l_bomhei_head, l_bomhei_wall);
}

void daEnBomhei_c::setDeathInfo_Carry(dActor_c *killedBy) {
    mDeathInfo = (sDeathInfoData) {
        0.0f,
        3.0f,
        -4.0f,
        -0.1875f,
        &StateID_DieFall,
        -1,
        -1,
        mDirection,
        (u8) *killedBy->getPlrNo()
    };
}

void daEnBomhei_c::checkBombBreak() {
    mVec2_c v1, v2;

    static const float offsets[2][2] = {
        { 40.0f, -24.0f },
        { 24.0f, -40.0f }
    };

    v1.x = mPos.x + offsets[mDirection][1];
    v1.y = mPos.y + 15.0f;
    v2.x = mPos.x + offsets[mDirection][0];
    v2.y = mPos.y + 1.0f;
    mBc.checkBombBreak(v1, v2);

    v1.x = mPos.x - 8.0f;
    v1.y = mPos.y + 31.0f;
    v2.x = mPos.x + 8.0f;
    v2.y = mPos.y - 15.0f;
    mBc.checkBombBreak(v1, v2);
}

void daEnBomhei_c::explodeBgUnit() {
    mVec2_c pos = getExplodeTilePos();
    for (int i = 0; i < 2; i++) {
        int unitType = dBc_c::getUnitType(pos.x, pos.y, mLayer);
        if (unitType & 0x1c) {
            short x, y;
            y = -pos.y;
            x = pos.x;
            dBg_c::m_bg_p->BgUnitChange(x, y, 0, 0);
        }
        pos.x += l_EnMuki[mDirection] * 16.0f;
    }
}

mVec2_c daEnBomhei_c::getExplodeTilePos() {
    if (mExplodeTimer >= 9) {
        return mVec2_c(0.0f, 0.0f);
    }

    static const float xOffsets[] = {
        -16.0f, 0.0f, 16.0f,
        -16.0f, 0.0f, 16.0f,
        -16.0f, 0.0f, 16.0f
    };
    static const float yOffsets[] = {
        16.0f, 16.0f, 16.0f,
        0.0f, 0.0f, 0.0f,
        -16.0f, -16.0f, -16.0f
    };

    int tileX = mPos.x / 16;
    int tileY = mPos.y / 16;
    float x = tileX * 16.0f;
    float y = tileY * 16.0f;
    x += xOffsets[mExplodeTimer];
    y += yOffsets[mExplodeTimer];
    return mVec2_c(x, y);
}

void explosionCallback(dCc_c *self, dCc_c *other) {
    dActor_c *selfActor = self->getOwner();
    dActor_c *otherActor = other->getOwner();
    if (otherActor->mKind == dActor_c::STAGE_ACTOR_PLAYER || otherActor->mKind == dActor_c::STAGE_ACTOR_YOSHI) {
        daPlBase_c *player = (daPlBase_c *) otherActor;
        player->setDamage(selfActor, daPlBase_c::DAMAGE_NONE);
    }
}

void daEnBomhei_c::initializeState_Walk() {
    if (
        *mStateMgr.getOldStateID() != StateID_AfterIce &&
        *mStateMgr.getOldStateID() != StateID_CannonHop_Upper &&
        *mStateMgr.getOldStateID() != StateID_CannonHop_Under
    ) {
        nw4r::g3d::ResAnmChr anm = mFile.GetResAnmChr("walk");
        mAnm.setAnm(mModel, anm, m3d::FORWARD_LOOP);
        mModel.setAnm(mAnm, 2.0f);
        mAnm.setRate(1.0f);
    }

    static const float dirSpeed[] = { smc_WalkSpeed, -smc_WalkSpeed };

    mFlags &= ~0x10000;
    mSpeed.x = dirSpeed[mDirection];
    mPos.z = 1500.0f + ACTOR_PARAM(ZLayer) * 16.0f;
    mPlayerNo = -1;
    mBc.mOwningPlayerNo = -1;
}

void daEnBomhei_c::finalizeState_Walk() {}

void daEnBomhei_c::executeState_Walk() {
    mModel.play();
    calcSpeedY();
    posMove();

    EnBgCheckFoot();
    if (mBc.isFoot()) {
        mSpeed.y = 0.0f;
    }

    mBc.checkHead(mBc.mFlags);

    EnBgCheckWall();

    WaterCheck(mPos, 1.0f);
    if (mBc.mFlags & 0x15 << mDirection) {
        changeState(StateID_Turn);
    }
}

void daEnBomhei_c::initializeState_Turn() {
    mSpeed.x = 0.0f;
    mDirection ^= 1;
}

void daEnBomhei_c::finalizeState_Turn() {}

void daEnBomhei_c::executeState_Turn() {
    mModel.play();
    calcSpeedY();
    posMove();

    EnBgCheckFoot();
    if (mBc.isFoot()) {
        mAnm.setRate(1.0f);
        mSpeed.y = 0.0f;
    }

    mBc.checkHead(mBc.mFlags);

    EnBgCheckWall();

    WaterCheck(mPos, 1.0f);

    mAngle.y += l_turn_angleY_add[mDirection];
    int angleY = l_base_angleY[mDirection];

    int abs1 = abs(angleY);
    int abs2 = abs(mAngle.y);
    if (abs2 >= abs1) {
        mAngle.y = angleY;
        changeState(StateID_Walk);
    }
}

void daEnBomhei_c::initializeState_Sleep() {
    nw4r::g3d::ResAnmChr anm = mFile.GetResAnmChr("stop");
    mAnm.setAnm(mModel, anm, m3d::FORWARD_LOOP);
    mModel.setAnm(mAnm, 2.0f);
    mAnm.setRate(1.0f);
    mFlags &= ~0x10000;
    mAngle.y = l_base_angleY[mDirection];
}

void daEnBomhei_c::finalizeState_Sleep() {}

void daEnBomhei_c::executeState_Sleep() {
    mModel.play();
    calcSpeedY();
    posMove();

    EnBgCheckFoot();
    if (mBc.isFoot()) {
        Bound(0.1875f, 0.5f, 0.5f);
    }

    mBc.checkHead(0);

    EnBgCheckWall();
    if (mBc.mFlags & 0x3c000000) {
        if (mSpeed.y > 0.0f) {
            mSpeed.y = -0.85f * mSpeed.y;
        }
    }
    if (mBc.mFlags & 0x3f) {
        mSpeed.x = 0.0f;
    }

    mEffect.createEffect("Wm_en_bombignition", 0, &mIgnitionPos, nullptr, nullptr);
    mExplodeWaitTimer--;
    if (mExplodeWaitTimer <= 0) {
        changeState(StateID_Explode);
    }
}

void daEnBomhei_c::initializeState_Carry() {
    nw4r::g3d::ResAnmChr anm = mFile.GetResAnmChr("carry");
    mAnm.setAnm(mModel, anm, m3d::FORWARD_LOOP);
    mModel.setAnm(mAnm, 2.0f);
    mAnm.setRate(1.0f);

    mPlayerNo = mCarryingPlayer;
    dAcPy_c *player = daPyMng_c::getPlayer(mCarryingPlayer);
    if (player->mAmiLayer == 1) {
        mAmiLayer = 0;
    } else {
        mAmiLayer = 1;
    }

    float x = mCc.mCcData.mOffset.x;
    float y = mCc.mCcData.mOffset.y;
    float width = mCc.mCcData.mSize.x;
    float height = mCc.mCcData.mSize.y;
    mCcOffsetX = x;
    mCcOffsetY = y;
    mCcWidth = width;
    mCcHeight = height;
    mCc.mAmiLine = l_Ami_Line[mAmiLayer];
    mBc.mAmiLine = l_Ami_Line[mAmiLayer];
    mCc.mCcData.mVsKind |= BIT_FLAG(CC_KIND_KILLER) | BIT_FLAG(CC_KIND_BALLOON);
    mCc.mCcData.mAttack = CC_ATTACK_SHELL;
    mRc.setRide(nullptr);
    mActorProperties &= ~0x2;
}

void daEnBomhei_c::finalizeState_Carry() {
    dAcPy_c *player = daPyMng_c::getPlayer(mCarryingPlayer);
    player->cancelCarry(this);
    mCc.mCcData.mVsKind &= ~BIT_FLAG(CC_KIND_KILLER);
    mCc.mCcData.mAttack = CC_ATTACK_NONE;
    mCc.mCcData.mOffset.set(mCcOffsetX, mCcOffsetY);
    mCc.mCcData.mSize.set(mCcWidth, mCcHeight);
    mRc.setRide(nullptr);
    mBc.mFlags = 0;
    mCarryingFlags &= ~(CARRY_RELEASE | CARRY_THROW);
    mActorProperties |= 0x2;
    mAngle.y = l_base_angleY[mDirection];
}

void daEnBomhei_c::executeState_Carry() {
    dAcPy_c *player = daPyMng_c::getPlayer(mCarryingPlayer);
    mModel.play();
    mEffect.createEffect("Wm_en_bombignition", 0, &mIgnitionPos, nullptr, nullptr);

    mExplodeWaitTimer--;
    if (mExplodeWaitTimer <= 0) {
        mDirection = player->mDirection;
        if (checkWallAndBg()) {
            setDeathInfo_Carry(player);
        } else {
            changeState(StateID_Explode);
        }
    } else if (mCarryingFlags & CARRY_RELEASE) {
        mDirection = mThrowDirection;
        mAngle.y = l_base_angleY[mDirection];
        if (checkWallAndBg()) {
            setDeathInfo_Carry(player);
            return;
        }
        if ((mBc.mFlags & 0x15 << mDirection) == 0) {
            mPos.x += l_EnMuki[mDirection] * 6.0f;
        }
        if (mCarryingFlags & CARRY_THROW) {
            changeState(StateID_Slide);
        } else {
            changeState(StateID_Sleep);
        }
    } else {
        mAngle.y = player->mAngle.y;
        WaterCheck(mPos, 1.0f);
    }
    updateCarryCc();
}

void daEnBomhei_c::initializeState_Slide() {
    nw4r::g3d::ResAnmChr anm = mFile.GetResAnmChr("stop");
    mAnm.setAnm(mModel, anm, m3d::FORWARD_LOOP);
    mModel.setAnm(mAnm, 2.0f);
    mAnm.setRate(1.0f);

    static const float dirSpeed[] = { smc_SlideSpeedX, -smc_SlideSpeedX };
    clrComboCnt();
    mAccelF = 0.0625f;
    mSpeed.x = dirSpeed[mDirection];
    mSpeedMax.x = 0.0f;
    mAngle.y = l_base_angleY[mDirection];
    mNoHitPlayer.mTimer[mPlayerNo] = 10;
    mCc.mCcData.mAttack = CC_ATTACK_SHELL;
    bcSet1();
}

void daEnBomhei_c::finalizeState_Slide() {
    clrComboCnt();
    mAccelF = 0.0f;
    mSpeedMax.x = 0.0f;
    bcSet2();
    mCc.mCcData.mAttack = CC_ATTACK_NONE;
}

void daEnBomhei_c::executeState_Slide() {
    mModel.play();
    calcSpeedX();
    calcSpeedY();
    posMove();

    u32 bcFlags = EnBgCheckFoot();
    if (mBc.isFoot()) {
        Bound(0.1875f, 1.0f, 0.5f);
        if (std::fabs(mSpeed.x) > 0.2f) {
            mVec3_c efPos(mPos.x, mPos.y, 5500.0f);
            mEf::createEffect("Wm_en_landsmoke_s", 0, &efPos, nullptr, nullptr);
        }
    }

    mBc.checkHead(bcFlags);

    EnBgCheckWall();
    if (mBc.mFlags & 0x15 << mDirection) {
        mDirection ^= 1;
        mAngle.y = l_base_angleY[mDirection];
        mSpeed.x = -mSpeed.x * smc_WalkSpeed;
    }

    mEffect.createEffect("Wm_en_bombignition", 0, &mIgnitionPos, nullptr, nullptr);

    WaterCheck(mPos, 1.0f);

    mExplodeWaitTimer--;
    if (mExplodeWaitTimer <= 0) {
        changeState(StateID_Explode);
    } else if (mSpeed.x == 0.0f) {
        changeState(StateID_Sleep);
    }
}

void daEnBomhei_c::initializeState_Kick() {
    static const float dirSpeed[] = { smc_KickSpeedX, -smc_KickSpeedX };

    mSpeed.set(dirSpeed[mDirection], smc_KickSpeedY, 0.0f);
    mAngle.y = l_base_angleY[mDirection];
    clrComboCnt();
    mAccelY = -0.1875f;
    mCc.mCcData.mAttack = CC_ATTACK_SHELL;
}

void daEnBomhei_c::finalizeState_Kick() {
    clrComboCnt();
    mCc.mCcData.mAttack = CC_ATTACK_NONE;
}

void daEnBomhei_c::executeState_Kick() {
    calcSpeedY();
    posMove();

    u32 bcFlags = EnBgCheckFoot();
    if (mBc.isFoot()) {
        Bound(0.1875f, 0.5f, 0.5f);
    }

    mBc.checkHead(bcFlags);

    EnBgCheckWall();
    if (bcFlags != 0) {
        if (std::fabs(mSpeed.x) > 0.2f) {
            mVec3_c efPos(mPos.x, mPos.y, 5500.0f);
            mEf::createEffect("Wm_en_landsmoke_s", 0, &efPos, nullptr, nullptr);
        }
    }
    if (mBc.mFlags & 0x3c000000) {
        if (mSpeed.y > 0.0f) {
            mSpeed.y = -0.85f * mSpeed.y;
        }
    }
    if (mBc.mFlags & 0x15 << mDirection) {
        mDirection = mDirection ^ 1;
        mAngle.y = l_base_angleY[mDirection];
        mSpeed.x = -mSpeed.x;
    }

    mEffect.createEffect("Wm_en_bombignition", 0, &mIgnitionPos, nullptr, nullptr);

    WaterCheck(mPos, 1.0f);

    mExplodeWaitTimer--;
    if (mExplodeWaitTimer <= 0) {
        changeState(StateID_Explode);
    } else if (mSpeed.x == 0.0f) {
        changeState(StateID_Sleep);
    }
}

void daEnBomhei_c::initializeState_KickBom() {
    static const float dirSpeed[] = { smc_KickSpeedX, -smc_KickSpeedX };

    mSpeed.set(dirSpeed[mDirection], 2.0f, 0.0f);
    mAngle.y = l_base_angleY[mDirection];
    clrComboCnt();
    mAccelY = -0.1875f;
    mCc.mCcData.mAttack = CC_ATTACK_SHELL;
    m_23b = 1;
}

void daEnBomhei_c::finalizeState_KickBom() {
    clrComboCnt();
    mCc.mCcData.mAttack = CC_ATTACK_NONE;
}

void daEnBomhei_c::executeState_KickBom() {
    calcSpeedY();
    posMove();

    u32 bcFlags = EnBgCheckFoot();
    mBc.checkHead(bcFlags);
    EnBgCheckWall();
    if (mBc.mFlags & 0x15 << mDirection) {
        mDirection = mDirection ^ 1;
        mAngle.y = l_base_angleY[mDirection];
        mSpeed.x = -mSpeed.x;
    }

    WaterCheck(mPos, 1.0f);

    switch (m_23b) {
        case 1:
            if (bcFlags != 0) {
                mSpeed.y = 1.0f;
                m_23b = 2;
            }
            break;
        case 2:
            if (bcFlags != 0) {
                changeState(StateID_Explode);
            }
            break;
    }
}

void daEnBomhei_c::initializeState_Wakidashi() {
    nw4r::g3d::ResAnmChr anm = mFile.GetResAnmChr("walk");
    mAnm.setAnm(mModel, anm, m3d::FORWARD_LOOP);
    mModel.setAnm(mAnm, 2.0f);
    mAnm.setRate(2.0f);

    u8 wakidashiDir = ACTOR_PARAM(WakidashiSpawnDir);
    if (wakidashiDir & 2) {
        mDirection = (wakidashiDir & 1) ^ 1;
    } else {
        mDirection = getPl_LRflag(mPos);
    }

    static const float speedX[] = { 0.0f, 0.0f, -smc_WalkSpeed, smc_WalkSpeed };
    static const float speedY[] = { smc_WalkSpeed, -smc_WalkSpeed, 0.0f, 0.0f };

    mAngle.y = l_base_angleY[mDirection];
    mSpeed.set(speedX[wakidashiDir], speedY[wakidashiDir], 0.0f);
    mFlags |= 0x10000;
    mWakidashiTimer = 32;
    mCc.mCcData.mVsKind &= ~BIT_FLAG(CC_KIND_ENEMY);
    mCc.mCcData.mVsDamage &= ~BIT_FLAG(CC_ATTACK_FIREBALL);
}

void daEnBomhei_c::finalizeState_Wakidashi() {
    mCc.mCcData.mVsKind |= BIT_FLAG(CC_KIND_ENEMY);
    mPos.z = 1500.0f;
    mCc.mCcData.mVsDamage |= BIT_FLAG(CC_ATTACK_FIREBALL);
}

void daEnBomhei_c::executeState_Wakidashi() {
    mModel.play();
    dBaseActor_c::posMove();
    mWakidashiTimer--;
    if (mWakidashiTimer == 0) {
        changeState(StateID_Walk);
    }
}

void daEnBomhei_c::initializeState_CannonHop_Upper() {
    nw4r::g3d::ResAnmChr anm = mFile.GetResAnmChr("walk");
    mAnm.setAnm(mModel, anm, m3d::FORWARD_LOOP);
    mModel.setAnm(mAnm, 0.0f);
    mAnm.setRate(2.0f);

    static const float speedX[2][2] = {
        {1.5f, -1.5f},
        {1.3f, -1.3f}
    };
    static const float speedY[] = { 2.25f, 4.5f };

    mAccelY = -0.1875f;
    mSpeed.set(
        speedX[ACTOR_PARAM(CannonHopType)][mDirection],
        speedY[ACTOR_PARAM(CannonHopType)],
        0.0f
    );
    mBc.set(this, l_bomhei_8, l_bomhei_7, l_bomhei_9);
    mCannonHopTimer = 8;
    m_23b = 1;
}

void daEnBomhei_c::finalizeState_CannonHop_Upper() {
    mBc.set(this, l_bomhei_foot, l_bomhei_head, l_bomhei_wall);
}

void daEnBomhei_c::executeState_CannonHop_Upper() {
    mModel.play();
    calcSpeedY();
    posMove();

    switch (m_23b) {
        case 1:
            mBc.checkWall(nullptr);
            mBc.checkHead(0);
            mCannonHopTimer--;
            if (mCannonHopTimer <= 0) {
                mBc.set(this, l_bomhei_foot, l_bomhei_head, l_bomhei_wall);
                m_23b = 2;
            }
            break;
        case 2:
            if (mBc.checkFootEnm()) {
                mAnm.setRate(1.0f);
                changeState(StateID_Walk);
            } else if (mBc.checkWall(nullptr)) {
                mSpeed.x = -mSpeed.x;
                changeState(StateID_Turn);
            }
            break;
    }
}

void daEnBomhei_c::initializeState_CannonHop_Under() {
    nw4r::g3d::ResAnmChr anm = mFile.GetResAnmChr("walk");
    mAnm.setAnm(mModel, anm, m3d::FORWARD_LOOP);
    mModel.setAnm(mAnm, 0.0f);
    mAnm.setRate(1.0f);
    mAnm.setRate(2.0f);

    static const float speedX[] = { 1.25f, -1.25f };

    mAccelY = -0.1875f;
    mSpeed.set(speedX[mDirection], -0.75f, 0.0f);
    mCannonHopTimer = 8;
    mBc.set(this, l_bomhei_8, l_bomhei_7, l_bomhei_9);
    m_23b = 1;
}

void daEnBomhei_c::finalizeState_CannonHop_Under() {
    mBc.set(this, l_bomhei_foot, l_bomhei_head, l_bomhei_wall);
}

void daEnBomhei_c::executeState_CannonHop_Under() {
    mModel.play();
    calcSpeedY();
    posMove();

    switch (m_23b) {
        case 1:
            mBc.checkWall(nullptr);
            mBc.checkHead(0);
            mCannonHopTimer--;
            if (mCannonHopTimer <= 0) {
                mBc.set(this, l_bomhei_foot, l_bomhei_head, l_bomhei_wall);
                m_23b = 2;
            }
            break;
        case 2:
            if (mBc.checkFootEnm()) {
                mAnm.setRate(1.0f);
                changeState(StateID_Walk);
            } else if (mBc.checkWall(nullptr)) {
                mSpeed.x = -mSpeed.x;
                changeState(StateID_Turn);
            }
            break;
    }
}

void daEnBomhei_c::initializeState_AfterIce() {
    mAnmMat.setFrame(0.0f, 0);
    m_23b = 1;
}

void daEnBomhei_c::finalizeState_AfterIce() {}

void daEnBomhei_c::executeState_AfterIce() {
    calcSpeedY();
    posMove();
    if (mBc.checkWall(nullptr)) {
        mSpeed.x = -mSpeed.x;
    }

    switch (m_23b) {
        case 1:
            if (mBc.checkFootEnm()) {
                nw4r::g3d::ResAnmChr anm = mFile.GetResAnmChr("walk");
                mAnm.setAnm(mModel, anm, m3d::FORWARD_LOOP);
                mModel.setAnm(mAnm, 5.0f);
                mAnm.setRate(1.0f);
                mSpeed.set(0.0f, 0.0f, 0.0f);
                mDirection = getPl_LRflag(mPos);
                m_23b = 2;
            }
            break;
        case 2:
            mModel.play();
            if (mBc.checkFootEnm()) {
                mSpeed.y = 0.0f;
            }
            if (mAngle.y.chase(l_base_angleY[mDirection], 0x400)) {
                changeState(StateID_Walk);
            }
            break;
    }
}

void daEnBomhei_c::initializeState_EatOut() {
    dEn_c::initializeState_EatOut();
    nw4r::g3d::ResAnmChr anm = mFile.GetResAnmChr("stop");
    mAnm.setAnm(mModel, anm, m3d::FORWARD_LOOP);
    mModel.setAnm(mAnm, 0.0f);
    mAnm.setRate(1.0f);
}

void daEnBomhei_c::finalizeState_EatOut() {
    dEn_c::finalizeState_EatOut();
}

void daEnBomhei_c::executeState_EatOut() {
    mEffect.createEffect("Wm_en_bombignition", 0, &mIgnitionPos, nullptr, nullptr);
    mExplodeWaitTimer--;
    if (mExplodeWaitTimer <= 0) {
        changeState(StateID_Explode);
    } else {
        dEn_c::executeState_EatOut();
    }
}

void daEnBomhei_c::initializeState_Explode() {
    explosionEffect();
    breakEffect();
    dAudio::SoundEffectID_t(SE_EMY_BH_BOMB).playEmySound(mPos, 0);

    mCc.mCcData.mOffset.x = 0.0f;
    mCc.mCcData.mOffset.y = 0.0f;
    mCc.mCcData.mSize.x = 18.0f;
    mCc.mCcData.mSize.y = 18.0f;
    mCc.mCcData.mAttack = CC_ATTACK_SHELL;
    mCc.mCcData.mVsKind |= BIT_FLAG(CC_KIND_PLAYER) | BIT_FLAG(CC_KIND_PLAYER_ATTACK) | BIT_FLAG(CC_KIND_YOSHI) |
                           BIT_FLAG(CC_KIND_ENEMY) | BIT_FLAG(CC_KIND_TAMA) | BIT_FLAG(CC_KIND_KILLER);
    mCc.mCcData.mVsDamage = 0;
    mCc.mCcData.mStatus |= CC_STATUS_NO_REVISION;
    mCc.mCcData.mCallback = explosionCallback;

    mExplodeWaitTimer = 0;
    mAngle.y = l_base_angleY[mDirection];
    mPos.y += 8.0f;
    checkBombBreak();
}

void daEnBomhei_c::finalizeState_Explode() {}

void daEnBomhei_c::executeState_Explode() {
    mExplodeTimer++;
    if (mExplodeTimer < 9) {
        explodeBgUnit();
    } else if (mExplodeTimer == 16) {
        deleteActor(true);
    }
}

void daEnBomhei_c::initializeState_InIceLump() {
    mAngle.x = dGameCom::rndInt(9) * 0x1000 - 0x4000;
    mAngle.y = dGameCom::rndInt(9) * 0x1000 - 0x4000;
    mAngle.z = dGameCom::rndInt(9) * 0x1000 - 0x4000;
    mCc.release();
}

void daEnBomhei_c::finalizeState_InIceLump() {}

void daEnBomhei_c::executeState_InIceLump() {}

const int daEnBomhei_c::smc_SoundEffectIDs[] = {
    SE_EMY_YOSHI_HPDP,
    SE_EMY_DOWN,
    SE_EMY_KAME_KERU,
    SE_EMY_BH_HIBANA,
    SE_EMY_BH_BOMB
};
