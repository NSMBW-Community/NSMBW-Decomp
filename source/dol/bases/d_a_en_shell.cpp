#include <game/bases/d_a_en_shell.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_yoshi.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <game/bases/d_score_manager.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/sLib/s_GlobalData.hpp>
#include <constants/sound_list.h>

STATE_VIRTUAL_DEFINE(daEnShell_c, Sleep);
STATE_VIRTUAL_DEFINE(daEnShell_c, Carry);
STATE_VIRTUAL_DEFINE(daEnShell_c, Slide);
STATE_VIRTUAL_DEFINE(daEnShell_c, Wakeup);
STATE_VIRTUAL_DEFINE(daEnShell_c, WakeupReverse);
STATE_VIRTUAL_DEFINE(daEnShell_c, WakeupTurn);
STATE_VIRTUAL_DEFINE(daEnShell_c, DieFall);

static const float l_slide_throw_speed[] = { 2.5f, -2.5f };
static const float l_slide_max_speed[] = { 3.0f, -3.0f };

template<>
const daEnShell_c::GlobalData_t sGlobalData_c<daEnShell_c>::mData = {
    0xc00,
    0x1d00,
    1.5f,1.0f
};

daEnShell_c::daEnShell_c() : mYoshiKickable(false), mJumpPlayerCarryActorID(BASE_ID_NULL), mIsMugenCombo(false), mMugenComboSpeed(0.0f) {
    mFumiProc.refresh(new NonUniqueFumiCheck_c());
    mEatBehaviour = EAT_TYPE_EAT;
    mFlags = EN_IS_SHELL | EN_IS_HARD;
}

void DUMMY_UNUSED() {
    UniqueFumiCheckInf_c a;
}

daEnShell_c::~daEnShell_c() {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && fManager_c::searchBaseByID(player->mCarryActorID) == this) {
            player->cancelCarry(this);
        }
    }
}

void daEnShell_c::createShell(const char *arcName, const char *resPath, const char *modelName, const char *anmTexName, float animFrame) {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);

    mResFile = dResMng_c::m_instance->getRes(arcName, resPath);
    nw4r::g3d::ResMdl mdl = mResFile.GetResMdl(modelName);
    mModel.create(mdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC | nw4r::g3d::ScnMdl::ANM_TEXPAT);

    dActor_c::setSoftLight_Enemy(mModel);

    if (mProfName == fProfile::EN_NOKONOKO || mProfName == fProfile::EN_PATAPATA) {
        nw4r::g3d::ResAnmChr anim = mResFile.GetResAnmChr("revival_shell");
        mAnim.create(mdl, anim, &mAllocator);
    }

    if (anmTexName != nullptr) {
        mResAnmTexPat = mResFile.GetResAnmTexPat(anmTexName);
        mAnimTex.create(mdl, mResAnmTexPat, &mAllocator);
        mAnimTex.setAnm(mModel, mResAnmTexPat, 0, m3d::FORWARD_ONCE);
        mModel.setAnm(mAnimTex);
        mAnimTex.setFrame(animFrame, 0);
        mAnimTex.setRate(0.0f, 0);
    }

    mAllocator.adjustFrmHeap();
}

void daEnShell_c::calcShellMdl() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;

    if (isState(daEnCarry_c::StateID_Carry)) {
        pos = calcCarryPos(mCarryPos);
        mPos.x = pos.x;
        mPos.y = pos.y;
    } else if (mIsCarryFall) {
        pos.z = 3248.0f;
    }

    changePosAngle(&pos, &angle, 1);

    float wakeupShakeYOffset = 0.0f;
    if (mSleepTimer < smc_SLEEP_TIMER_SHAKE && mSleepTimer != 0 && mSpeed.y == 0.0f && mIsFlipped != 0) {
        wakeupShakeYOffset = 16.0f;
    }

    if (mIsFlipped) {
        if (angle.y >= 0) {
            angle.y = -angle.y.mAngle + 0x8000;
        } else {
            angle.y = -angle.y.mAngle - 0x8000;
        }
    }

    mMatrix.trans(pos.x, pos.y, pos.z);
    mMatrix.YrotM(l_EnMuki[mDirection] * 0x4000);

    mMatrix.concat(mMtx_c::createTrans(0.0f, 7.0f, 0.0f));
    mMatrix.XrotM(angle.x);
    mMatrix.concat(mMtx_c::createTrans(0.0f, -7.0f, 0.0f));

    mMatrix.concat(mMtx_c::createTrans(0.0f, wakeupShakeYOffset, 0.0f));
    mMatrix.XrotM(mWakeupShakeAngle3D.x);
    mMatrix.ZrotM(mWakeupShakeAngle3D.z);
    mMatrix.concat(mMtx_c::createTrans(0.0f, -wakeupShakeYOffset, 0.0f));

    mMatrix.YrotM(l_EnMuki[mDirection ^ 1] * 0x4000);
    mMatrix.YrotM(angle.y);

    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(mBoyoMng.getScale());
    mModel.calc(false);

    calcShellEffectPos();
}

void daEnShell_c::postExecute(fBase_c::MAIN_STATE_e status) {
    if (status == SUCCESS) {
        if (!isState(StateID_Ice)) {
            if (!mNoRespawn && hasamiCheck()) {
                setDeathInfo_Hasami();
            }
            mVec3_c pos = mPos;
            if (!mUseBaseIceBehaviour) {
                pos = getCenterPos();
            }
            WaterCheck(pos, 1.0f);
            if (mIsCarryFall && mBc.isFoot()) {
                mIsCarryFall = 0;
            }
        }

        if (mJumpPlayerNoCarryHitTimer > 0) {
            mJumpPlayerNoCarryHitTimer--;
            if (mJumpPlayerNoCarryHitTimer == 0) {
                dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(mJumpPlayerCarryActorID);
                if (actor != nullptr) {
                    actor->mCc.mpFriendActor = nullptr;
                }
                mJumpPlayerCarryActorID = BASE_ID_NULL;
            }
        }
    }

    return dEn_c::postExecute(status);
}

