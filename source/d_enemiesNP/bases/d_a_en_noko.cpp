#include <game/bases/d_a_en_noko.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_a_tag_wind.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_audio.hpp>

const sBcSensorPoint l_noko_head = { SENSOR_IS_POINT, 0, 0xc000 };
const sBcSensorLine l_noko_foot = { SENSOR_IS_LINE, -0x4000, 0x4000, 0x0 };
const sBcSensorLine l_noko_wall = { SENSOR_IS_LINE, 0x6000, 0x9000, 0x6000 };
const sCcDatNewF l_noko_cc = {
    0.0f, 8.0f,
    9.0f, 8.0f,
    CC_KIND_ENEMY,
    CC_ATTACK_NONE,
    BIT_FLAG(CC_KIND_PLAYER) | BIT_FLAG(CC_KIND_PLAYER_ATTACK) | BIT_FLAG(CC_KIND_YOSHI) |
        BIT_FLAG(CC_KIND_ENEMY) | BIT_FLAG(CC_KIND_TAMA),
    (u32) ~BIT_FLAG(CC_ATTACK_NONE) & ~BIT_FLAG(CC_ATTACK_YOSHI_MOUTH) & ~BIT_FLAG(CC_ATTACK_SPIN_LIFT_UP) & ~BIT_FLAG(CC_ATTACK_SAND_PILLAR),
    CC_STATUS_NONE,
    dEn_c::normal_collcheck
};

const float l_walk_speed[2] = {0.5f, -0.5f};
const s16 l_turn_speed[2] = {ANGLE_360_DIV(32), -ANGLE_360_DIV(32)};
const s16 l_turn_target_angle[2] = {DEG_TO_ANGLE(90), -DEG_TO_ANGLE(90)};

STATE_DEFINE(daEnNoko_c, BlockAppear);
STATE_DEFINE(daEnNoko_c, Walk);
STATE_DEFINE(daEnNoko_c, Turn);
STATE_DEFINE(daEnNoko_c, WindTurn);
STATE_DEFINE(daEnNoko_c, SpitOut_Ready);
STATE_DEFINE(daEnNoko_c, BgmDance);
STATE_DEFINE(daEnNoko_c, BgmDanceEd);
STATE_VIRTUAL_DEFINE(daEnNoko_c, Wakeup);
STATE_VIRTUAL_DEFINE(daEnNoko_c, WakeupTurn);

ACTOR_PROFILE(EN_NOKONOKO, daEnNoko_c, 0x12);

int daEnNoko_c::create() {
    mNokoType = ACTOR_PARAM(NokoType);

    loadRes();
    createShell("nokonokoA", "g3d/nokonokoA.brres", "nokonoko_shell", "Tnokonoko_shell", mNokoType);
    mScale.set(1.0f, 1.0f, 1.0f);
    mCenterOffs.set(0.0f, 8.0f, 0.0);
    mVisibleAreaOffset.set(0.0f, 16.0f);
    mVisibleAreaSize.set(28.0f, 32.0f);
    mNokoModel.setCallback(&mMdlCallback);
    clrComboCnt();

    u8 dir = getPl_LRflag(mPos);
    mDirection = dir;
    mAmiLayer = ACTOR_PARAM(Layer);
    mAngle.y = l_turn_target_angle[dir];
    mCreatePos = mPos;
    mFlags |= dEn_c::EN_FLAG_16;
    mPos.z = l_Ami_Zpos[ACTOR_PARAM(Layer)];

    mNokoAnimTex.setPlayMode(m3d::FORWARD_ONCE, 0);
    mNokoModel.setAnm(mNokoAnimTex, 0.0f);
    mNokoAnimTex.setFrame(mNokoType, 0);
    mNokoAnimTex.setRate(0.0f, 0);

    mSensorHead = l_noko_head;
    mSensorWall = l_noko_wall;
    mSensorFootNormal = l_noko_foot;

    mCcData.mBase.mOffset = l_noko_cc.mBase.mOffset;
    mCcData.mBase.mSize = l_noko_cc.mBase.mSize;
    mCcData.mKind = l_noko_cc.mKind;
    mCcData.mAttack = l_noko_cc.mAttack;
    mCcData.mVsKind = l_noko_cc.mVsKind;
    mCcData.mVsDamage = l_noko_cc.mVsDamage;
    mCcData.mStatus = l_noko_cc.mStatus;
    mCcData.mCallback = l_noko_cc.mCallback;
    mCc.set(this, &mCcData, l_Ami_Line[mAmiLayer]);
    mCc.entry();

    mBaseZPos = dActorMng_c::m_instance->mGoombaZOrderThing * 16.0f;
    dActorMng_c::m_instance->mGoombaZOrderThing = (dActorMng_c::m_instance->mGoombaZOrderThing + 1) & 0xF;

    setInitialState();

    mBc.set(this, mSensorFootNormal, mSensorHead, mSensorWall);
    mBc.m_4c = mPos.x;

    return 1;
}

