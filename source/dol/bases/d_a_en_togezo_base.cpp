#include <game/bases/d_a_en_togezo_base.hpp>
#include <game/bases/d_a_player.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <constants/sound_list.h>

const sBcSensorPoint daEnTogezoBase_c::smc_toge_head = { SENSOR_IS_POINT, 0, 0xc000 };
const sBcSensorLine daEnTogezoBase_c::smc_toge_foot = { SENSOR_IS_LINE, -0x4000, 0x4000, 0x0 };
const sBcSensorLine daEnTogezoBase_c::smc_toge_wall = { SENSOR_IS_LINE, 0x7000, 0x5000, 0x8000 };

const sBcSensorLine daEnTogezoBase_c::smc_sakasa_toge_foot = { SENSOR_IS_LINE, -0x3000, 0x2000, 0x0 };

const sBcSensorPoint daEnTogezoBase_c::smc_pipo_head = { SENSOR_IS_POINT, 0, 0x6000 };
const sBcSensorLine daEnTogezoBase_c::smc_pipo_foot = { SENSOR_IS_LINE, -0x3000, 0x3000, -0x6000 };
const sBcSensorLine daEnTogezoBase_c::smc_pipo_wall = { SENSOR_IS_LINE, 0x2000, -0x2000, 0x6000 };
const sCcDatNewF daEnTogezoBase_c::smc_toge_cc = {
    0.0f, 6.0f,
    8.0f, 6.0f,
    CC_KIND_ENEMY,
    CC_ATTACK_NONE,
    BIT_FLAG(CC_KIND_PLAYER) | BIT_FLAG(CC_KIND_PLAYER_ATTACK) |
        BIT_FLAG(CC_KIND_YOSHI) | BIT_FLAG(CC_KIND_ENEMY) |
        BIT_FLAG(CC_KIND_TAMA),
    (u32) ~(BIT_FLAG(CC_ATTACK_NONE) | BIT_FLAG(CC_ATTACK_YOSHI_MOUTH) | BIT_FLAG(CC_ATTACK_SPIN_LIFT_UP) | BIT_FLAG(CC_ATTACK_SAND_PILLAR)),
    CC_STATUS_NONE,
    dEn_c::normal_collcheck
};

const float daEnTogezoBase_c::smc_WALK_SPEED_DAT[2] = { 0.5f, -0.5f };
const s16 daEnTogezoBase_c::smc_ROTATE_SPEED_DAT[2] = { 0x400, -0x400 };
const s16 daEnTogezoBase_c::smc_ANGLE_Y[2] = { 12000, -12000 };

STATE_DEFINE(daEnTogezoBase_c, Walk);
STATE_DEFINE(daEnTogezoBase_c, Pipo);
STATE_DEFINE(daEnTogezoBase_c, Change_Pipo);
STATE_DEFINE(daEnTogezoBase_c, Change_Togezo);
STATE_DEFINE(daEnTogezoBase_c, Turn);
STATE_VIRTUAL_DEFINE(daEnTogezoBase_c, DieFall);
STATE_VIRTUAL_DEFINE(daEnTogezoBase_c, Wakeup);
STATE_VIRTUAL_DEFINE(daEnTogezoBase_c, WakeupTurn);
STATE_VIRTUAL_DEFINE(daEnTogezoBase_c, WakeupReverse);

int daEnTogezoBase_c::create() {
    createModel();

    createShell("togezo", "g3d/togezo.brres", "togezo_shell", nullptr, 0.0f);

    mVisibleAreaOffset.set(0.0f, 8.0f);
    mVisibleAreaSize.set(16.0f, 16.0f);

    mActorProperties |= BIT_FLAG(9);

    mCenterOffs.set(0.0f, 6.0f, 0.0f);
    mSpeedMax.set(0.0f, -4.0f, 0.0f);
    mScale.set(1.0f, 1.0f, 1.0f);

    mAngle.y = smc_ANGLE_Y[mDirection];
    mFumiProc.mFumiCheck.mUnused = 8;
    m_894 = 2;

    initialize();

    return SUCCEEDED;
}