bool daEnShell_c::drawShell() {
    mModel.entry();
    return true;
}

bool daEnShell_c::EtcDamageCheck(dCc_c *self, dCc_c *other) {
    if (mJumpPlayerCarryActorID == other->mpOwner->mUniqueID) {
        return false;
    }
    return dEn_c::EtcDamageCheck(self, other);
}

void daEnShell_c::Normal_VsEnHitCheck(dCc_c *self, dCc_c *other) {
    dActor_c *otherActor = other->getOwner();
    if (
        isState(daEnCarry_c::StateID_Carry) &&
        other->mCcData.mVsDamage & (1 << CC_ATTACK_SHELL) &&
        otherActor->mProfName != fProfile::EN_HATENA_BALLOON &&
        hitCallback_Shell(self, other)
    ) {
        other->mInfo |= CC_NO_HIT;
        return;
    }
    if (
        isState(StateID_Slide) &&
        (other->mCcData.mStatus & CC_STATUS_8 &&
        hitCallback_Shell(self, other))
    ) {
        self->mInfo |= CC_NO_HIT;
        return;
    }
    float collOffsetX = mCc.mCollOffsetX[CC_KIND_ENEMY];
    if (
        mDirection == DIR_LR_L && collOffsetX > 0.0f ||
        mDirection == DIR_LR_R && collOffsetX < 0.0f
    ) {
        setEnemyTurn();
    }
}

void daEnShell_c::Normal_VsPlHitCheck(dCc_c *self, dCc_c *other) {
    dAcPy_c *player = (dAcPy_c *) other->getOwner();
    if (fManager_c::searchBaseByID(player->mCarryActorID) == this) {
        return;
    }

    if (!player->isStatus(0x2b) && isState(StateID_Sleep)) {
        if (carry_check(player)) {
            mCarriedBy = mPlayerNo;
            mCarryPos.set(0.0f, -5.0f, 6.0f);
            changeState(StateID_Carry);
            return;
        }
        if (!player->isItemKinopio()) {
            dScoreMng_c *scoreMng;
            int fumiCount = dEnCombo_c::calcPlFumiCnt(player);
            if (fumiCount < 1) {
                fumiCount = 1;
            }
            scoreMng = dScoreMng_c::m_instance;
            scoreMng->ScoreSet(this, fumiCount, *player->getPlrNo());
        }
        dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(player->mCarryActorID);
        if (actor != nullptr) {
            if (actor->mKind == STAGE_ACTOR_PLAYER) {
                mNoHitPlayer.mTimer[*actor->getPlrNo()] = 10;
            } else {
                actor->mCc.mpFriendActor = this;
                mJumpPlayerCarryActorID = actor->mUniqueID;
                mJumpPlayerNoCarryHitTimer = 12;
            }
        }
        if (mInLiquid) {
            mNoHitPlayer.mTimer[*player->getPlrNo()] = 16;
        }
        setKickSlide(self, player);
        changeState(StateID_Slide);
        return;
    }

    bool fumiInvalid = isFumiInvalid();
    int x = 0;
    if (fumiInvalid) {
        x = 2;
    }

    int fumiRes = Enfumi_check(self, other, x);
    if (fumiRes != 0) {
        if (fumiRes == 1) {
            mSpeed.y = 0.0f;
            if (isState(daEnCarry_c::StateID_Carry)) {
                setDeathInfo_CarryBgIn(player);
            } else if (!specialFumiProc(player)) {
                mSpeed.set(0.0f, 0.0f, 0.0f);
                mFootPush.set(0.0f, 0.0f, 0.0f);
                if (mIsMugenCombo) {
                    mPos.x = mMugenComboPosX;
                }
                changeState(StateID_Sleep);
            }
        } else if (fumiRes == 3) {
            if (fumiInvalid) {
                dEn_c::Normal_VsPlHitCheck(self, other);
            } else {
                setDeathInfo_SpinFumi(player, 1);
            }
        } else if (fumiRes == 2 && fumiInvalid) {
            dEn_c::Normal_VsPlHitCheck(self, other);
        }
    } else if (isState(StateID_Sleep)) {
        if (!player->isItemKinopio()) {
            dScoreMng_c *scoreMng;
            int fumiCount = dEnCombo_c::calcPlFumiCnt(player);
            if (fumiCount < 1) {
                fumiCount = 1;
            }
            scoreMng = dScoreMng_c::m_instance;
            scoreMng->ScoreSet(this, fumiCount, *player->getPlrNo());
        }
        dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(player->mCarryActorID);
        if (actor != nullptr) {
            if (actor->mKind == STAGE_ACTOR_PLAYER) {
                mNoHitPlayer.mTimer[*actor->getPlrNo()] = 10;
            } else {
                actor->mCc.mpFriendActor = this;
                mJumpPlayerCarryActorID = actor->mUniqueID;
                mJumpPlayerNoCarryHitTimer = 12;
            }
        }
        setKickSlide(self, player);
        changeState(StateID_Slide);
    } else if (!isState(daEnCarry_c::StateID_Carry) && !player->isNoDamage()) {
        if (isState(StateID_Slide) && isSlideTowards(player)) {
            if (!mIsMugenCombo) {
                setKickSlide(self, player);
                if (mInLiquid) {
                    mNoHitPlayer.mTimer[mPlayerNo] = 24;
                    mAccelF = 0.1f;
                    mSpeed.x = l_EnMuki[mDirection] * 7.0f;
                }
            }
        } else {
            dEn_c::Normal_VsPlHitCheck(self, other);
        }
    }
}