void daEnNoko_c::loadRes() {
    mNokoAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);

    mNokoResFile = dResMng_c::m_instance->getRes("nokonokoA", "g3d/nokonokoA.brres");

    nw4r::g3d::ResMdl mdl = mNokoResFile.GetResMdl("nokonokoA");
    mNokoModel.create(mdl, &mNokoAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC | nw4r::g3d::ScnMdl::BUFFER_RESTEXOBJ | nw4r::g3d::ScnMdl::BUFFER_RESTLUTOBJ, 1, nullptr);
    dActor_c::setSoftLight_Enemy(mNokoModel);

    nw4r::g3d::ResAnmChr anm = mNokoResFile.GetResAnmChr("walkA");
    mMoveAnim.create(mdl, anm, &mNokoAllocator, nullptr);
    mNokoResAnmTexPat = mNokoResFile.GetResAnmTexPat("nokonokoA");
    mNokoAnimTex.create(mdl, mNokoResAnmTexPat, &mNokoAllocator, nullptr, 1);

    vf324();
    mNokoAllocator.adjustFrmHeap();

    mNokoAnimTex.setAnm(mNokoModel, mNokoResAnmTexPat, 0, m3d::FORWARD_ONCE);
}

void daEnNoko_c::setInitialState() {
    mDirection = getPl_LRflag(mPos);
    if (ACTOR_PARAM(SpawnMode) == 1) {
        mSpeed.y = 0.0f;
        changeState(daEnShell_c::StateID_Sleep);
    } else if (ACTOR_PARAM(BlockAppear)) {
        changeState(daEnNoko_c::StateID_BlockAppear);
    } else if (ACTOR_PARAM(SpitOut)) {
        changeState(daEnNoko_c::StateID_SpitOut_Ready);
    } else {
        changeState(daEnNoko_c::StateID_Walk);
    }
}