void daEnTogezoBase_c::createModel() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], 0, 0x20);

    mTogezoResFile = dResMng_c::m_instance->getRes("togezo", "g3d/togezo.brres");
    nw4r::g3d::ResMdl mdl = mTogezoResFile.GetResMdl("togezo");
    mTogezoModel.create(mdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC);

    dActor_c::setSoftLight_Enemy(mTogezoModel);

    nw4r::g3d::ResAnmChr anim = mTogezoResFile.GetResAnmChr("walk");
    mTogezoAnim.create(mdl, anim, &mAllocator);

    mPaipoResFile = dResMng_c::m_instance->getRes("paipo", "g3d/paipo.brres");
    nw4r::g3d::ResMdl paipoMdl = mPaipoResFile.GetResMdl("paipo");
    mPaipoMdl.create(paipoMdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC);

    dActor_c::setSoftLight_Enemy(mPaipoMdl);

    mAllocator.adjustFrmHeap();
}

int daEnTogezoBase_c::execute() {
    mStateMgr.executeState();
    if (HasamareBgCheck()) {
        setDeathInfo_Smoke(nullptr);
        return SUCCEEDED;
    }

    if (!mNoRespawn) {
        if (!isState(StateID_Ice) && checkRyusa()) {
            ryusaEffect();
        }
    }

    if (isCullCheckOk()) {
        cullCheck();
    }

    return SUCCEEDED;
}

int daEnTogezoBase_c::draw() {
    if (mShellMode == SHELL_MODE_PAIPO) {
        drawPaipo();
    } else if (mShellMode == SHELL_MODE_BASE) {
        drawShell();
    } else {
        drawTogezo();
    }

    return SUCCEEDED;
}

void daEnTogezoBase_c::calcMdl() {
    calcCatchPos();
    if (mShellMode == SHELL_MODE_PAIPO) {
        calcPaipoMdl();
    } else if (mShellMode == SHELL_MODE_BASE) {
        calcShellMdl();
    } else {
        calcTogezoMdl();
    }
}

void daEnTogezoBase_c::calcPaipoMdl() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    changePosAngle(&pos, &angle, 1);
    mMatrix.trans(pos.x, pos.y, pos.z);
    mMatrix.YrotM(angle.y);
    mMatrix.XrotM(angle.x);
    mMatrix.ZrotM(angle.z);

    mPaipoMdl.setLocalMtx(&mMatrix);
    mPaipoMdl.setScale(mBoyoMng.getScale());
    mPaipoMdl.calc(false);
}

void daEnTogezoBase_c::calcTogezoMdl() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    changePosAngle(&pos, &angle, 1);
    mMatrix.trans(pos.x, pos.y, pos.z);

    mMatrix.YrotM(angle.y);
    mMatrix.ZrotM(angle.z);

    mMatrix.concat(mMtx_c::createTrans(0.0f, 6.0f, 0.0f));
    mMatrix.XrotM(angle.x);
    mMatrix.concat(mMtx_c::createTrans(0.0f, -6.0f, 0.0f));

    mTogezoModel.setLocalMtx(&mMatrix);
    mTogezoModel.setScale(mBoyoMng.getScale());
    mTogezoModel.calc(false);
}

void daEnTogezoBase_c::drawTogezo() {
    mTogezoModel.entry();
}

void daEnTogezoBase_c::drawPaipo() {
    mPaipoMdl.entry();
}

void daEnTogezoBase_c::deleteReady() {}

int daEnTogezoBase_c::doDelete() {
    if (mAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        mTogezoAnim.remove();
        mTogezoModel.remove();
        mPaipoMdl.remove();
    }
    return SUCCEEDED;
}

void daEnTogezoBase_c::setAfterSleepState() {
    mSpeed.x = smc_WALK_SPEED_DAT[mDirection];
    changeState(StateID_Walk);
}