void daEnShell_c::Normal_VsYoshiHitCheck(dCc_c *self, dCc_c *other) {
    daYoshi_c *yoshi = (daYoshi_c *) other->getOwner();

    bool fumiInvalid = isFumiInvalid();
    int x = 0;
    if (fumiInvalid) {
        x = 1;
    }
    int fumiRes = Enfumi_check(self, other, x);
    if (fumiRes != 0) {
        if (fumiRes == 1) {
            mSpeed.y = 0.0f;
            if (isState(daEnCarry_c::StateID_Carry)) {
                setDeathInfo_CarryBgIn(yoshi);
            } else if (!specialFumiProc_Yoshi(yoshi)) {
                setDeathInfo_YoshiFumi(yoshi);
            }
        }
    } else if (isState(StateID_Sleep) || mYoshiKickable) {
        dScoreMng_c *scoreMng;
        int fumiCount = dEnCombo_c::calcPlFumiCnt(yoshi);
        if (fumiCount < 1) {
            fumiCount = 1;
        }
        scoreMng = dScoreMng_c::m_instance;
        scoreMng->ScoreSet(this, fumiCount, *yoshi->getPlrNo());
        setKickSlide(self, yoshi);
        changeState(StateID_Slide);
    } else if (!isState(daEnCarry_c::StateID_Carry) && !yoshi->isNoDamage()) {
        if (isState(StateID_Slide) && isSlideTowards(yoshi)) {
            if (!mIsMugenCombo) {
                setKickSlide(self, yoshi);
            }
        } else {
            dEn_c::Normal_VsYoshiHitCheck(self, other);
        }
    }
}

bool daEnShell_c::checkComboClap(int max) {
    if (mIsMugenCombo) {
        return max >= 8;
    }
    return dEn_c::checkComboClap(max);
}

void daEnShell_c::FumiJumpSet(dActor_c *actor) {
    if (isFumiInvalid()) {
        return;
    }
    float jumpSpeed = dAcPy_c::msc_JUMP_SPEED + 0.2815f;
    float speedF = actor->mSpeedF;
    if (mIsMugenCombo && std::fabs(speedF) < 1.0f) {
        speedF = 0.0f;
    }
    ((daPlBase_c *) actor)->vf3fc(jumpSpeed, speedF, 1, 0, 2);
    dEnemyMng_c::m_instance->m_138 = 1;
}

bool daEnShell_c::hitCallback_Shell(dCc_c *self, dCc_c *other) {
    daPlBase_c *shell = (daPlBase_c *) other->getOwner();
    u8 dir = getTrgToSrcDir_Main(getCenterX(), shell->getCenterX());
    int plrNo = acmShellPlayerNo(shell);
    shellDamageEffect(self, shell);

    int score = -1;

    if (plrNo < 0) {
        dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN, mPos, 0);
    } else {
        shell->slideComboSE(shell->mComboMultiplier, false);
        shell->mComboMultiplier++;
        if (shell->mComboMultiplier >= 8) {
            shell->mComboMultiplier = 8;
        }
        score = mCombo.getComboScore(shell->mComboMultiplier);
    }

    float speedMult = 1.0f;
    if (isState(daEnCarry_c::StateID_Carry)) {
        speedMult = 0.7f;
    }

    mDeathInfo = (sDeathInfoData) {
        speedMult * l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &dEn_c::StateID_DieFall,
        score,
        -1,
        dir,
        (u8) plrNo
    };

    return true;
}

void daEnShell_c::FumiScoreSet(dActor_c *actor) {
    if (isFumiInvalid()) {
        return;
    }
    if (mIsMugenCombo && actor->mSpeed.y > 0.0f) {
        return;
    }
    dEn_c::FumiScoreSet(actor);
}

void daEnShell_c::setCarryFall(dActor_c *carryingActor, int collisionDelay) {
    int plrNo = *carryingActor->getPlrNo();
    if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
        mNoHitPlayer.mTimer[plrNo] = collisionDelay;
        mIsCarryFall = 1;
    }
}

bool daEnShell_c::setDamage(dActor_c *actor) {
    return setPlayerDamage(actor);
}

void daEnShell_c::setKickSlide(dCc_c *self, dActor_c *other) {
    mPlayerNo = *other->getPlrNo();
    mDirection = getTrgToSrcDir_Main(getCenterX(), other->getCenterX());
    mNoHitPlayer.mTimer[mPlayerNo] = 4;
    setSlideKickSpeed(other);
    kickSE(mPlayerNo);
    float collY = self->mCollPos.y;
    float collX = self->mCollPos.x;
    kickEffect(mVec3_c(collX, collY, 5500.0f));
}

bool daEnShell_c::isSlideTowards(dActor_c *actor) {
    if (mPlayerNo == -1) {
        return false;
    }

    if (!mBc.isFoot() && mPos.y >= actor->mPos.y + 4.0f) {
        return false;
    }

    static const float dir[] = { 1.0f, -1.0f };

    u8 idx = !(actor->mPos.x < mPos.x);
    if (mSpeed.x * dir[idx] >= 0.0f) {
        return true;
    }

    return false;
}

