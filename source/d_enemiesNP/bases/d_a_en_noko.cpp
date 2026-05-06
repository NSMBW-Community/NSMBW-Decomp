#include <game/bases/d_a_en_noko.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_audio.hpp>

const sBcSensorPoint smc_noko_head = { SENSOR_IS_POINT, 0, 0xc000 };
const sBcSensorLine smc_noko_foot = { SENSOR_IS_LINE, -0x4000, 0x4000, 0x0 };
const sBcSensorLine smc_noko_wall = { SENSOR_IS_LINE, 0x6000, 0x9000, 0x6000 };
const sCcDatNewF ccData = {
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

const float buf_80ad34b0[2] = {0.5f, -0.5f};
const s16 buf_80ad34b8[2] = {0x800, -0x800};
const s16 buf_80ad34bc[2] = {0x4000, -0x4000};

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
    mAmiLayer = (mParam >> 16) & 1;
    mAngle.y = buf_80ad34bc[dir];
    m_8ac = mPos;
    mFlags |= dEn_c::EN_FLAG_16;
    mPos.z = l_Ami_Zpos[(mParam >> 16) & 1];

    mNokoAnimTex.setPlayMode(m3d::FORWARD_ONCE, 0);
    mNokoModel.setAnm(mNokoAnimTex, 0.0f);
    mNokoAnimTex.setFrame(mWalksOffLedges, 0);
    mNokoAnimTex.setRate(0.0f, 0);

    mSensorHead = smc_noko_head;
    mSensorWall = smc_noko_wall;
    mSensorFootNormal = smc_noko_foot;

    mCcData.mBase.mOffset = ccData.mBase.mOffset;
    mCcData.mBase.mSize = ccData.mBase.mSize;
    mCcData.mKind = ccData.mKind;
    mCcData.mAttack = ccData.mAttack;
    mCcData.mVsKind = ccData.mVsKind;
    mCcData.mVsDamage = ccData.mVsDamage;
    mCcData.mStatus = ccData.mStatus;
    mCcData.mCallback = ccData.mCallback;
    mCc.set(this, &mCcData, l_Ami_Line[mAmiLayer]);
    mCc.entry();

    m_8c4 = dActorMng_c::m_instance->mGoombaZOrderThing * 16.0f;
    dActorMng_c::m_instance->mGoombaZOrderThing = (dActorMng_c::m_instance->mGoombaZOrderThing + 1) & 0xF;

    changeStateAccordingToSettings();

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
        } else if (dAudio::isBgmAccentSign(4)) {
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

    if (isState(daEnShell_c::StateID_Wakeup)) {
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
    mVec3_c center = getCenterPos();
    mVec3_c efPos(center.x, center.y, 5500.0f);
    mQuickSandEffect.createEffect("Wm_en_quicksand", 0, &efPos, nullptr, nullptr);
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

    mMatrix.trans(pos);
    mMatrix.ZXYrotM(angle);

    mNokoModel.setLocalMtx(&mMatrix);

    mNokoModel.setScale(mBoyoMng.getScale());
    mNokoModel.calc(false);
}

void daEnNoko_c::calcShellEffectPos() {
    m3d::mdl_c * model = &mModel;
    nw4r::g3d::ResMdl resMdl = model->getResMdl();
    nw4r::g3d::ResNode resNode = resMdl.GetResNode("nokonoko_shell_model");

    ulong idx = resNode.GetID();
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
    u8 x = dActor_c::getTrgToSrcDir_Main(actor->getCenterX(), getCenterX());

    u8 mShellKind = mUseBaseIceBehaviour;
    if (mDirection == x || mShellKind == 0) {
        return false;
    }

    mDirection = x;
    mAngle.y = buf_80ad34bc[x];
    m_8a8 *= -1;
    vf30C();

    return true;
}

void daEnNoko_c::setNokoBc() {
    mSensorHead = smc_noko_head;
    mSensorWall = smc_noko_wall;
    mSensorFootNormal = smc_noko_foot;
    mBc.set(this, mSensorFootNormal, mSensorHead, mSensorWall);
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
    float width = mCc.mCcData.mBase.mSize.x;
    float offX = mCc.mCcData.mBase.mOffset.x;
    float offY = mCc.mCcData.mBase.mOffset.y;

    float x = mPos.x + offX;
    float y = mPos.y + offY;

    u32 u1 = dBc_c::getUnitType(x + width, y, mLayer);
    u8 u2 = dBc_c::getUnitKind(x + width, y, mLayer) & 0xFF;
    u32 u3 = dBc_c::getUnitType(x - width, y, mLayer);
    u8 u4 = dBc_c::getUnitKind(x - width, y, mLayer) & 0xFF;

    if (
        u1 & BIT_FLAG(10) && u2 >= 2 ||
        u3 & BIT_FLAG(10) && u4 >= 2
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
    static const s16 buf_80ad34f0[2] = {-0x1555, 0x1555};
    sLib::chaseAngle(&m_8a8, buf_80ad34f0[mDirection], 0x400);
}

bool daEnNoko_c::createIceActor() {
    static const int buf_80ad34f8[2] = {0, 1};
    static const float buf_80ad3500[2] = {-2.0f, -4.0f};

    float f_60;
    mVec3_c f_5c;
    int ice_mode;

    mVec3_c scale;

    if (*mStateMgr.getMainStateID() == StateID_BgmDance) {
        f_5c.set(mPos.x, mPos.y - 4.0f, mPos.z + 2.0f);
        ice_mode = 1;
        scale.set(1.05f, 1.0f, 1.75f);
    } else {
        ice_mode = buf_80ad34f8[mUseBaseIceBehaviour];
        f_5c.set(mPos.x, mPos.y + buf_80ad3500[mUseBaseIceBehaviour], mPos.z);
        scale.set(1.05f, 1.0f, 1.05f);
    }

    dIceInfo iceInfo[] = {
        {
            ice_mode,
            f_5c,
            scale
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

void daEnNoko_c::setMoveAnimation(const char *name, m3d::playMode_e mode, float frame) {
    mMoveAnim.setAnm(mNokoModel, mNokoResFile.GetResAnmChr(name), mode);
    mNokoModel.setAnm(mMoveAnim, frame);
}

void daEnNoko_c::setBaseAnimation(const char *name, m3d::playMode_e mode, float frame) {
    mAnim.setAnm(mModel, mResFile.GetResAnmChr(name), mode);
    mModel.setAnm(mAnim, frame);
}

bool daEnNoko_c::sub_80A73BC0() {
    static const float buf_80ad3518[2] = {2.5f, -2.5f};

    mVec3_c pos(mPos.x + buf_80ad3518[mDirection], mPos.y + 4.0f, mPos.z);
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

void daEnNoko_c::sub_80A73CB0() {
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

    float f = buf_80ad34b0[mDirection];
    mUseBaseIceBehaviour = 1;
    mAccelY = -0.1875f;
    mAccelF = 0.05f;
    mSpeedMax.x = f;
    mSpeedMax.y = -4.0f;
    mSpeedMax.z = 0.0f;
    mSensorFootNormal = smc_noko_foot;
    mSpeed.x = f;
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
            return;
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
        mPlayerNo = player->getPlrNo();
        changeState(StateID_Slide);
    }
}

void daEnNoko_c::initializeState_BgmDance() {
    static const char *buf_80afdd98[3] = {
        "BGM_anim_walkA_1",
        "BGM_anim_walkA_1",
        "BGM_anim_walkA_3",
    };

    setMoveAnimation(buf_80afdd98[mDanceMove], m3d::FORWARD_ONCE, 3.0f);
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
        changeState(StateID_BgmDanceEd);
    }
}

void daEnNoko_c::initializeState_BgmDanceEd() {
    setMoveAnimation("walkA", m3d::FORWARD_ONCE, 4.0f);
    mSpeed.x = 0.0f;
    mDancesRemaining = 4;
    mYRotIncrease = abs(buf_80ad34bc[mDirection]) / mDancesRemaining;
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
        mDancesRemaining--;
        sLib::chaseAngle((s16*)&mAngle.y, buf_80ad34bc[mDirection], mYRotIncrease);

        if (mDancesRemaining > 0) {
            return;
        }

        mAngle.y = buf_80ad34bc[mDirection];
        changeState(StateID_Walk);
    }
}

void daEnNoko_c::nodeCallback_c::timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl) {
    mMtx_c mtx;
    if (strcmp(resMdl.GetResNode(nodeId).GetName(), "head") == 0) {
        manip->GetMatrix(&mtx);
        mtx.XrotM(mpOwner->m_8a8);
        manip->SetMatrix(mtx);
    }
}