bool daEnTogezoBase_c::specialFumiProc(dActor_c *actor) {
    if (mIsFlipped) {
        return false;
    }
    setDamage(actor);
    return true;
}


bool daEnTogezoBase_c::specialFumiProc_Yoshi(dActor_c *actor) {
    return !mIsFlipped;
}

bool daEnTogezoBase_c::checkRyusa() {
    if (mBc.isFoot()) {
        mVec3_c center = getCenterPos();
        u32 unitType = dBc_c::getUnitType(center.x, center.y, mLayer);
        u32 unitKind = dBc_c::getUnitKind(center.x, center.y, mLayer);
        u8 v = unitKind >> 16;
        if (unitType & 0x8000 && v == 3) {
            return true;
        }
    }
    return false;
}

void daEnTogezoBase_c::ryusaEffect() {
    mVec3_c center = getCenterPos();
    mVec3_c efPos(center.x, center.y, 5500.0f);
    mLevelEffect.createEffect("Wm_en_quicksand", 0, &efPos, nullptr, nullptr);
}

bool daEnTogezoBase_c::setPlayerDamage(dActor_c *actor) {
    dAcPy_c *player = (dAcPy_c *) actor;
    if (player->setDamage(this, daPlBase_c::DAMAGE_DEFAULT)) {
        if (mShellMode != SHELL_MODE_BASE) {
            u8 dir = getTrgToSrcDir_Main(player->getCenterX(), getCenterX());
            if (mDirection != dir) {
                mDirection = dir;
                mAngle.y = smc_ANGLE_Y[mDirection];
                mSpeed.x = -mSpeed.x;
                mSpeedMax.x = -mSpeedMax.x;
            }
        }
        return true;
    }

    return false;
}

bool daEnTogezoBase_c::turnProc() {
    mAngle.y += smc_ROTATE_SPEED_DAT[mDirection];
    s16 target = smc_ANGLE_Y[mDirection];
    int absTarget = abs(target);
    int absAngle = abs(mAngle.y);
    if (absAngle >= absTarget) {
        mAngle.y = target;
        return true;
    }
    return false;
}

bool daEnTogezoBase_c::setEatSpitOut(dActor_c *eatingActor) {
    calcSpitOutPos(eatingActor);
    mDirection = eatingActor->mDirection;
    mPlayerNo = eatingActor->getPlrNo();
    setSlideThrowSpeed(eatingActor);
    mIsFlipped = true;
    reviveCc();
    mScale.set(1.0f, 1.0f, 1.0f);

    mSensorHead = smc_toge_head;
    mSensorFootNormal = smc_toge_foot;
    mSensorWall = smc_toge_wall;

    mCenterOffs.set(0.0f, 6.0f, 0.0f);
    changeState(StateID_Slide);
    return true;
}

bool daEnTogezoBase_c::createIceActor() {
    const static float cs_offset_tbl[] = {
        4.0f, 4.0f, 10.0f
    };

    const static mVec3_c cs_size_tbl[] = {
        mVec3_c(1.15f, 1.15f, 1.15f),
        mVec3_c(1.15f, 1.15f, 1.15f),
        mVec3_c(1.0f, 1.0f, 1.0f)
    };

    mVec3_c offset(mPos.x, mPos.y - cs_offset_tbl[mShellMode], mPos.z);

    dIceInfo iceInfo[] = {
        {
            0,
            offset,
            cs_size_tbl[mShellMode]
        }
    };
    return mIceMng.createIce(iceInfo, ARRAY_SIZE(iceInfo));
}

void daEnTogezoBase_c::pipoRolling() {
    int ang = std::fabs(mSpeed.x * 0.5f) * 4096.0f;
    if (ang > 0x400) {
        ang = 0x400;
    }

    s16 prevAng = m_88c;
    if (abs(m_88c) > ang) {
        m_88c += 0x20;
        ang = prevAng;
    } else {
        m_88c = 0;
    }

    mAngle.x += ang;
}