bool daEnShell_c::checkMugenCombo(dActor_c *actor) {
    daPlBase_c *player = (daPlBase_c *) actor;

    if (!ACTOR_PARAM(MugenComboAllowed)) {
        return false;
    }
    if (player->mNowBgCross1 & 1) {
        return false;
    }
    if (player->mSpeed.y > 0.0f) {
        return false;
    }

    mVec3_c playerPos = player->mPos;
    playerPos.x += l_EnMuki[mDirection] * 1.5f;

    float height = 0.0f;
    if (dBc_c::checkGround(&playerPos, &height, player->mLayer, 1, -1) && player->mPos.y < height + 16.0f) {
        return false;
    }

    mVec3_c centerPos = getCenterPos();

    mVec3_c tileOppositeSide(centerPos.x + l_EnMuki[mDirection ^ 1] * 16.0f, centerPos.y - 16.0f, centerPos.z);
    if (dBc_c::checkBg(tileOppositeSide.x, tileOppositeSide.y, mLayer, 3, 0x819)) {
        return false;
    }

    mVec3_c tileSameSide(centerPos.x + l_EnMuki[mDirection] * 16.0f, centerPos.y, centerPos.z);
    float wallX = 0.0f;
    if (!dBc_c::checkWall(&centerPos, &tileSameSide, &wallX, mLayer, 1, nullptr)) {
        return false;
    }

    float wallDist = std::fabs(wallX - centerPos.x);
    float minDist = 8.0f;
    if (player->mTreadCount < 2) {
        minDist = 10.0f;
    }
    if (wallDist < minDist || wallDist > 16.0f) {
        return false;
    }

    if (mDirection == DIR_LR_R) {
        mMugenComboPosX = wallX - 14.0f;
        mPos.x = wallX - 15.0f;
    } else {
        mMugenComboPosX = wallX + 14.0f;
        mPos.x = wallX + 15.0f;
    }

    return true;
}

bool daEnShell_c::hitCallback_Spin(dCc_c *self, dCc_c *other) {
    if (!isFumiInvalid()) {
        dActor_c *actor = other->getOwner();

        setDeathSound_HipAttk();
        hipatkEffect(actor->mPos);
        SpinFumiScoreSet(actor);
        setDeathInfo_SpinFumi(actor, 1);
        return true;
    }
    return false;
}

bool daEnShell_c::hitCallback_HipAttk(dCc_c *self, dCc_c *other) {
    if (isFumiInvalid()) {
        return false;
    }

    dActor_c *actor = other->getOwner();

    setDeathSound_HipAttk();
    hipatkEffect(actor->mPos);
    SpinFumiScoreSet(actor);

    u8 dir = getTrgToSrcDir_Main(getCenterX(), actor->getCenterX());
    u8 plrNo = *actor->getPlrNo();

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &dEn_c::StateID_DieFall,
        -1,
        -1,
        dir,
        plrNo
    };

    return true;
}

bool daEnShell_c::hitCallback_YoshiHipAttk(dCc_c *self, dCc_c *other) {
    daYoshi_c *yoshi = (daYoshi_c *) other->getOwner();

    if (isFumiInvalid()) {
        float speedF = 0.0f;
        float jumpSpeed = 4.5f;
        yoshi->vf3fc(jumpSpeed, speedF, 1, 0, 2);
        yoshifumistepSE(yoshi);
        mNoHitPlayer.mTimer[*yoshi->getPlrNo()] = 5;
    } else {
        dEn_c::hitCallback_YoshiHipAttk(self, other);
    }

    return true;
}

bool daEnShell_c::cullCheck() {
    if (isState(StateID_Slide) && cullCheck_Shell()) {
        deleteActor(mNoRespawn);
        return true;
    }

    return ActorScrOutCheck(0);
}

bool daEnShell_c::cullCheck_Shell() {
    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;

    float xStart = bgParam->xStart();
    float yStart = bgParam->yStart();
    float xEnd = bgParam->xStart() + bgParam->xSize();
    float yEnd = bgParam->yStart() - bgParam->ySize();

    float right = mPos.x + mVisibleAreaSize.x * 0.5f;
    float left = mPos.x - mVisibleAreaSize.x * 0.5f;
    float bottom = mPos.y + mVisibleAreaSize.y;
    float top = mPos.y;

    if (right < xStart - 160.0f) {
        return true;
    }
    if (left > xEnd + 160.0f) {
        return true;
    }
    if (top > yStart + 256.0f) {
        return true;
    }

    float offs = 256.0f;
    if (mBc.mFlags & 0x8000) {
        const dRide_ctr_c *ride = mRc.getRide();
        if (ride != nullptr && ride->mOwner != nullptr && ride->mOwner->mProfName == fProfile::AC_FLOOR_GYRATION) {
            offs = 64.0f;
        }
    }
    if (bottom < yEnd - offs) {
        return true;
    }

    return false;
}

bool daEnShell_c::ActorDrawCullCheck() {
    if (isState(daEnCarry_c::StateID_Carry) || isState(StateID_Slide)) {
        return false;
    }

    return dActor_c::ActorDrawCullCheck();
}

void daEnShell_c::block_hit_init() {
    mDirection = mDeathFallDirection;
    if (isBlockHitDeath()) {
        dEn_c::block_hit_init();
        return;
    }
    mVec3_c efPos(mVec2_c(mPos.x, mPos.y), 5500.0f);
    hitdamageEffect(efPos);
    dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN, mPos, 0);
    mIsFlipped = true;
    mSpeed.set(l_base_fall_speed_x[mDirection] * 0.5f, 3.5f, 0.0f);
    if (isState(StateID_Sleep)) {
        mStateMgr.initializeState();
    } else {
        changeState(StateID_Sleep);
    }
}

void daEnShell_c::setSlideThrowSpeed(dActor_c *actor) {
    float baseSpeed = actor->mSpeed.x;
    float speed = l_slide_throw_speed[mDirection];
    if (mDirection == baseSpeed < 0.0f) {
        speed += baseSpeed * 0.35f;
    }
    mCc.mpFriendActor = nullptr;
    if (actor->mKind == STAGE_ACTOR_PLAYER) {
        dAcPy_c *player = (dAcPy_c *) actor;
        if (player->m_1308 == BASE_ID_NULL) {
            mCc.mpFriendActor = player;
        }
    } else if (actor->mKind == STAGE_ACTOR_YOSHI) {
        daYoshi_c *yoshi = (daYoshi_c *) actor;
        if (yoshi->isStar()) {
            mCc.mpFriendActor = yoshi;
        }
    }

    if (speed < mSpeedMax.x) {
        float tmp;
        if (speed > mSpeedMax.x) {
            tmp = mSpeedMax.x;
        } else {
            tmp = speed;
        }
        speed = tmp;
    } else if (speed > mSpeedMax.x) {
        float tmp;
        if (speed < mSpeedMax.x) {
            tmp = mSpeedMax.x;
        } else {
            tmp = speed;
        }
        speed = tmp;
    }
    mSpeed.set(speed, 0.2f, 0.0f);
}

