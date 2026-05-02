#include <game/bases/d_a_en_shell.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_yoshi.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <game/bases/d_score_manager.hpp>
#include <game/bases/d_res_mng.hpp>
#include <constants/sound_list.h>

STATE_VIRTUAL_DEFINE(daEnShell_c, Sleep);
STATE_VIRTUAL_DEFINE(daEnShell_c, Carry);
STATE_VIRTUAL_DEFINE(daEnShell_c, Slide);
STATE_VIRTUAL_DEFINE(daEnShell_c, Wakeup);
STATE_VIRTUAL_DEFINE(daEnShell_c, WakeupReverse);
STATE_VIRTUAL_DEFINE(daEnShell_c, WakeupTurn);
STATE_VIRTUAL_DEFINE(daEnShell_c, DieFall);

static const float l_slide_throw_speed[] = { 2.5f, -2.5f };

const s16 daEnShell_c::msc_SPIN_SPEED = 0xc00;

daEnShell_c::daEnShell_c() : m_254(0), m_28c(BASE_ID_NULL), m_2a8(0), m_2ac(0.0f) {
    mFumiProc.refresh(new NonUniqueFumiCheck_c());
    mEatBehaviour = EAT_TYPE_EAT;
    mFlags = 3;
}

daEnShell_c::~daEnShell_c() {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && fManager_c::searchBaseByID(player->mCarryActorID) == this) {
            player->cancelCarry(this);
        }
    }
}

void daEnShell_c::createShell(const char *s1, const char *s2, const char *s3, const char *s4, float f) {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);

    mResFile = dResMng_c::m_instance->getRes(s1, s2);
    nw4r::g3d::ResMdl mdl = mResFile.GetResMdl(s3);
    mModel.create(mdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC | nw4r::g3d::ScnMdl::ANM_TEXPAT);

    dActor_c::setSoftLight_Enemy(mModel);

    nw4r::g3d::ResAnmChr anim;
    nw4r::g3d::ResAnmTexPat animTexPat;
    if (mProfName == fProfile::EN_NOKONOKO || mProfName == fProfile::EN_PATAPATA) {
        anim = mResFile.GetResAnmChr("revival_shell");
        mAnim.create(mdl, anim, &mAllocator);
    }

    if (s4 != nullptr) {
        mResAnmTexPat = mResFile.GetResAnmTexPat(s4);
        mAnimTex.create(mdl, mResAnmTexPat, &mAllocator);
        mAnimTex.setAnm(mModel, mResAnmTexPat, 0, m3d::FORWARD_ONCE);
        mModel.setAnm(mAnimTex);
        mAnimTex.setFrame(f, 0);
        mAnimTex.setRate(0.0f, 0);
    }

    mAllocator.adjustFrmHeap();
}

void daEnShell_c::calcShellMdl() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    if (isState(StateID_Carry)) {
        pos = calcCarryPos(m_268);
        mPos.x = pos.x;
        mPos.y = pos.y;
    } else if (m_274) {
        pos.z = 3248.0f;
    }
    changePosAngle(&pos, &angle, 1);
    float shiftY = 0.0f;
    if (m_29c < 96 && m_29c != 0 && mSpeed.y == 0.0f && m_2a0 != 0) {
        shiftY = 16.0f;
    }
    if (m_2a0) {
        if (angle.y >= 0) {
            angle.y = -angle.y.mAngle + 0x8000;
        } else {
            angle.y = -angle.y.mAngle - 0x8000;
        }
    }
    mMatrix.trans(pos.x, pos.y, pos.z);
    mMatrix.YrotM(l_EnMuki[mDirection] * 16384);

    mMatrix.concat(mMtx_c::createTrans(0.0f, 7.0f, 0.0f));
    mMatrix.XrotM(angle.x);
    mMatrix.concat(mMtx_c::createTrans(0.0f, -7.0f, 0.0f));

    mMatrix.concat(mMtx_c::createTrans(0.0f, shiftY, 0.0f));
    mMatrix.XrotM(m_296.x);
    mMatrix.ZrotM(m_296.z);
    mMatrix.concat(mMtx_c::createTrans(0.0f, -shiftY, 0.0f));

    mMatrix.YrotM(l_EnMuki[mDirection ^ 1] * 16384);
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
            if (m_258 == 0) {
                pos = getCenterPos();
            }
            WaterCheck(pos, 1.0f);
            if (m_274 && mBc.isFoot()) {
                m_274 = 0;
            }
        }

        if (m_290 > 0) {
            m_290--;
            if (m_290 == 0) {
                dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_28c);
                if (actor != nullptr) {
                    actor->mCc.mFriendActor = nullptr;
                }
                m_28c = BASE_ID_NULL;
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
    if (m_28c == other->mpOwner->mUniqueID) {
        return false;
    }
    return dEn_c::EtcDamageCheck(self, other);
}