void daEnTogezoBase_c::pipoWallBound() {
    mDirection ^= 1;
    mAngle.y = smc_ANGLE_Y[mDirection];
    mSpeedMax.x = -mSpeedMax.x;
    mSpeed.x = -mSpeed.x;
}

void daEnTogezoBase_c::block_hit_init() {
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
    mCenterOffs.set(0.0f, 6.0f, 0.0f);
    if (isState(StateID_Sleep)) {
        mStateMgr.initializeState();
    } else {
        changeState(StateID_Sleep);
    }

    mSensorHead = smc_toge_head;
    mSensorWall = smc_toge_wall;

    mBc.set(this, smc_sakasa_toge_foot, mSensorHead, mSensorWall);
}

bool daEnTogezoBase_c::isBlockHitDeath() const {
    return mShellMode == SHELL_MODE_PAIPO && ACTOR_PARAM(DieOnBlockHit) == 1;
}

bool daEnTogezoBase_c::isDieShell() {
    return mShellMode != SHELL_MODE_PAIPO;
}

void daEnTogezoBase_c::landonEffect(float height) {
    mVec3_c landPos(mPos.x, mPos.y + height, 5500.0f);
    u32 unitType = dBc_c::getUnitType(landPos.x, landPos.y - 8.0f, mLayer);
    u32 unitKind = dBc_c::getUnitKind(landPos.x, landPos.y - 8.0f, mLayer);
    u32 v = (u8) (unitKind >> 16);
    u16 footAttr = mBc.getFootAttr();
    if (footAttr == 12 || unitType & 0x8000 && v == 3) {
        mEf::createEffect("Wm_en_sndlandsmk_s", 0, &landPos, nullptr, nullptr);
    } else {
        mEf::createEffect("Wm_en_landsmoke_s", 0, &landPos, nullptr, nullptr);
    }
}

void daEnTogezoBase_c::initializeState_DieFall() {
    daEnShell_c::initializeState_DieFall();
    if (mShellMode == SHELL_MODE_PAIPO) {
        mAngle.y = 0;
    }
}

void daEnTogezoBase_c::finalizeState_DieFall() {
    daEnShell_c::finalizeState_DieFall();
}

void daEnTogezoBase_c::executeState_DieFall() {
    const static s16 cs_spin_speed[] = { 0x200, -0x200 };

    if (mShellMode == SHELL_MODE_PAIPO) {
        s16 xSpeed = smc_DEADFALL_SPINSPEED;
        s16 ySpeed = cs_spin_speed[mIceDeathDirection];
        if (mInLiquid) {
            xSpeed *= smc_WATER_ROLL_DEC_RATE;
            ySpeed *= smc_WATER_ROLL_DEC_RATE;
        }
        mAngle.x += xSpeed;
        mAngle.y += ySpeed;
        calcSpeedY();
        posMove();
    } else {
        daEnShell_c::executeState_DieFall();
    }
}

void daEnTogezoBase_c::initializeState_Pipo() {
    mSpeed.x = -1.0f;
    mShellMode = SHELL_MODE_PAIPO;
    mCenterOffs.set(0.0f, 0.0f, 0.0f);
    mCc.mCcData.mBase.mOffset.y = 0.0f;
    mSpeedMax.y = -4.0f;
    mMaxFallSpeed = -4.0f;
    m_898 = nullptr;
}

void daEnTogezoBase_c::finalizeState_Pipo() {
    mAccelF = 0.0f;
    mAccelY = -0.1875f;
}