void daEnShell_c::setSlideKickSpeed(dActor_c *actor) {
    float baseSpeed = actor->mSpeed.x;
    float speed = l_slide_throw_speed[mDirection];
    if (mDirection == baseSpeed < 0.0f) {
        speed += baseSpeed * 0.5f;
    }
    mSpeed.x = speed;
}

void daEnShell_c::slideSpin() {
    float tmp = std::fabs(mSpeed.x) / 2.5f;
    s16 angle = tmp * GLOBAL_DATA.mSpinSpeed;
    if (mDirection == DIR_LR_L) {
        angle = -angle;
    }
    mAngle.y += angle;
}

void daEnShell_c::kickSE(int plrNo) {
    if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
        dAudio::SoundEffectID_t se(SE_EMY_KAME_KERU);
        se.playEmySound(getCenterPos(), dAudio::getRemotePlayer(mPlayerNo));
    }
}

void daEnShell_c::setDeathInfo_CarryBgIn(dActor_c *actor) {
    mDeathInfo = (sDeathInfoData) {
        0.0f,
        3.0f,
        -4.0f,
        -0.1875f,
        &dEn_c::StateID_DieFall,
        -1,
        -1,
        mDirection,
        (u8) *actor->getPlrNo()
    };
}

void daEnShell_c::setEatTongue(dActor_c *actor) {
    dEn_c::setEatTongue(actor);
    mPlayerNo = *actor->getPlrNo();
}

bool daEnShell_c::setEatSpitOut(dActor_c *actor) {
    calcSpitOutPos(actor);
    reviveCc();
    setAfterEatScale();
    mEatState = EAT_STATE_NONE;
    mDirection = actor->mDirection;
    mPlayerNo = *actor->getPlrNo();

    u32 unit1 = dBc_c::getUnitType(mPos.x, mPos.y, mLayer);
    u32 unit2 = dBc_c::getUnitType(mPos.x, mPos.y + 16.0f, mLayer);

    bool smth = false;
    if ((unit1 & 0x1001d) && !(unit2 & 0x1001d)) {
        smth = true;
        mPos.y = (int) (mPos.y / 16.0f) * 16.0f;
    }
    if (!smth && mBc.checkWallEnm(nullptr)) {
        mPos.x = actor->mPos.x;
    }

    mNoHitPlayer.mTimer[mPlayerNo] = 16;
    setSlideThrowSpeed(actor);
    mSpeed.y = 1.4f;
    mAccelY = -0.09375f;

    daPlBase_c *player = (daPlBase_c *) actor;
    if (player->isStatus(0x30)) {
        static const float cs_slip_offset[] = { 8.0f, -8.0f };
        mPos.x += cs_slip_offset[mDirection];
    }
    changeState(StateID_Slide);

    return true;
}

bool daEnShell_c::hitCallback_Ice(dCc_c *self, dCc_c *other) {
    if (mIceMng.mActive) {
        return true;
    }

    if (other->mpOwner->mSpeed.x >= 0.0f) {
        mBoyoMng.mDirection = DIR_LR_R;
    } else {
        mBoyoMng.mDirection = DIR_LR_L;
    }
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && fManager_c::searchBaseByID(player->mCarryActorID) == this) {
            player->cancelCarry(this);
            break;
        }
    }
    if (!mUseBaseIceBehaviour) {
        changeState(StateID_Ice);
        mPlayerNo = -1;
    } else {
        dEn_c::hitCallback_Ice(self, other);
    }
    return true;
}

void daEnShell_c::returnState_Ice() {
    if (!mUseBaseIceBehaviour) {
        mPlayerNo = -1;
        mSpeed.set(0.0f, 0.0f, 0.0f);
        changeState(StateID_Sleep);
    } else {
        dEn_c::returnState_Ice();
    }
}

void daEnShell_c::adjustCarryCc() {
    const static float cs_check_ofs[] = { -8.0f, 8.0f };
    bool dir = mAngle.y < 0;

    mVec3_c pos(getCenterPos().x + cs_check_ofs[dir], getCenterPos().y, getCenterPos().z);
    mVec3_c pos2(pos.x + l_EnMuki[dir] * 16.0f, pos.y, pos.z);

    mCc.mCcData.mOffset.set(mCcOffset.x, mCcOffset.y);
    mCc.mCcData.mSize.set(mCcSize.x, mCcSize.y);

    float wallX = 0.0f;
    if (dBc_c::checkWall(&pos, &pos2, &wallX, mLayer, 1, nullptr)) {
        float offsX = (pos.x + wallX) * 0.5f;
        float sizeX = std::fabs((pos.x - wallX) * 0.5f);

        mCc.mCcData.mOffset.set(offsX - mPos.x, 8.0f);
        mCc.mCcData.mSize.set(sizeX, 8.0f);
    }
}