void daEnShell_c::Normal_VsEnHitCheck(dCc_c *self, dCc_c *other) {
    dActor_c *otherActor = other->getOwner();
    if (
        isState(StateID_Carry) &&
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
        mDirection == 1 && collOffsetX > 0.0f ||
        mDirection == 0 && collOffsetX < 0.0f
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
            m_268.set(0.0f, -5.0f, 6.0f);
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
                actor->mCc.mFriendActor = this;
                m_28c = actor->mUniqueID;
                m_290 = 12;
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
                if (m_2a8) {
                    mPos.x = m_2b0;
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
                actor->mCc.mFriendActor = this;
                m_28c = actor->mUniqueID;
                m_290 = 12;
            }
        }
        setKickSlide(self, player);
        changeState(StateID_Slide);
    } else if (!isState(daEnCarry_c::StateID_Carry) && !player->isNoDamage()) {
        if (isState(StateID_Slide) && fn_800397b0(player)) {
            if (m_2a8 == 0) {
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
    } else if (isState(StateID_Sleep) || m_254) {
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
        if (isState(StateID_Slide) && fn_800397b0(yoshi)) {
            if (m_2a8 == 0) {
                setKickSlide(self, yoshi);
            }
        } else {
            dEn_c::Normal_VsYoshiHitCheck(self, other);
        }
    }
}

bool daEnShell_c::checkComboClap(int max) {
    if (m_2a8) {
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
    if (m_2a8 && std::fabs(speedF) < 1.0f) {
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
        &StateID_DieFall,
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
    if (m_2a8 && actor->mSpeed.y > 0.0f) {
        return;
    }
    dEn_c::FumiScoreSet(actor);
}

void daEnShell_c::setCarryFall(dActor_c *carryingActor, int collisionDelay) {
    int plrNo = *carryingActor->getPlrNo();
    if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
        mNoHitPlayer.mTimer[plrNo] = collisionDelay;
        m_274 = 1;
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

bool daEnShell_c::fn_800397b0(dActor_c *actor) {
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

    if (!ACTOR_PARAM(MugenRelated)) {
        return false;
    }
    if (player->mNowBgCross1 & 1) {
        return false;
    }
    if (player->mSpeed.y > 0.0f) {
        return false;
    }

    mVec3_c pos = player->mPos;
    pos.x += l_EnMuki[mDirection] * 1.5f;

    float height = 0.0f;
    if (dBc_c::checkGround(&pos, &height, player->mLayer, 1, -1) && player->mPos.y < height + 16.0f) {
        return false;
    }

    mVec3_c centerPos = getCenterPos();

    mVec3_c tmp(centerPos.x + l_EnMuki[mDirection ^ 1] * 16.0f, centerPos.y - 16.0f, centerPos.z);
    if (dBc_c::checkBg(tmp.x, tmp.y, mLayer, 3, 0x819)) {
        return false;
    }

    mVec3_c tmp2(centerPos.x + l_EnMuki[mDirection] * 16.0f, centerPos.y, centerPos.z);
    float wallX = 0.0f;
    if (!dBc_c::checkWall(&centerPos, &tmp2, &wallX, mLayer, 1, nullptr)) {
        return false;
    }

    float diff = std::fabs(wallX - centerPos.x);
    float f = 8.0f;
    if (player->mTreadCount < 2) {
        f = 10.0f;
    }
    if (diff < f || diff > 16.0f) {
        return false;
    }

    if (mDirection == DIR_LR_R) {
        m_2b0 = wallX - 14.0f;
        mPos.x = wallX - 15.0f;
    } else {
        m_2b0 = wallX + 14.0f;
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
        &StateID_DieFall,
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
    float xEnd = bgParam->xEnd();
    float yEnd = bgParam->yEnd();

    float posX1 = mPos.x + mVisibleAreaSize.x * 0.5f;
    float posX2 = mPos.x - mVisibleAreaSize.x * 0.5f;

    float posY = mPos.y;
    float posY1 = mPos.y + mVisibleAreaSize.y;

    if (posX1 < xStart - 160.0f) {
        return true;
    }
    if (posX2 > xEnd + 160.0f) {
        return true;
    }
    if (posY > yStart + 256.0f) {
        return true;
    }

    float offs = 256.0f;
    if (mBc.mFlags & 0x8000) {
        const dRide_ctr_c *ride = mRc.getRide();
        if (ride != nullptr && ride->mOwner != nullptr && ride->mOwner->mProfName == fProfile::AC_FLOOR_GYRATION) {
            offs = 64.0f;
        }
    }
    if (posY1 < yEnd - offs) {
        return true;
    }

    return false;
}

bool daEnShell_c::ActorDrawCullCheck() {
    if (isState(StateID_Carry) || isState(StateID_Slide)) {
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
    m_2a0 = 1;
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
    mCc.mFriendActor = nullptr;
    if (actor->mKind == STAGE_ACTOR_PLAYER) {
        dAcPy_c *player = (dAcPy_c *) actor;
        if (player->m_1308 == BASE_ID_NULL) {
            mCc.mFriendActor = player;
        }
    } else if (actor->mKind == STAGE_ACTOR_YOSHI) {
        daYoshi_c *yoshi = (daYoshi_c *) actor;
        if (yoshi->isStar()) {
            mCc.mFriendActor = yoshi;
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
    s16 angle = tmp * getSpinSpeed();
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
        &StateID_DieFall,
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
    if (m_258 == 0) {
        changeState(StateID_Ice);
        mPlayerNo = -1;
    } else {
        dEn_c::hitCallback_Ice(self, other);
    }
    return true;
}

void daEnShell_c::returnState_Ice() {
    if (m_258 == 0) {
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
    if (m_29e > 0) {
        m_29e--;
        return false;
    }

    if (isFunsui()) {
        return false;
    }

    m_29c--;
    if (m_29c < 96) {
        m_294 += 0x1d00;
        float sin = m_294.sin();
        float cos = m_294.cos();
        m_296.x = sin * 2560.0f;
        m_296.z = cos * 2560.0f;
        if (m_29c == 0) {
            m_296.x = 0;
            m_296.z = 0;
            return true;
        }
    }
    return false;
}