void daEnTogezoBase_c::executeState_Pipo() {
    calcSpeedX();
    calcFallSpeed();
    posMove();
    pipoRolling();
    EnBgCheckFoot();
    mBc.checkHead(mBc.mFlags);
    EnBgCheckWall();

    if (mSensorHead.mFlags & BIT_FLAG(31)) {
        if (mBc.mFlags != 0) {
            mBc.mFlags = 0;
        } else {
            mSensorHead.mFlags &= ~BIT_FLAG(31);
            mSensorFootNormal.mFlags &= ~BIT_FLAG(31);
            mSensorWall.mFlags &= ~BIT_FLAG(31);
            mBc.set(this, mSensorFootNormal, mSensorHead, mSensorWall);
        }
    }
    if (mBc.isHead()) {
        mSpeed.y = 0;
    }
    if (mBc.isWall(mDirection)) {
        pipoWallBound();
        if (!mBc.isFoot()) {
            mDirection ^= 1;
            mSpeed.x *= -0.5f;
        }
    }
    if (mBc.isFoot()) {
        Bound(0.1875f, 0.875f, 0.5f);
        if (mSpeed.y > 0.25f) {
            landonEffect(-8.0f);
            if (mBc.getFootAttr() == 3) {
                mSpeed.y = 0.0f;
            }
        }
        if (m_898 == 0) {
            m_898 = 1;
            dAudio::g_pSndObjEmy->startSound(SE_OBJ_PAIPO_LAND, mPos, 0);
            mAngle.y = smc_ANGLE_Y[mDirection];
            m_88a = getPl_LRflag(mPos);
        } else if (mSpeed.y == 0.0f && mTimer1 == 0) {
            mAngle.y = smc_ANGLE_Y[mDirection];
            changeState(StateID_Change_Togezo);
        }
    } else {
        mTimer1 = 12;
    }
}

void daEnTogezoBase_c::initializeState_Walk() {
    if (
        *mStateMgr.getOldStateID() != StateID_Turn &&
        *mStateMgr.getOldStateID() != daEnShell_c::StateID_WakeupTurn &&
        *mStateMgr.getOldStateID() != daEnShell_c::StateID_WakeupReverse
    ) {
        nw4r::g3d::ResAnmChr anm = mTogezoResFile.GetResAnmChr("walk");
        mTogezoAnim.setAnm(mTogezoModel, anm, m3d::FORWARD_LOOP);
        mTogezoModel.setAnm(mTogezoAnim, 2.0f);
        mTogezoAnim.setRate(1.0f);
    }
    mShellMode = SHELL_MODE_TOGEZO;
    mAccelY = -0.1875f;
    mAngle.y = smc_ANGLE_Y[mDirection];
    mSpeed.x = smc_WALK_SPEED_DAT[mDirection];
    mSpeed.z = 0.0f;
    mSpeedMax.y = -4.0f;
    m_898 = 0;
    m_894 = 2;
}

void daEnTogezoBase_c::finalizeState_Walk() {}

void daEnTogezoBase_c::executeState_Walk() {
    mTogezoModel.play();
    if (!mInLiquid) {
        mAccelY = -0.1875f;
        mSpeedMax.set(0.0f, -4.0f, 0.0f);
    }
    calcSpeedY();
    posMove();
    u32 prevFoot = mBc.isFoot();
    EnBgCheckFoot();
    mBc.checkHead(mBc.mFlags);
    EnBgCheckWall();

    if (mBc.isFoot()) {
        mSpeed.y = 0.0f;
        if (*mStateMgr.getOldStateID() == StateID_Change_Togezo) {
            if (!m_898) {
                landonEffect(0.0f);
            }
            if (mDirection == m_88a) {
                mSpeed.x = smc_WALK_SPEED_DAT[mDirection];
            } else {
                changeState(StateID_Turn);
                return;
            }
        }
        if (dAudio::isBgmAccentSign(1)) {
            mSpeed.y = 1.85f;
        }
        m_898 = 1;
    } else if (prevFoot && !mInLiquid && mSpeed.y <= 0.0f) {
        mFootPush2.x += m_1eb.x;
    }

    if (mBc.isWall(mDirection)) {
        changeState(StateID_Turn);
    }
}