bool daEnShell_c::checkSleep() {
    if (mCarryTimer > 0) {
        mCarryTimer--;
        return false;
    }

    if (isFunsui()) {
        return false;
    }

    mSleepTimer--;
    if (mSleepTimer < smc_SLEEP_TIMER_SHAKE) {
        mWakeupShakeAngle += GLOBAL_DATA.mUnkSpeed;
        float sin = mWakeupShakeAngle.sin();
        float cos = mWakeupShakeAngle.cos();
        mWakeupShakeAngle3D.x.mAngle = sin * (0x10000 / 128.0f * 5.0f);
        mWakeupShakeAngle3D.z.mAngle = cos * (0x10000 / 128.0f * 5.0f);
        if (mSleepTimer == 0) {
            mWakeupShakeAngle3D.x = 0;
            mWakeupShakeAngle3D.z = 0;
            return true;
        }
    }
    return false;
}

void daEnShell_c::setSpinLiftUpActor(dActor_c *carryingActor) {
    s16 plrNo = *carryingActor->getPlrNo();
    mCarriedBy = plrNo;
    mPlayerNo = plrNo;
    mCarryPos.set(0.0f, 0.0f, 0.0f);
    changeState(StateID_Carry);
}

bool daEnShell_c::hasamiCheck() {
    if (mBgCollFlags != COLL_NONE) {
        if ((mBgCollFlags & COLL_WALL_R) && mBc.isWallR() && !(mBc.mFlags & dBc_c::FLAG_8)) {
            return true;
        }

        if ((mBgCollFlags & COLL_WALL_L) && mBc.isWallL() && !(mBc.mFlags & dBc_c::FLAG_8)) {
            return true;
        }

        if ((mBgCollFlags & COLL_FOOT) && mBc.isFoot()) {
            return true;
        }

        if ((mBgCollFlags & COLL_HEAD) && mBc.isHead() && !(mBc.getFlags() & dBc_c::FLAG_30)) {
            return true;
        }
    }

    return false;
}

void daEnShell_c::setDeathInfo_Hasami() {
    u8 dir = !(mPos.x - mLastPos.x >= 0.0f);
    mVec3_c efPos(mVec2_c(mPos.x, mPos.y + mCenterOffs.y), 5500.0f);
    hitdamageEffect(efPos);
    removeCc();
    dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN, mPos, 0);
    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &dEn_c::StateID_DieFall,
        -1,
        -1,
        dir,
        (u8) -1
    };
}

void daEnShell_c::initializeState_Sleep() {
    mSleepTimer = smc_SLEEP_TIMER;
    mSpeedMax.y = -4.0f;
    mAccelY = -0.1875f;
    clrComboCnt();
    mUseBaseIceBehaviour = 0;
    mCc.mCcData.mVsDamage |= (1 << CC_ATTACK_SPIN_LIFT_UP);
    mWakeupShakeAngle = 0;
    mWakeupShakeAngle3D.x = 0;
    mWakeupShakeAngle3D.y = 0;
    mWakeupShakeAngle3D.z = 0;
}

void daEnShell_c::finalizeState_Sleep() {
    mCc.mCcData.mVsDamage &= ~(1 << CC_ATTACK_SPIN_LIFT_UP);
    mActorProperties &= ~BIT_FLAG(2);
    mComboMultiplier = 0;
}

void daEnShell_c::executeState_Sleep() {
    if (mIsFlipped) {
        u16 ang = mAngle.x;
        if (mDirection) {
            ang -= 0x800;
            if (ang < 0x8000) {
                ang = 0x8000;
            }
        } else {
            ang += 0x800;
            if (ang > 0x8000) {
                ang = -0x8000;
            }
        }
        mAngle.x = ang;
    }
    calcSpeedY();
    posMove();

    u32 bgCheckRes = EnBgCheck();
    if (bgCheckRes & 1) {
        Bound(0.1875f, 0.5f, 0.5f);
    }
    if (bgCheckRes & 2) {
        mSpeed.y = 0.0f;
    }
    if (bgCheckRes & 4) {
        mSpeed.x = 0.0f;
    }

    if (checkSleep()) {
        if (mIsFlipped) {
            changeState(StateID_WakeupReverse);
        } else {
            changeState(StateID_Wakeup);
        }
    }
}

void daEnShell_c::initializeState_Carry() {
    mPlayerNo = mCarriedBy;
    dAcPy_c *player = daPyMng_c::getPlayer(mPlayerNo);
    if (player->mAmiLayer == 1) {
        mAmiLayer = 0;
    } else {
        mAmiLayer = 1;
    }
    mCc.mAmiLine = l_Ami_Line[mAmiLayer];
    mBc.mAmiLine = l_Ami_Line[mAmiLayer];
    float x = mCc.mCcData.mOffset.x;
    float y = mCc.mCcData.mOffset.y;
    mCcOffset.set(x, y);
    float w = mCc.mCcData.mSize.x;
    float h = mCc.mCcData.mSize.y;
    mCcSize.set(w, h);
    if (player->mPowerup != POWERUP_MINI_MUSHROOM && player->mPowerup != POWERUP_NONE) {
        mCc.mCcData.mOffset.set(0.0f, 5.0f);
        mCc.mCcData.mSize.set(7.0f, 9.0f);
    }
    mCc.mCcData.mVsKind |= (1 << CC_KIND_KILLER);
    mCc.mCcData.mAttack = CC_ATTACK_SHELL;
    mRc.setRide(nullptr);
    if (mSleepTimer > smc_SLEEP_TIMER_SHAKE) {
        mCarryTimer = mSleepTimer;
    }
    mUseBaseIceBehaviour = 0;
    mWakeupShakeAngle = 0;
    mWakeupShakeAngle3D.x = 0;
    mWakeupShakeAngle3D.y = 0;
    mWakeupShakeAngle3D.z = 0;
    mFootAttr3 = false;
}

