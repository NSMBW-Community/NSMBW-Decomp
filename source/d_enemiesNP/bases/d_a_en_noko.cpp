#include <game/bases/d_a_en_noko.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_audio.hpp>

const float buf_80ad34b0[2] = {0.5f, -0.5f};
const mAng buf_80ad34b8[2] = {0x800, 0xF800};
const mAng buf_80ad34bc[2] = {0x4000, 0xC000};
const s16 buf_80ad34f0[2] = {0xEAAB, 0x1555};
const int buf_80ad34f8[2] = {0, 1};
const float buf_80ad3500[2] = {-2.0f, -4.0f};
const float buf_80ad3518[2] = {2.5f, -2.5f};
const char *buf_80afdd98[3] = {
    "BGM_anim_walkA_1",
    "BGM_anim_walkA_1",
    "BGM_anim_walkA_3",
};

STATE_DEFINE(daEnNoko_c, BlockAppear);
STATE_DEFINE(daEnNoko_c, Walk);
STATE_DEFINE(daEnNoko_c, Turn);
STATE_DEFINE(daEnNoko_c, WindTurn);
STATE_DEFINE(daEnNoko_c, SpitOut_Ready);
STATE_DEFINE(daEnNoko_c, BgmDance);
STATE_DEFINE(daEnNoko_c, BgmDanceEd);