void daEnTogezoBase_c::initializeState_Turn() {
    if (*mStateMgr.getOldStateID() == StateID_Wakeup) {
        nw4r::g3d::ResAnmChr anm = mTogezoResFile.GetResAnmChr("walk");
        mTogezoAnim.setAnm(mTogezoModel, anm, m3d::FORWARD_LOOP);
        mTogezoModel.setAnm(mTogezoAnim, 2.0f);
        mTogezoAnim.setRate(1.0f);
    }
    mSpeed.x = 0.0f;
    mSpeed.z = 0.0f;
    mDirection ^= 1;
}

void daEnTogezoBase_c::finalizeState_Turn() {}

void daEnTogezoBase_c::executeState_Turn() {
    mTogezoModel.play();
    if (!mInLiquid) {
        mAccelY = -0.1875f;
        mSpeedMax.set(0.0f, -4.0f, 0.0f);
    }
    calcSpeedY();
    posMove();
    EnBgCheckFoot();
    mBc.checkHead(mBc.mFlags);
    EnBgCheckWall();
    if (mBc.isFoot()) {
        if (dAudio::isBgmAccentSign(1)) {
            mSpeed.y = 1.85f;
        } else {
            mSpeed.y = 0.0f;
        }
    }
    if (turnProc()) {
        changeState(StateID_Walk);
    }
}

void daEnTogezoBase_c::initializeState_Change_Pipo() {
    m_884 = 0x8000;
    mSpeedMax.x = 0.0f;
}

void daEnTogezoBase_c::finalizeState_Change_Pipo() {
    if (mShellMode != SHELL_MODE_PAIPO) {
        mShellMode = SHELL_MODE_PAIPO;
        mAngle.y = smc_ANGLE_Y[mDirection];

        mSensorHead = smc_pipo_head;
        mSensorFootNormal = smc_pipo_foot;
        mSensorWall = smc_pipo_wall;

        mCc.mCcData.mBase.mSize.x = 6.0f;
    }
    mAccelF = 0.0f;
}

void daEnTogezoBase_c::executeState_Change_Pipo() {
    calcSpeedX();
    calcFallSpeed();
    posMove();
    EnBgCheckFoot();
    mBc.checkHead(mBc.mFlags);
    EnBgCheckWall();
    Bound(0.1875f, 1.0f, 0.5f);
    if (mBc.isHead()) {
        mSpeed.y = 0.0f;
    }
    if (mBc.isWall()) {
        pipoWallBound();
    }
    mAngle.x = (u16) mAngle.x.mAngle - 0x800; // [fake match]
    m_884 -= 0x800;
    if (m_884 == 0) {
        changeState(StateID_Pipo);
    }
}

void daEnTogezoBase_c::initializeState_Change_Togezo() {
    mIsFlipped = false;
    mSpeed.y = 2.0f;
    mAngle.x = -0x8000;
    m_884 = 0x8000;
    mSpeedMax.x = 0.0f;
    mMaxFallSpeed = -4.0f;
    mShellMode = SHELL_MODE_TOGEZO;
    dAudio::g_pSndObjEmy->startSound(SE_OBJ_PAIPO_TO_TOGEZO, mPos, 0);

    mSensorHead = smc_toge_head;
    mSensorFootNormal = smc_toge_foot;
    mSensorWall = smc_toge_wall;

    mBc.set(this, mSensorFootNormal, mSensorHead, mSensorWall);

    mCc.mCcData.mBase.mOffset.y = 6.0f;
    mCc.mCcData.mBase.mSize.x = 8.0f;
}

void daEnTogezoBase_c::finalizeState_Change_Togezo() {
    mCenterOffs.set(0.0f, 6.0f, 0.0f);
    mAccelF = 0.0f;
}

void daEnTogezoBase_c::executeState_Change_Togezo() {
    calcFallSpeed();
    posMove();
    EnBgCheckFoot();
    mBc.checkHead(mBc.mFlags);
    EnBgCheckWall();
    Bound(0.1875f, 1.0f, 0.5f);

    if (mBc.isHead()) {
        mSpeed.y = 0.0f;
    }
    if (mBc.isFoot()) {
        mSpeed.y = 0.0f;
    }
    if (mBc.isWall()) {
        pipoWallBound();
    }

    mAngle.x = (u16) mAngle.x.mAngle + 0xc00; // [fake match]
    m_884 -= 0xc00;
    if (m_884 > 0x8000) {
        changeState(StateID_Walk);
    }
}