void daEnShell_c::finalizeState_Carry() {
    dAcPy_c *player = daPyMng_c::getPlayer(mCarriedBy);
    player->cancelCarry(this);
    mCc.mCcData.mOffset.set(mCcOffset.x, mCcOffset.y);
    mCc.mCcData.mSize.set(mCcSize.x, mCcSize.y);
    mCc.mCcData.mVsKind &= ~(1 << CC_KIND_KILLER);
    mCc.mCcData.mAttack = CC_ATTACK_NONE;
    mRc.setRide(nullptr);
    mBc.mFlags = 0;
    mCarryingFlags &= ~(CARRY_RELEASE | CARRY_THROW);
    mCarryTimer = 0;
    if (mPos.z >= 0.0f) {
        mAmiLayer = 0;
    } else {
        mAmiLayer = 1;
    }
}

void daEnShell_c::executeState_Carry() {
    dAcPy_c *player = daPyMng_c::getPlayer(mCarriedBy);
    if (mCarryingFlags & CARRY_RELEASE) {
        mDirection = mThrowDirection;
        mAngle.y = l_base_angleY[mDirection];
        if (checkWallAndBg()) {
            setDeathInfo_CarryBgIn(player);
        } else {
            if (!(mBc.mFlags & (0x15 << mDirection))) {
                mPos.x += l_EnMuki[mDirection] * 6.0f;
            }
            if (mCarryingFlags & CARRY_THROW) {
                setSlideThrowSpeed(player);
                changeState(StateID_Slide);
            } else {
                changeState(StateID_Sleep);
            }
        }
    } else if (checkSleep()) {
        mDirection = player->mDirection;
        if (checkWallAndBg()) {
            setDeathInfo_CarryBgIn(player);
        } else if (mIsFlipped) {
            changeState(StateID_WakeupReverse);
        } else {
            changeState(StateID_Wakeup);
        }
    } else {
        mAngle.y = player->mAngle.y;
        adjustCarryCc();
    }
}

void daEnShell_c::initializeState_Slide() {
    mSensorFootSlide.mBase.mFlags = 1;
    mSensorFootSlide.mLineA = -0x3000;
    mSensorFootSlide.mLineB = 0x2000;
    mSensorFootSlide.mDistanceFromCenter = 0;

    if (mPlayerNo >= 0 && mPlayerNo < PLAYER_COUNT) {
        dAcPy_c *player = daPyMng_c::getPlayer(mPlayerNo);

        mCc.mAmiLine = player->mCc.mAmiLine;
        mBc.mAmiLine = player->mCc.mAmiLine;

        mIsMugenCombo = checkMugenCombo(player);
        if (mIsMugenCombo) {
            mMugenComboSpeed = 0.0f;
            if (player->mPowerup == POWERUP_NONE) {
                mSpeed.x = l_EnMuki[mDirection] * GLOBAL_DATA.mSlideSpeedNoPowerup;
            } else {
                mSpeed.x = l_EnMuki[mDirection] * GLOBAL_DATA.mSlideSpeedPowerup;
            }
            mNoHitPlayer.mTimer[mPlayerNo] = 3;
            mFumiProc.refresh(new MugenComboFumiCheck_c());
        } else if (!player->mBc.isFoot()) {
            mNoHitPlayer.mTimer[mPlayerNo] = 18;
        } else {
            mNoHitPlayer.mTimer[mPlayerNo] = 10;
        }

        mSensorHead.mBase.mFlags |= 0xc0000;
        mSensorFootNormal.mBase.mFlags |= 0xc0000;
        mSensorWall.mBase.mFlags |= 0xc0000;
        mSensorFootSlide.mBase.mFlags |= 0xc0000;
    }

    mSlideAirAfterThrow = 0;
    if (*mStateMgr.getOldStateID() == StateID_Carry) {
        mSlideAirAfterThrow = 1;
    }

    mCc.mCcData.mVsKind |= (1 << CC_KIND_BALLOON) | (1 << CC_KIND_ITEM) | (1 << CC_KIND_KILLER);
    mCc.mCcData.mAttack = CC_ATTACK_SHELL;

    mUseBaseIceBehaviour = 0;
    mAccelF = 1 / 1024.0f;
    mAccelY = -0.1875f;
    mSpeedMax.set(mSpeed.x, -4.0f, 0.0f);

    mSensorHead.mBase.mFlags |= 0x900000;
    mSensorWall.mBase.mFlags |= 0x900000;
    if (mJumpPlayerCarryActorID) {
        mSensorHead.mBase.mFlags |= 0x400;
        mSensorWall.mBase.mFlags |= 0x400;
    }
    mBc.set(this, mSensorFootSlide, mSensorHead, mSensorWall);

    mBc.mOwningPlrNo = mPlayerNo;
    mWakeupShakeAngle = 0;
    mWakeupShakeAngle3D.x = 0;
    mWakeupShakeAngle3D.y = 0;
    mWakeupShakeAngle3D.z = 0;
    if (mIsFlipped) {
        mAngle.x = -0x8000;
    } else {
        mAngle.x = 0;
    }
}

void daEnShell_c::finalizeState_Slide() {
    if (mIsMugenCombo) {
        static const float speeds[] = { 0.375f, -0.375f };
        mSpeed.x = speeds[mDirection];
        mFumiProc.refresh(new NonUniqueFumiCheck_c());
    } else {
        clrComboCnt();
    }
    mIsMugenCombo = 0;
    mCc.mCcData.mVsKind &= ~((1 << CC_KIND_BALLOON) | (1 << CC_KIND_ITEM) | (1 << CC_KIND_KILLER));
    mCc.mCcData.mAttack = CC_ATTACK_NONE;
    mAccelF = 0.0f;
    mSpeedMax.set(0.0f, -4.0f, 0.0f);
    mSensorHead.mBase.mFlags &= ~0x49c0400;
    mSensorFootNormal.mBase.mFlags &= ~0xc0000;
    mSensorWall.mBase.mFlags &= ~0x9c0400;
    mSensorFootSlide.mBase.mFlags &= ~0xc0000;
    mBc.set(this, mSensorFootNormal, mSensorHead, mSensorWall);
    mBc.mOwningPlrNo = -1;
    mPlayerNo = -1;
}