int daEnNoko_c::create() {
    mWalksOffLedges = mParam & 1;
    loadRes();
    createShell("nokonokoA", "g3d/nokonokoA.brres", "nokonoko_shell", "Tnokonoko_shell", mWalksOffLedges);
    mScale.set(1.0f, 1.0f, 1.0f);
    mCenterOffs.set(0.0f, 8.0f, 0.0);
    mVisibleAreaOffset.set(0.0f, 16.0f);
    mVisibleAreaSize.set(28.0f, 32.0f);
    mNokoModel.setCallback(&mMdlCallback);
    clrComboCnt();

    u8 dir = getPl_LRflag(mPos);
    mDirection = dir;
    mAngle.y = buf_80ad34bc[dir];
    mAmiLayer = (mParam >> 16) & 1;
    m_8ac = mPos;
    mFlags |= dEn_c::EN_FLAG_16;
    mPos.z = l_Ami_Zpos[(mParam >> 16) & 1];

    mNokoAnimTex.setPlayMode(m3d::FORWARD_ONCE, 0);
    mNokoModel.setAnm(mNokoAnimTex, 0.0f);
    mNokoAnimTex.setFrame(mWalksOffLedges, 0);
    mNokoAnimTex.setRate(0.0f, 0);

    mSensorHead.mX = 0;
    mSensorHead.mY = 0xC000;
    mSensorWall.mLineA = 0x6000;
    mSensorWall.mLineA = 0x9000;
    mSensorWall.mDistanceFromCenter = 0x6000;
    mSensorFootNormal.mLineA = -0x4000;
    mSensorFootNormal.mLineB = 0x4000;
    mSensorFootNormal.mDistanceFromCenter = 0;

    mCcData.mBase.mOffset.set(0.0f, 8.0f);
    mCcData.mBase.mSize.set(9.0f, 8.0f);
    mCcData.mKind = CC_KIND_ENEMY;
    mCcData.mAttack = CC_ATTACK_NONE;
    mCcData.mVsKind = ((1 << CC_KIND_YOSHI) | (1 << CC_KIND_ENEMY) | (1 << CC_KIND_TAMA));
    mCcData.mVsDamage = ~((1 << CC_ATTACK_NONE) | (1 << CC_ATTACK_YOSHI_MOUTH) | (1 << CC_ATTACK_SPIN_LIFT_UP) | (1 << CC_ATTACK_SAND_PILLAR));
    mCcData.mStatus = CC_STATUS_NONE;
    mCcData.mCallback = dEn_c::normal_collcheck;
    mCc.set(this, &mCcData, l_Ami_Line[mAmiLayer]);
    mCc.entry();

    m_8c4 = dActorMng_c::m_instance->mGoombaZOrderThing * 16.0f;
    dActorMng_c::m_instance->mGoombaZOrderThing = (dActorMng_c::m_instance->mGoombaZOrderThing + 1) & 0xF;

    changeStateAccordingToSettings();

    mBc.set(this, (const sBcSensorBase *) &mSensorFootNormal, (const sBcSensorBase *) &mSensorHead, (const sBcSensorBase *) &mSensorWall);
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

void daEnNoko_c::vf324() { }

void daEnNoko_c::changeStateAccordingToSettings() {
    mDirection = getPl_LRflag(mPos);
    if (((mParam >> 4) & 1) == 1) {
        mSpeed.y = 0.0f;
        changeState(daEnShell_c::StateID_Sleep);
    } else if ((mParam >> 0x1C) & 1) {
        changeState(daEnNoko_c::StateID_BlockAppear);
    } else if ((mParam >> 0x1D) & 1) {
        changeState(daEnNoko_c::StateID_SpitOut_Ready);
    } else {
        changeState(daEnNoko_c::StateID_Walk);
    }
}

int daEnNoko_c::preExecute() {
    int res = dEn_c::preExecute();
    if (res == 0) {
        return 0;
    }

    if (canDance()) {
        if (dAudio::isBgmAccentSign(2)) {
            danceWithMove(0);
        } else if (dAudio::isBgmAccentSign(2)) {
            danceWithMove(1);
        } else if (dAudio::isBgmAccentSign(8)) {
            danceWithMove(2);
        }
    }

    return 1;
}

int daEnNoko_c::execute() {
    mStateMgr.executeState();

    if (cullCheck()) {
        return 1;
    }

    if (isState(StateID_BgmDance)) {
        m_8a8 = 0;
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

    return 1;
}

int daEnNoko_c::draw() {
    if (!mUseBaseIceBehaviour) {
        return daEnShell_c::drawShell();
    }

    if (isState(StateID_Wakeup)) {
        daEnShell_c::drawShell();
    }

    mNokoModel.entry();
    return 1;
}

int daEnNoko_c::doDelete() {
    if (mNokoAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        mMoveAnim.remove();
        mNokoModel.remove();
        mNokoAnimTex.remove();
        deleteRest();
    }

    return 1;
}

void daEnNoko_c::deleteRest() { }

bool daEnNoko_c::isInQuicksand() {
    if (mBc.isFoot()) {
        mVec3_c pos = getCenterPos();
        u32 type = dBc_c::getUnitType(pos.x, pos.y, mLayer);
        u8 subkind = dBc_c::getUnitKind(pos.x, pos.y, mLayer) >> 16;

        if ((type & 0x8000) && (subkind == 3)) {
            return true;
        }
    }

    return false;
}

void daEnNoko_c::spawnQuicksandEffects() {
    mVec3_c pos = getCenterPos();
    mQuickSandEffect.createEffect("Wm_en_quicksand", 0, &pos, nullptr, nullptr);
}

bool daEnNoko_c::canDance() {
    if (isState(StateID_Walk) || isState(StateID_Turn) || isState(StateID_WindTurn) || isState(StateID_BgmDance) || isState(StateID_BgmDanceEd)) {
        return true;
    }

    return false;
}

void daEnNoko_c::danceWithMove(int move) {
    mDanceMove = move;
    changeState(StateID_BgmDance);
}

void daEnNoko_c::vf310() {
    calcShellMdl();
    mVec3_c pos = getPos();
    mAng3_c angle = mAngle;
    dActor_c::changePosAngle(&pos, &angle, 1);
    PSMTXTrans(mMatrix, pos.x, pos.y, pos.z);
    mMatrix.ZXYrotM(angle.x, angle.y, angle.z);
    mNokoModel.setLocalMtx(&mMatrix);
    mVec3_c boyo_scale = mBoyoMng.mScale;
    mNokoModel.setScale(boyo_scale);
    mNokoModel.calc(false);
}

mVec3_c daEnNoko_c::getPos() {
    return mPos;
}

void daEnNoko_c::calcShellEffectPos() {
    m3d::mdl_c * model = &mModel;
    nw4r::g3d::ResMdl resMdl = model->getResMdl();
    nw4r::g3d::ResNode resNode = resMdl.GetResNode("nokonoko_shell_model");

    ulong idx = 0;
    if (resNode.IsValid()) {
        idx = resNode.GetID();
    }
    mMtx_c matrix;

    model->getNodeWorldMtx(idx, &matrix);
    matrix.multVecZero(m_71c);
    if (mIsFlipped) {
        m_71c.y -= 16.0f;
    }
}

bool daEnNoko_c::setPlayerDamage(dActor_c *ac_player) {
    daPlBase_c * player = (daPlBase_c *)ac_player;
    bool ok = player->setDamage(this, daPlBase_c::DAMAGE_DEFAULT);

    if (ok && isWalking()) {
        sub_80A73330(ac_player);
        return true;
    }

    return false;
}

bool daEnNoko_c::isWalking() {
    return true;
}

bool daEnNoko_c::sub_80A73330(dActor_c *actor) {
    bool x = dActor_c::getTrgToSrcDir_Main(actor->mPos.x + actor->mCenterOffs.x, mPos.x + mCenterOffs.x);

    if ((mDirection == x) || ((mUseBaseIceBehaviour & 0xFF) == 0)) {
        return false;
    }

    mDirection = x;
    mAngle.y = buf_80ad34bc[x];
    m_8a8 = -m_8a8;
    vf30C();

    return true;
}

void daEnNoko_c::setBc() {
    mSensorHead.mBase.mFlags = SENSOR_IS_POINT;
    mSensorHead.mX = 0;
    mSensorHead.mY = 0xc000;
    mSensorWall.mBase.mFlags = SENSOR_IS_LINE;
    mSensorWall.mLineA = 0x6000;
    mSensorWall.mLineB = 0x9000;
    mSensorWall.mDistanceFromCenter = 0x6000;
    mSensorFootNormal.mBase.mFlags = SENSOR_IS_LINE;
    mSensorFootNormal.mLineA = -0x4000;
    mSensorFootNormal.mLineB = 0x4000;
    mSensorFootNormal.mDistanceFromCenter = 0;
    mBc.set(this, (const sBcSensorBase *) &mSensorFootNormal, (const sBcSensorBase *) &mSensorHead, (const sBcSensorBase *) &mSensorWall);
}

void daEnNoko_c::vf30C() {
    if (isState(StateID_Turn)) {
        changeState(StateID_Walk);
    } else {
        mSpeed.x = -mSpeed.x;
    }
}

bool daEnNoko_c::turnProc() {
    mAng turnAng;
    u32 turnDir;

    vf304(&turnDir, &turnAng);
    mAngle.y += turnAng;

    int z = ((mAng *)buf_80ad34bc)[turnDir];
    int x = abs(z);
    int y = abs(mAngle.y);

    if (y >= x) {
        mAngle.y = z;
        return true;
    }

    return false;
}

void daEnNoko_c::vf304(u32 * pDir, mAng * pAng) {
    u32 x = mDirection;
    *pDir = x;
    *pAng = buf_80ad34b8[x];
}

void daEnNoko_c::setZPos() {
    if (mLayer == 0) {
        mPos.z = m_8c4 + l_Ami_Zpos[mAmiLayer];
    } else {
        mPos.z = -2500.0f;
    }
}

void daEnNoko_c::updateAmiLine() {
    float f1 = mCc.mCcData.mBase.mSize.x;
    float f2 = mCc.mCcData.mBase.mOffset.x;
    float f3 = mPos.x;
    float f4 = f3 + f2;

    float f5 = mCc.mCcData.mBase.mOffset.y;
    float f6 = mPos.y;
    float f7 = f6 + f5;

    u32 u1 = dBc_c::getUnitType(f4 + f1, f7, mLayer);
    u32 u2 = dBc_c::getUnitKind(f4 + f1, f7, mLayer) & 0xFF;
    u32 u3 = dBc_c::getUnitType(f4 - f1, f7, mLayer);
    u32 u4 = dBc_c::getUnitKind(f4 - f1, f7, mLayer) & 0xFF;

    if ((((u1 & 1024) == 0) || (u2 < 2)) && (((u3 & 1024) == 0) || (u4 < 2))) {
        goto e8;
    } else if (!mAmiLayer) {
        mCc.mAmiLine = 1;
        goto ret;
    } else {
        mCc.mAmiLine = 2;
        goto ret;
    }
    {
        e8:
        mCc.mAmiLine = 3;
    }
ret:
    return;
}

void daEnNoko_c::dance() {
    sLib::chaseAngle(&m_8a8, buf_80ad34f0[mDirection], 0x400);
}

bool daEnNoko_c::createIceActor() {
    float f_60;
    mVec3_c f_5c;
    int ice_mode;

    if (*mStateMgr.getMainStateID() == StateID_BgmDance) {
        f_5c.set(mPos.x, mPos.y - 4.0f, mPos.z + 2.0f);
        f_60 = 1.75f;
        ice_mode = 1;
    } else {
        int x = mUseBaseIceBehaviour;
        f_5c.set(mPos.x, mPos.y + buf_80ad3500[x], mPos.z);
        f_60 = 1.05f;
        ice_mode = buf_80ad34f8[x];
    }

    dIceInfo iceInfo[] = {
        {
            ice_mode,
            f_5c,
            mVec3_c(1.0f, 1.0f, f_60)
        }
    };

    return mIceMng.createIce(iceInfo, ARRAY_SIZE(iceInfo));
}

bool daEnNoko_c::checkSleep() {
    if (((mParam >> 4) & 1) == 1) {
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

    if (mUseBaseIceBehaviour == 0) {
        changeState(StateID_Sleep);
    } else {
        mMoveAnim.setRate(2.0f);
    }
}

void daEnNoko_c::endFunsui() {

    mIsFrozen = 0;

    if (mUseBaseIceBehaviour != 0) {
        mMoveAnim.setRate(1.0f);
        mSpeed.x = mXSpeedBeforeFrozen;
    }
}

float daEnNoko_c::getWindMultiplier() {
    daTagWind_c *ac_wind = (daTagWind_c *)fManager_c::searchBaseByProfName(fProfile::TAG_WIND, nullptr);

    if (ac_wind == nullptr) {
        return 0.0f;
    }

    return ac_wind->m_394;
}

void daEnNoko_c::setMoveAnimation(char * name, m3d::playMode_e mode, float frame) {
    mMoveAnim.setAnm(mNokoModel, mNokoResFile.GetResAnmChr(name), mode);
    mNokoModel.setAnm(mMoveAnim, frame);
}

void daEnNoko_c::setBaseAnimation(char * name, m3d::playMode_e mode, float frame) {
    mAnim.setAnm(mModel, mResFile.GetResAnmChr(name), mode);
    mModel.setAnm(mAnim, frame);
}

bool daEnNoko_c::sub_80A73BC0() {
    float fVar1 = mPos.y;
    mVec3_c local_14 = mVec3_c(mPos.x + buf_80ad3518[mDirection], fVar1 + 4.0f, mPos.z);
    u32 uVar2 = dBc_c::getUnitKind(local_14.x, fVar1 - 2.0f, mLayer);

    if (((uVar2 >> 0x10) & 0xff) == 8) {
        return false;
    }

    float local_18 = 0.0f;
    u32 temp = dBc_c::checkGround(&local_14, &local_18, mLayer, 1, -1);
    if ((temp == 0) || !(local_14.y < local_18) || (local_18 > mPos.y - 5.0f)) {
        return true;
    }

    return false;
}

void daEnNoko_c::sub_80A73CB0() {
    mVec3_c pos = mVec3_c(mPos.x, mPos.y, 5500.0f);

    if (mBc.getFootAttr() == 12) {
        mEf::createEffect("Wm_en_sndlandsmk_s", 0, &pos, nullptr, nullptr);
    } else {
        mEf::createEffect("Wm_en_landsmoke_s", 0, &pos, nullptr, nullptr);
    }
}

void daEnNoko_c::initializeState_Walk() {
    if ((*mStateMgr.getOldStateID() != StateID_Turn) && (*mStateMgr.getOldStateID() != StateID_Walk) && (*mStateMgr.getOldStateID() != StateID_WindTurn) && (*mStateMgr.getOldStateID() != StateID_WakeupTurn)) {
        setMoveAnimation("walkA", m3d::FORWARD_LOOP, 1.0f);
    }

    float f = buf_80ad34b0[mDirection];
    mUseBaseIceBehaviour = 1;
    mAccelY = -0.1875f;
    mAccelF = 0.05f;
    mSpeedMax.x = f;
    mSpeedMax.y = -4.0f;
    mSpeedMax.z = 0.0f;
    mSensorFootNormal.mBase.mFlags = SENSOR_IS_LINE;
    mSensorFootNormal.mLineA = -0x4000;
    mSensorFootNormal.mLineB = 0x4000;
    mSensorFootNormal.mDistanceFromCenter = 0;
    mSpeed.x = f;
    m_8c8 = 0;
}

void daEnNoko_c::finalizeState_Walk() {}

void daEnNoko_c::executeState_Walk() {
    mNokoModel.play();
    float wind = getWindMultiplier();

    if (std::fabs(wind) > 0.475f) {
        if (((float)(int)buf_80ad34b0[mDirection]) * wind >= 0.0f) {
            changeState(StateID_WindTurn);
            return;
        } else {
            mSpeedMax.x = 0.0f;
        }
    } else {
        mSpeedMax.x = buf_80ad34b0[mDirection];
    }

    calcSpeedX();
    calcSpeedY();
    posMove();
    u32 foot = mBc.isFoot();
    EnBgCheck();
    if (mBc.isFoot()) {
        mSpeed.y = 0.0f;
        if ((mWalksOffLedges == true) && (!sub_80A73BC0())) {
            changeState(StateID_Turn);
        }
    } else if (foot && !mInLiquid && (mSpeed.y <= 0.0f)) {
        mFootPush2.x += m_1eb.x;
    }

    if (mBc.mFlags & (0x15 << mDirection)) {
        changeState(StateID_Turn);
    }
}

void daEnNoko_c::initializeState_Wakeup() {
    setMoveAnimation("revival", m3d::FORWARD_ONCE, 0.0f);
    setBaseAnimation("revival_shell", m3d::FORWARD_ONCE, 0.0f);
    mUseBaseIceBehaviour = 1;

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

    u32 x = EnBgCheck();
    if (x & 1) {
        mSpeed.y = 0.0f;
    }
    if (x & 4) {
        mSpeed.x = 0.0f;
    }

    if (mMoveAnim.isStop()) {
        changeState(StateID_WakeupTurn);
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
    u32 x = EnBgCheck();
    if (x & 1) {
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
    u32 x = EnBgCheck();
    if (x & 1) {
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
    mNoHitPlayer.mTimer[(mParam >> 24) & 3] = 16;
}

void daEnNoko_c::finalizeState_SpitOut_Ready() { }

void daEnNoko_c::executeState_SpitOut_Ready() {
    if (mEatenByID == BASE_ID_NULL) {
        return;
    }

    dActor_c *player = (dActor_c *)fManager_c::searchBaseByID(mEatenByID);
    if (player == nullptr) {
        deleteRequest();
    } else {
        setSlideThrowSpeed(player);
        mPlayerNo = getPlrNo();
        changeState(StateID_Slide);
    }
}

void daEnNoko_c::initializeState_BgmDance() {
    setMoveAnimation((char *)buf_80afdd98[mDanceMove], m3d::FORWARD_ONCE, 3.0f);
    mSpeed.x = 0.0;
    mDancesRemaining = 3;
    mYRotIncrease = (u16)(abs((int)mAngle.y) / (int)mDancesRemaining);
}

void daEnNoko_c::finalizeState_BgmDance() {}

void daEnNoko_c::executeState_BgmDance() {
    mNokoModel.play();
    calcSpeedY();
    posMove();

    u32 x = EnBgCheck();
    if (x & 1) {
        mSpeed.y = 0.0f;
    }

    if (mDancesRemaining > 0) {
        mDancesRemaining--;
        sLib::chaseAngle((s16*)&mAngle.y, 0, mYRotIncrease);
    } else {
        mAngle.y = 0;
    }

    if (mMoveAnim.isStop()) {
        changeState(StateID_BgmDance);
    }
}

void daEnNoko_c::initializeState_BgmDanceEd() {
    setMoveAnimation("walkA", m3d::FORWARD_ONCE, 4.0f);
    mAng x = buf_80ad34bc[mDirection];
    mSpeed.x = 0.0f;
    mDancesRemaining = 4;
    mYRotIncrease = abs(x) / mDancesRemaining;
}

void daEnNoko_c::finalizeState_BgmDanceEd() {}

void daEnNoko_c::executeState_BgmDanceEd() {
    mNokoModel.play();
    calcSpeedY();
    posMove();
    u32 x = EnBgCheck();
    if (x & 1) {
        mSpeed.y = 0.0;
    }

    if (mDancesRemaining > 0) {
        mAng y = buf_80ad34bc[mDirection];
        mDancesRemaining--;
        sLib::chaseAngle((s16*)&mAngle.y, y, mYRotIncrease);

        if (mDancesRemaining > 0) {
            return;
        }

        mAngle.y = buf_80ad34bc[mDirection];
        changeState(StateID_Walk);
    }
}