void daEnTogezoBase_c::initializeState_Wakeup() {
    mShellMode = SHELL_MODE_TOGEZO;
    daEnShell_c::initializeState_Wakeup();
}

void daEnTogezoBase_c::finalizeState_Wakeup() {
    daEnShell_c::finalizeState_Wakeup();
}

void daEnTogezoBase_c::executeState_Wakeup() {
    mTogezoModel.play();
    daEnShell_c::executeState_Wakeup();
}

void daEnTogezoBase_c::initializeState_WakeupTurn() {
    nw4r::g3d::ResAnmChr anm = mTogezoResFile.GetResAnmChr("walk");
    mTogezoAnim.setAnm(mTogezoModel, anm, m3d::FORWARD_LOOP);
    mTogezoModel.setAnm(mTogezoAnim, 2.0f);
    mTogezoAnim.setRate(1.0f);
    daEnShell_c::initializeState_WakeupTurn();
}

void daEnTogezoBase_c::finalizeState_WakeupTurn() {
    daEnShell_c::finalizeState_WakeupTurn();
}

void daEnTogezoBase_c::executeState_WakeupTurn() {
    mTogezoModel.play();
    daEnShell_c::executeState_WakeupTurn();
}

void daEnTogezoBase_c::initializeState_WakeupReverse() {
    nw4r::g3d::ResAnmChr anm = mTogezoResFile.GetResAnmChr("walk");
    mTogezoAnim.setAnm(mTogezoModel, anm, m3d::FORWARD_LOOP);
    mTogezoModel.setAnm(mTogezoAnim, 2.0f);
    mTogezoAnim.setRate(1.0f);

    mShellMode = SHELL_MODE_TOGEZO;
    mSpeed.set(0.0f, 2.5f, 0.0);
    mDirection = getPl_LRflag(mPos);

    mSensorFootNormal = smc_toge_foot;
    mSensorHead = smc_toge_head;
    mSensorWall = smc_toge_wall;

    mBc.set(this, mSensorFootNormal, mSensorHead, mSensorWall);

    m_23b = 1;
}

void daEnTogezoBase_c::finalizeState_WakeupReverse() {}

void daEnTogezoBase_c::executeState_WakeupReverse() {
    mTogezoModel.play();
    calcSpeedY();
    posMove();
    EnBgCheckFoot();
    mBc.checkHead(mBc.mFlags);
    EnBgCheckWall();

    bool isFoot = false;
    if (mBc.isFoot()) {
        mSpeed.y = 0.0f;
        isFoot = true;
    }

    bool limitReached = false;
    if (mAngle.y > smc_ANGLE_Y[mDirection]) {
        mAngle.y -= 0x400;
        if (mAngle.y <= smc_ANGLE_Y[mDirection]) {
            mAngle.y = smc_ANGLE_Y[mDirection];
            limitReached = true;
        }
    } else if (mAngle.y < smc_ANGLE_Y[mDirection]) {
        mAngle.y += 0x400;
        if (mAngle.y >= smc_ANGLE_Y[mDirection]) {
            mAngle.y = smc_ANGLE_Y[mDirection];
            limitReached = true;
        }
    } else {
        limitReached = true;
    }

    switch (m_23b) {
        case 1: {
            s16 prevAngle = mAngle.x;
            mAngle.x += 0xc00;
            if (prevAngle < 0 && mAngle.x >= 0) {
                mAngle.x = 0;
                m_23b = 2;
            }
            break;
        }
        case 2:
            if (isFoot && limitReached) {
                mIsFlipped = false;
                changeState(StateID_Walk);
            }
            break;
    }
}