void daEnShell_c::executeState_Slide() {
    mRc.mFlags &= ~0x8;
    calcSpeedX();
    calcSpeedY();
    posMove();
    slideSpin();
    slideEffect();
    if (mJumpPlayerNoCarryHitTimer == 0) {
        mSensorHead.mBase.mFlags &= ~0x400;
        mSensorWall.mBase.mFlags &= ~0x400;
    }
    EnBgCheckWall();
    EnBgCheckFoot();
    mBc.checkHead(mBc.mFlags);
    if (mIsMugenCombo) {
        float max = 14.0f;
        mMugenComboSpeed += std::fabs(mSpeed.x);
        if (std::fabs(mMugenComboSpeed) > max) {
            mAccelF = 0.15f;
            mNoHitPlayer.mTimer[mPlayerNo] = 16;
            mSpeedMax.x = l_slide_max_speed[mDirection];
            mIsMugenCombo = 0;
        } else {
            dAcPy_c *player = daPyMng_c::getPlayer(mPlayerNo);
            if (player->mNowBgCross1 & 1) {
                mAccelF = 0.15f;
                mSpeedMax.x = l_slide_max_speed[mDirection];
                mIsMugenCombo = 0;
                mNoHitPlayer.mTimer[mPlayerNo] = 16;
            }
        }
    }
    if (mBc.isFoot()) {
        mAccelY = -0.1875f;
        mSpeed.y = 0.0f;
        mSlideAirAfterThrow = 0;
        mCc.mpFriendActor = nullptr;
        if (mBc.getFootAttr() == 3) {
            mCc.release();
            mPos.y -= 0.25f;
        }
    } else {
        if (mSlideAirAfterThrow) {
            mVec3_c pos(mPos.x, mPos.y + 6.0f, mPos.z);
            float height;
            if (dBc_c::checkGround(&pos, &height, mLayer, 1, -1) && mPos.y < height) {
                mSlideAirAfterThrow = 0;
                mCc.mpFriendActor = nullptr;
                u32 unitKind = dBc_c::getUnitKind(mPos.x, mPos.y - 1.0f, mLayer);
                if (((unitKind >> 16) & 0xFF) != 8 && height <= pos.y) {
                    mPos.y = height;
                }
            }
        }
        if (mBc.isHead()) {
            mSpeed.y = -0.625f;
        }
    }

    if (mBc.mFlags & (0x15 << mDirection)) {
        mDirection ^= 1;
        mSpeed.x = -mSpeed.x;
        mSpeedMax.x = -mSpeedMax.x;
        mSpeed.y -= 0.8125f;
        dAudio::g_pSndObjEmy->startSound(SE_OBJ_KOURA, getCenterPos(), 0);
    }
}

void daEnShell_c::initializeState_Wakeup() {
    mDirection = getPl_LRflag(mPos);
    mIsFlipped = 0;
    mAngle.x = 0;
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mSpeedMax.set(0.0f, -4.0f, 0.0f);
}

void daEnShell_c::finalizeState_Wakeup() {}

void daEnShell_c::executeState_Wakeup() {
    calcSpeedY();
    posMove();
    u32 bgCheckRes = EnBgCheck();
    if (bgCheckRes & 1) {
        mSpeed.y = 0.0f;
    }
    if (bgCheckRes & 4) {
        mSpeed.x = 0.0f;
    }
    changeState(StateID_WakeupTurn);
}

void daEnShell_c::initializeState_WakeupTurn() {
    mDirection = getPl_LRflag(mPos);
}

void daEnShell_c::finalizeState_WakeupTurn() {}

void daEnShell_c::executeState_WakeupTurn() {
    calcSpeedY();
    posMove();
    u32 bgCheckRes = EnBgCheck();
    if (bgCheckRes & 1) {
        mSpeed.y = 0.0f;
    }
    if (bgCheckRes & 4) {
        mSpeed.x = 0.0f;
    }
    if (turnProc()) {
        setAfterSleepState();
    }
}

void daEnShell_c::initializeState_WakeupReverse() {
    mSpeed.set(0.0f, 3.0f, 0.0f);
}

void daEnShell_c::finalizeState_WakeupReverse() {}

void daEnShell_c::executeState_WakeupReverse() {
    calcSpeedY();
    posMove();
    mAngle.x += 0x400;
    u32 bgCheckRes = EnBgCheck();
    if (bgCheckRes & 1) {
        mSpeed.y = 0.0f;
    }
    if (bgCheckRes & 4) {
        mSpeed.x = 0.0f;
    }
    if (mBc.isFoot()) {
        changeState(StateID_Wakeup);
    }
}

void daEnShell_c::initializeState_DieFall() {
    if (isDieShell()) {
        mUseBaseIceBehaviour = 0;
    }
    mFootAttr3 = false;
    dEn_c::initializeState_DieFall();
}

void daEnShell_c::finalizeState_DieFall() {
    dEn_c::finalizeState_DieFall();
}

void daEnShell_c::executeState_DieFall() {
    const static s16 cs_spin_speed[] = { 0x400, -0x400 };
    s16 spinX, spinY;
    if (mDirection == mIceDeathDirection) {
        spinX = smc_DEADFALL_SPINSPEED;
        spinY = -cs_spin_speed[mIceDeathDirection];
    } else {
        spinX = -smc_DEADFALL_SPINSPEED;
        spinY = cs_spin_speed[mIceDeathDirection];
    }
    if (mInLiquid) {
        spinX *= smc_WATER_ROLL_DEC_RATE;
        spinY *= smc_WATER_ROLL_DEC_RATE;
    }
    mAngle.x += spinX;
    mAngle.y += spinY;
    calcSpeedY();
    posMove();
}