int daEnNoko_c::preExecute() {
    if (dEn_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    if (canDance()) {
        if (dAudio::isBgmAccentSign(2)) {
            danceWithMove(0);
        } else if (dAudio::isBgmAccentSign(4)) {
            danceWithMove(1);
        } else if (dAudio::isBgmAccentSign(8)) {
            danceWithMove(2);
        }
    }

    return SUCCEEDED;
}

int daEnNoko_c::execute() {
    mStateMgr.executeState();

    if (cullCheck()) {
        return SUCCEEDED;
    }

    if (isState(StateID_BgmDance)) {
        mBgmDanceAngle = 0;
    } else {
        dance();
    }

    if (!mNoRespawn && !isState(StateID_Ice)) {
        updateAmiLine();
        setZPos();
        if (isInQuicksand()) {
            spawnQuicksandEffects();
        }
    }

    return SUCCEEDED;
}

int daEnNoko_c::draw() {
    if (mShellMode == SHELL_MODE_BASE) {
        return drawShell();
    }

    if (isState(daEnShell_c::StateID_Wakeup)) {
        drawShell();
    }

    mNokoModel.entry();
    return SUCCEEDED;
}

int daEnNoko_c::doDelete() {
    if (mNokoAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        mMoveAnim.remove();
        mNokoModel.remove();
        mNokoAnimTex.remove();
        deleteResExtra();
    }

    return SUCCEEDED;
}

bool daEnNoko_c::isInQuicksand() {
    if (mBc.isFoot()) {
        mVec3_c pos = getCenterPos();
        u32 type = dBc_c::getUnitType(pos.x, pos.y, mLayer);
        u8 subkind = dBc_c::getUnitKind(pos.x, pos.y, mLayer) >> 16;

        if (type & BIT_FLAG(15) && subkind == 3) {
            return true;
        }
    }

    return false;
}

void daEnNoko_c::spawnQuicksandEffects() {
    mVec3_c center = getCenterPos();
    mVec3_c efPos(center.x, center.y, 5500.0f);
    mQuickSandEffect.createEffect("Wm_en_quicksand", 0, &efPos, nullptr, nullptr);
}

bool daEnNoko_c::canDance() {
    if (
        isState(StateID_Walk) ||
        isState(StateID_Turn) ||
        isState(StateID_WindTurn) ||
        isState(StateID_BgmDance) ||
        isState(StateID_BgmDanceEd)
    ) {
        return true;
    }

    return false;
}

void daEnNoko_c::danceWithMove(int move) {
    mDanceMove = move;
    changeState(StateID_BgmDance);
}

void daEnNoko_c::calcMdl() {
    calcShellMdl();
    mVec3_c pos = getPos();
    mAng3_c angle = mAngle;
    dActor_c::changePosAngle(&pos, &angle, 1);

    mMatrix.trans(pos);
    mMatrix.ZXYrotM(angle);

    mNokoModel.setLocalMtx(&mMatrix);

    mNokoModel.setScale(mBoyoMng.getScale());
    mNokoModel.calc(false);
}

void daEnNoko_c::calcShellEffectPos() {
    m3d::mdl_c &model = mModel;
    nw4r::g3d::ResMdl resMdl = model.getResMdl();
    nw4r::g3d::ResNode resNode = resMdl.GetResNode("nokonoko_shell_model");

    mMtx_c matrix;

    model.getNodeWorldMtx(resNode.GetID(), &matrix);
    matrix.multVecZero(m_71c);
    if (mIsFlipped) {
        m_71c.y -= 16.0f;
    }
}

bool daEnNoko_c::setPlayerDamage(dActor_c *actor) {
    daPlBase_c *player = (daPlBase_c *)actor;

    if (player->setDamage(this, daPlBase_c::DAMAGE_DEFAULT) && isWalking()) {
        playerDamageTurn(actor);
        return true;
    }

    return false;
}

bool daEnNoko_c::playerDamageTurn(dActor_c *actor) {
    u8 dir = dActor_c::getTrgToSrcDir_Main(actor->getCenterX(), getCenterX());

    u8 mode = mShellMode;
    if (mDirection == dir || mode == SHELL_MODE_BASE) {
        return false;
    }

    mDirection = dir;
    mAngle.y = l_turn_target_angle[dir];
    mBgmDanceAngle *= -1;
    turnAround();

    return true;
}

void daEnNoko_c::setNokoBc() {
    mSensorHead = l_noko_head;
    mSensorWall = l_noko_wall;
    mSensorFootNormal = l_noko_foot;
    mBc.set(this, mSensorFootNormal, mSensorHead, mSensorWall);
}

void daEnNoko_c::turnAround() {
    if (isState(StateID_Turn)) {
        changeState(StateID_Walk);
    } else {
        mSpeed.x = -mSpeed.x;
    }
}

bool daEnNoko_c::turnProc() {
    int turnDir;
    s16 turnSpeed;
    doTurn(&turnDir, &turnSpeed);
    mAngle.y += turnSpeed;

    mAng target = l_turn_target_angle[turnDir];
    if (mAngle.y.abs() >= target.abs()) {
        mAngle.y = target;
        return true;
    }

    return false;
}

void daEnNoko_c::doTurn(int *dir, s16 *turnSpeed) {
    *dir = mDirection;
    *turnSpeed = l_turn_speed[*dir];
}

void daEnNoko_c::setZPos() {
    if (mLayer == 0) {
        mPos.z = mBaseZPos + l_Ami_Zpos[mAmiLayer];
    } else {
        mPos.z = -2500.0f;
    }
}

void daEnNoko_c::updateAmiLine() {
    float width = mCc.mCcData.mBase.mSize.x;
    float offX = mCc.mCcData.mBase.mOffset.x;
    float offY = mCc.mCcData.mBase.mOffset.y;

    float x = mPos.x + offX;
    float y = mPos.y + offY;

    u32 typeRight = dBc_c::getUnitType(x + width, y, mLayer);
    u8 kindRight = dBc_c::getUnitKind(x + width, y, mLayer) & 0xFF;
    u32 typeLeft = dBc_c::getUnitType(x - width, y, mLayer);
    u8 kindLeft = dBc_c::getUnitKind(x - width, y, mLayer) & 0xFF;

    if (
        typeRight & BIT_FLAG(10) && kindRight >= 2 ||
        typeLeft & BIT_FLAG(10) && kindLeft >= 2
    ) {
        if (!mAmiLayer) {
            mCc.mAmiLine = 1;
        } else {
            mCc.mAmiLine = 2;
        }
    } else {
        mCc.mAmiLine = 3;
    }
}

void daEnNoko_c::dance() {
    static const s16 sc_danceAngle[2] = {-DEG_TO_ANGLE(30), DEG_TO_ANGLE(30)};
    sLib::chaseAngle(&mBgmDanceAngle, sc_danceAngle[mDirection], 0x400);
}

bool daEnNoko_c::createIceActor() {
    static const int sc_iceMode[2] = {0, 1};
    static const float cs_iceOffsetY[2] = {-2.0f, -4.0f};

    int iceMode;
    mVec3_c icePos;
    mVec3_c iceSize;
    if (*mStateMgr.getMainStateID() == StateID_BgmDance) {
        icePos.set(mPos.x, mPos.y - 4.0f, mPos.z + 2.0f);
        iceMode = 1;
        iceSize.set(1.05f, 1.0f, 1.75f);
    } else {
        iceMode = sc_iceMode[mShellMode];
        icePos.set(mPos.x, mPos.y + cs_iceOffsetY[mShellMode], mPos.z);
        iceSize.set(1.05f, 1.0f, 1.05f);
    }

    dIceInfo iceInfo[] = {
        {
            iceMode,
            icePos,
            iceSize
        }
    };

    return mIceMng.createIce(iceInfo, ARRAY_SIZE(iceInfo));
}

bool daEnNoko_c::checkSleep() {
    if (ACTOR_PARAM(SpawnMode) == 1) {
        return false;
    }

    return daEnShell_c::checkSleep();
}

void daEnNoko_c::beginFunsui() {
    if (isState(StateID_Carry) || isState(StateID_Ice)) {
        return;
    }

    mIsFrozen = 1;
    mXSpeedBeforeFrozen = mSpeed.x;
    mSpeed.set(0.0f, 0.0f, 0.0f);

    if (mShellMode == SHELL_MODE_BASE) {
        changeState(StateID_Sleep);
    } else {
        mMoveAnim.setRate(2.0f);
    }
}

void daEnNoko_c::endFunsui() {
    mIsFrozen = 0;

    if (mShellMode != SHELL_MODE_BASE) {
        mMoveAnim.setRate(1.0f);
        mSpeed.x = mXSpeedBeforeFrozen;
    }
}

float daEnNoko_c::getWindMultiplier() {
    daTagWind_c *tagWind = (daTagWind_c *) fManager_c::searchBaseByProfName(fProfile::TAG_WIND, nullptr);

    if (tagWind == nullptr) {
        return 0.0f;
    }

    return tagWind->m_394;
}

void daEnNoko_c::setMoveAnimation(const char *name, m3d::playMode_e mode, float frame) {
    mMoveAnim.setAnm(mNokoModel, mNokoResFile.GetResAnmChr(name), mode);
    mNokoModel.setAnm(mMoveAnim, frame);
}

void daEnNoko_c::setBaseAnimation(const char *name, m3d::playMode_e mode, float frame) {
    mAnim.setAnm(mModel, mResFile.GetResAnmChr(name), mode);
    mModel.setAnm(mAnim, frame);
}

bool daEnNoko_c::checkLedge() {
    static const float sc_offsetX[2] = {2.5f, -2.5f};

    mVec3_c pos(mPos.x + sc_offsetX[mDirection], mPos.y + 4.0f, mPos.z);
    u8 kind = dBc_c::getUnitKind(pos.x, mPos.y - 2.0f, mLayer) >> 16;

    if (kind == 8) {
        return false;
    }

    float height = 0.0f;
    if (dBc_c::checkGround(&pos, &height, mLayer, 1, -1) && height < pos.y && height > mPos.y - 5.0f) {
        return true;
    }

    return false;
}

void daEnNoko_c::landEffect() {
    mVec3_c pos = mVec3_c(mPos.x, mPos.y, 5500.0f);

    if (mBc.getFootAttr() == 12) {
        mEf::createEffect("Wm_en_sndlandsmk_s", 0, &pos, nullptr, nullptr);
    } else {
        mEf::createEffect("Wm_en_landsmoke_s", 0, &pos, nullptr, nullptr);
    }
}

void daEnNoko_c::initializeState_Walk() {
    if (
        *mStateMgr.getOldStateID() != StateID_Turn &&
        *mStateMgr.getOldStateID() != StateID_Walk &&
        *mStateMgr.getOldStateID() != StateID_WindTurn &&
        *mStateMgr.getOldStateID() != daEnShell_c::StateID_WakeupTurn
    ) {
        setMoveAnimation("walkA", m3d::FORWARD_LOOP, 1.0f);
    }

    mShellMode = SHELL_MODE_NOKO_WALK;
    mAccelY = -0.1875f;
    mAccelF = 0.05f;
    mSpeedMax.set(l_walk_speed[mDirection], -4.0f, 0.0f);
    mSensorFootNormal = l_noko_foot;
    mSpeed.x = l_walk_speed[mDirection];
    m_8c8 = 0;
}

void daEnNoko_c::finalizeState_Walk() {}

void daEnNoko_c::executeState_Walk() {
    mNokoModel.play();

    float wind = getWindMultiplier();
    if (std::fabs(wind) > 0.475f) {
        if (l_EnMuki[mDirection] * wind >= 0.0f) {
            changeState(StateID_WindTurn);
            return;
        } else {
            mSpeedMax.x = 0.0f;
        }
    } else {
        mSpeedMax.x = l_walk_speed[mDirection];
    }

    calcSpeedX();
    calcSpeedY();
    posMove();

    u32 prevFoot = mBc.isFoot();
    EnBgCheck();
    if (mBc.isFoot()) {
        mSpeed.y = 0.0f;
        if (mNokoType == NOKO_RED && !checkLedge()) {
            changeState(StateID_Turn);
            return;
        }
    } else if (prevFoot && !mInLiquid && (mSpeed.y <= 0.0f)) {
        mFootPush2.x += m_1eb.x;
    }

    if (mBc.isWall(mDirection)) {
        changeState(StateID_Turn);
    }
}

void daEnNoko_c::initializeState_Wakeup() {
    setMoveAnimation("revival", m3d::FORWARD_ONCE, 0.0f);
    setBaseAnimation("revival_shell", m3d::FORWARD_ONCE, 0.0f);
    mShellMode = SHELL_MODE_NOKO_WALK;

    daEnShell_c::initializeState_Wakeup();
}

void daEnNoko_c::finalizeState_Wakeup() {
    mAnim.setFrame(0.0f);
    mCc.mCcData.mBase.mOffset.x = 0.0f;
    mCc.mCcData.mBase.mSize.x = 8.0f;
}

void daEnNoko_c::executeState_Wakeup() {
    mNokoModel.play();
    mModel.play();
    calcSpeedY();
    posMove();

    u32 bgCheck = EnBgCheck();
    if (bgCheck & 1) {
        mSpeed.y = 0.0f;
    }
    if (bgCheck & 4) {
        mSpeed.x = 0.0f;
    }

    if (mMoveAnim.isStop()) {
        changeState(daEnShell_c::StateID_WakeupTurn);
    }
}

void daEnNoko_c::initializeState_WakeupTurn() {
    setMoveAnimation("walkA", m3d::FORWARD_LOOP, 1.0f);
    daEnShell_c::initializeState_WakeupTurn();
}

void daEnNoko_c::finalizeState_WakeupTurn() {
    daEnShell_c::finalizeState_WakeupTurn();
}

void daEnNoko_c::executeState_WakeupTurn() {
    mNokoModel.play();
    daEnShell_c::executeState_WakeupTurn();
}

void daEnNoko_c::initializeState_Turn() {
    mDirection ^= 1;
    mSpeed.x = 0.0f;
    mSpeedMax.set(0.0f, -4.0f, 0.0f);
}

void daEnNoko_c::finalizeState_Turn() {}

void daEnNoko_c::executeState_Turn() {
    mNokoModel.play();
    calcSpeedY();
    posMove();

    if (EnBgCheck() & 1) {
        mSpeed.y = 0.0f;
    }

    WaterCheck(mPos, 1.0f);

    if (turnProc()) {
        changeState(StateID_Walk);
    }
}

void daEnNoko_c::initializeState_WindTurn() {
    mDirection ^= 1;
    mSpeed.x = 0.0f;
    mSpeedMax.set(0.0f, -4.0f, 0.0f);
}

void daEnNoko_c::finalizeState_WindTurn() {}

void daEnNoko_c::executeState_WindTurn() {
    mNokoModel.play();
    calcSpeedY();
    posMove();

    if (EnBgCheck() & 1) {
        mSpeed.y = 0.0f;
    }

    turnProc();

    if (std::fabs(getWindMultiplier()) < 0.25f) {
        changeState(StateID_Turn);
    }
}

void daEnNoko_c::initializeState_BlockAppear() {
    setMoveAnimation("walkA", m3d::FORWARD_LOOP, 1.0f);
    mTimer1 = 40;
    mCc.release();
}

void daEnNoko_c::finalizeState_BlockAppear() {
    mCc.entry();
}

void daEnNoko_c::executeState_BlockAppear() {
    if (mTimer1 == 0) {
        changeState(StateID_Walk);
    }
}

void daEnNoko_c::initializeState_SpitOut_Ready() {
    mNoHitPlayer.mTimer[ACTOR_PARAM(BlockHitPlayer)] = 16;
}

void daEnNoko_c::finalizeState_SpitOut_Ready() { }

void daEnNoko_c::executeState_SpitOut_Ready() {
    if (mEatenByID == BASE_ID_NULL) {
        return;
    }

    dActor_c *player = (dActor_c *) fManager_c::searchBaseByID(mEatenByID);
    if (player == nullptr) {
        deleteRequest();
    } else {
        setSlideThrowSpeed(player);
        mPlayerNo = player->getPlrNo();
        changeState(StateID_Slide);
    }
}

void daEnNoko_c::initializeState_BgmDance() {
    static const char *sc_bgmDanceAnim[3] = {
        "BGM_anim_walkA_1",
        "BGM_anim_walkA_1",
        "BGM_anim_walkA_3",
    };

    setMoveAnimation(sc_bgmDanceAnim[mDanceMove], m3d::FORWARD_ONCE, 3.0f);
    mSpeed.x = 0.0;
    mDancesRemaining = 3;
    mBgmDanceRotSpeed = mAngle.y.abs() / mDancesRemaining;
}

void daEnNoko_c::finalizeState_BgmDance() {}

void daEnNoko_c::executeState_BgmDance() {
    mNokoModel.play();
    calcSpeedY();
    posMove();

    if (EnBgCheck() & 1) {
        mSpeed.y = 0.0f;
    }

    if (mDancesRemaining > 0) {
        mDancesRemaining--;
        sLib::chaseAngle(&mAngle.y.mAngle, 0, mBgmDanceRotSpeed);
    } else {
        mAngle.y = 0;
    }

    if (mMoveAnim.isStop()) {
        changeState(StateID_BgmDanceEd);
    }
}

void daEnNoko_c::initializeState_BgmDanceEd() {
    setMoveAnimation("walkA", m3d::FORWARD_ONCE, 4.0f);
    mSpeed.x = 0.0f;
    mDancesRemaining = 4;
    mBgmDanceRotSpeed = abs(l_turn_target_angle[mDirection]) / mDancesRemaining;
}

void daEnNoko_c::finalizeState_BgmDanceEd() {}

void daEnNoko_c::executeState_BgmDanceEd() {
    mNokoModel.play();
    calcSpeedY();
    posMove();

    if (EnBgCheck() & 1) {
        mSpeed.y = 0.0;
    }

    if (mDancesRemaining > 0) {
        mDancesRemaining--;
        sLib::chaseAngle(&mAngle.y.mAngle, l_turn_target_angle[mDirection], mBgmDanceRotSpeed);

        if (mDancesRemaining > 0) {
            return;
        }

        mAngle.y = l_turn_target_angle[mDirection];
        changeState(StateID_Walk);
    }
}

void daEnNoko_c::nodeCallback_c::timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl) {
    mMtx_c mtx;
    if (strcmp(resMdl.GetResNode(nodeId).GetName(), "head") == 0) {
        manip->GetMatrix(&mtx);
        mtx.XrotM(mpOwner->mBgmDanceAngle);
        manip->SetMatrix(mtx);
    }
}
