#include <game/bases/d_a_ice.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_enemy.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_multi_manager.hpp>
#include <game/bases/d_quake.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_score_manager.hpp>
#include <game/bases/d_res_mng.hpp>
#include <constants/sound_list.h>

ACTOR_PROFILE(ICE_ACTOR, daIce_c, BIT_FLAG(11));

mVec3_c l_mdl_defsize[6] = {
    dIceParam_c::smc_ICE_DEFSIZE_SQUARE,
    dIceParam_c::smc_ICE_DEFSIZE_TATE,
    dIceParam_c::smc_ICE_DEFSIZE_YOKO,
    dIceParam_c::smc_ICE_DEFSIZE_BIG_SQUARE,
    dIceParam_c::smc_ICE_DEFSIZE_BIG_TATE,
    dIceParam_c::smc_ICE_DEFSIZE_BIG_YOKO
};

const float l_float_speed[daIce_c::ICE_WEIGHT_COUNT] = { 0.8f, 0.8f, 0.8f, 0.8f };
const float l_toge_float_speed[daIce_c::ICE_WEIGHT_COUNT] = { 0.5f, 0.5f, 0.5f, 0.5f };
const float l_attach_float_speed[daIce_c::ICE_WEIGHT_COUNT] = { 0.3f, 0.35f, 0.425f, 0.8f };

const sCcDatNewF l_ice_cc = {
    {
        { 0.0f, 0.0f },
        { 4.0f, 4.0f }
    },
    CC_KIND_ENEMY,
    CC_ATTACK_NONE,
    BIT_FLAG(CC_KIND_PLAYER) | BIT_FLAG(CC_KIND_PLAYER_ATTACK) | BIT_FLAG(CC_KIND_YOSHI) |
        BIT_FLAG(CC_KIND_ENEMY) | BIT_FLAG(CC_KIND_ITEM) | BIT_FLAG(CC_KIND_TAMA) | BIT_FLAG(CC_KIND_KILLER),
    BIT_FLAG(CC_ATTACK_ICE_BREAK) | BIT_FLAG(CC_ATTACK_YOSHI_FIRE) | BIT_FLAG(CC_ATTACK_YOSHI_BULLET),
    CC_STATUS_NONE,
    daIce_c::ccCallBack
};

STATE_DEFINE(daIce_c, Freeze);
STATE_DEFINE(daIce_c, Cover);
STATE_DEFINE(daIce_c, Hop);
STATE_DEFINE(daIce_c, Sink);
STATE_DEFINE(daIce_c, PushSink);
STATE_DEFINE(daIce_c, Float);
STATE_DEFINE(daIce_c, SmallFloat);
STATE_DEFINE(daIce_c, FloatWait);
STATE_DEFINE(daIce_c, TogeFloat);
STATE_DEFINE(daIce_c, ShakeFloat);
STATE_DEFINE(daIce_c, Air);
STATE_DEFINE(daIce_c, Fall);
STATE_DEFINE(daIce_c, Land);
STATE_DEFINE(daIce_c, Dive);
STATE_DEFINE(daIce_c, YoganDive);
STATE_DEFINE(daIce_c, Carry);
STATE_DEFINE(daIce_c, Throw);
STATE_DEFINE(daIce_c, Drop);
STATE_DEFINE(daIce_c, Melt_Normal);

int daIce_c::create() {
    createMdl();

    mScale.set(1.0f, 1.0f, 1.0f);
    mInitialPos = mPos;
    m_614 = (IceType_e) ACTOR_PARAM(Param18);
    if (!ACTOR_PARAM(Param8)) {
        m_608 |= 1;
    }
    if (!ACTOR_PARAM(ParamC)) {
        m_608 |= 2;
    }
    mSpeedMax.y = -6.0f;
    mAnm.setPlayMode(m3d::FORWARD_LOOP, 0);
    mMdl.setAnm(mAnm, 1.0f);
    dEn_c *parent = (dEn_c *) getConnectParent();
    mPosDiff = parent->mPos - mPos;
    mpIceMng = &parent->mIceMng;
    calcMdl();
    m_600 = 2;
    m_5ec = 0;
    m_610 = mPos.x;
    switch (m_614) {
        case ICE_TYPE_1:
        case ICE_TYPE_6:
            changeState(StateID_Freeze);
            break;
        case ICE_TYPE_7:
            changeState(StateID_Cover);
            break;
        case ICE_TYPE_2:
            changeState(StateID_Air);
            break;
        case ICE_TYPE_3:
            changeState(StateID_Fall);
            break;
        case ICE_TYPE_4:
        case ICE_TYPE_5:
            m_5ec = 1;
            switch (dBc_c::checkWater(mPos.x, mPos.y, mLayer, nullptr)) {
                case 3:
                case 4:
                    yoganEffect();
                    mSpeed.y = -12.0f;
                    changeState(StateID_YoganDive);
                    break;
                default:
                    changeState(StateID_Sink);
                    break;
            }
            break;
        case ICE_TYPE_8:
            changeState(StateID_Hop);
            break;
        default:
            changeState(StateID_Land);
            break;
    }
    m_5a8 = mPos;
    return SUCCEEDED;
}

void daIce_c::createMdl() {
    int iceType = ACTOR_PARAM(IceType);
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);

    mResFileMdl = dResMng_c::m_instance->getRes("ice", "g3d/ice.brres");
    const static char *cs_ice_mdl_name[] = {
        "ice_A1", "ice_B1", "ice_C1",
        "ice_A2", "ice_B2", "ice_C2"
    };
    const char *iceMdlName = cs_ice_mdl_name[iceType];
    nw4r::g3d::ResMdl mdl = mResFileMdl.GetResMdl(iceMdlName);
    mMdl.create(mdl, &mAllocator, 0x24);
    setSoftLight_MapObj(mMdl);
    mMdl.setPriorityDraw(127, 128);

    const static char *cs_ice_anm_name[] = {
        "ice_A1", "ice_B1", "ice_C1",
        "ice_A2", "ice_B2", "ice_C2"
    };
    const char *iceAnmName = cs_ice_anm_name[iceType];
    mResFileAnm = mResFileMdl.GetResAnmTexSrt(iceAnmName);
    mAnm.create(mdl, mResFileAnm, &mAllocator);
    mAnm.setAnm(mMdl, mResFileAnm, 0, m3d::FORWARD_LOOP);

    mAllocator.adjustFrmHeap();
}

int daIce_c::preExecute() {
    if (dActorState_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    dEn_c *parent = (dEn_c *) getConnectParent();
    if (parent == nullptr) {
        deleteRequest();
        return NOT_READY;
    }

    if (m_600 > 0) {
        if (--m_600 == 0) {
            mCc.mCcData.mBase.mSize.set(mCcSize.x * 0.5f, mCcSize.y * 0.5f);
            mCc.mCcData.mVsDamage |= BIT_FLAG(CC_ATTACK_SPIN_LIFT_UP);
            mCc.mCcData.mAttack = CC_ATTACK_NONE;
            mBgCtr.entry();
        }
    }
    if (m_60c == 0 && isQuakeDamage() && dQuake_c::m_instance->mFlags & 6) {
        quakeDamage();
        return NOT_READY;
    }

    m_610 = mPos.x;
    return SUCCEEDED;
}

int daIce_c::execute() {
    mStateMgr.executeState();
    mAnm.play();
    mIceEfMaker.execute();
    if (!isState(StateID_Melt_Normal)) {
        enlargeBgc();
        mBgCtr.calc();
        if (canCarry()) {
            mVec2_c size(mCcSize.x * 0.5f + 2.0f, mCcSize.y * 0.5f + 2.0f);
            carryFukidashiCheck(17, size);
        } else if (mCarryFukidashiPlayerNo >= 0) {
            carryFukidashiCancel(17, -1);
        }
    }
    m_5a8 = mPos;
    mBgCollFlags = 0;
    return SUCCEEDED;
}

int daIce_c::draw() {
    mMdl.entry();
    return SUCCEEDED;
}

void daIce_c::deleteReady() {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *pl = daPyMng_c::getPlayer(i);
        if (pl != nullptr && fManager_c::searchBaseByID(pl->mCarryActorID) == this) {
            pl->cancelCarry(this);
        }
    }
    if (mCarryFukidashiPlayerNo >= 0) {
        carryFukidashiCancel(17, -1);
    }
}

int daIce_c::doDelete() {
    mIceEfMaker.fin();
    return SUCCEEDED;
}

void daIce_c::calcMdl() {
    mVec3_c pos = m_59c;
    mAng3_c ang = mAngle;
    changePosAngle(&pos, &ang, 1);
    mMatrix.trans(pos.x, pos.y, pos.z);
    mMatrix.YrotM(ang.y);
    mMatrix.XrotM(ang.x);
    mMatrix.concat(mMtx_c::createTrans(0.0f, mCcSize.y * 0.5f, 0.0f));
    mMatrix.ZrotM(ang.z);
    mMatrix.concat(mMtx_c::createTrans(0.0f, mCcSize.y * -0.5f, 0.0f));
    mMdl.setLocalMtx(&mMatrix);
    mMdl.setScale(mScale);
    mMdl.calc(false);
}

bool daIce_c::vf68(dBg_ctr_c *collider) {
    if (collider != nullptr && collider->mFlags & 0x800 && collider->mpActor->checkCarried(nullptr)) {
        mBgCollFlags = 0;
        return false;
    }

    return true;
}

bool daIce_c::someCarryCheck() {
    if (mBc.mpCtrFoot != nullptr) {
        return daIce_c::vf68(mBc.mpCtrFoot);
    }
    return true;
}

void daIce_c::setInfo(mVec3_c size, dIceEfScale_c scale) {
    initSize(size);
    initPos();
    initWeight();
    initCollision();
    initEffect(scale);
    mIceEfMaker.createEffect(dIceEfMaker_c::ICE_EF_FREEZE);
    mIceEfMaker.createEffect(dIceEfMaker_c::ICE_EF_SMOKE);
}

void daIce_c::initSize(const mVec3_c &size) {
    mScale = size;
    mVec3_c ccSize;
    ccSize.x = l_mdl_defsize[ACTOR_PARAM(IceType)].x * size.x;
    ccSize.y = l_mdl_defsize[ACTOR_PARAM(IceType)].y * size.y;
    ccSize.z = l_mdl_defsize[ACTOR_PARAM(IceType)].z * size.z;
    mCcSize = ccSize;
    mCenterOffs.set(0.0f, mCcSize.y * 0.5f, 0.0f);
}

void daIce_c::initPos() {
    if (isState(StateID_Land)) {
        float height = 0.0f;
        if (calcLandLine(height)) {
            mPos.y = height + 1.0f;
        }
        mLastPos.y = mPos.y + 8.0f;
    } else if (isState(StateID_Fall)) {
        float height = 0.0f;
        if (calcLandLine(height)) {
            mPos.y = height + 1.0f;
        }
        mLastPos.y = mPos.y + 8.0f;
    }
}

void daIce_c::initWeight() {
    float area = mCcSize.x * mCcSize.y;
    if (area < 600.0f) {
        mWeightType = 0;
    } else if (area < 1536.0f) {
        mWeightType = 1;
    } else if (area < 3072.0f) {
        mWeightType = 2;
    } else {
        mWeightType = 3;
    }
}

void daIce_c::initCollision() {
    dEn_c *parent = (dEn_c *) getConnectParent();
    mLayer = parent->mLayer;
    setBc();
    if (isState(StateID_Land)) {
        mSensorWall.mFlags |= BIT_FLAG(30);
    }

    float sides[] = { 1.0f, -1.0f };
    if (isState(StateID_Land) || isState(StateID_Air) || isState(StateID_Fall)) {
        mBc.checkWall(&sides[0]);
        mBc.checkWall(&sides[1]);
    }

    mCc.set(this, (sCcDatNewF *) &l_ice_cc);
    mCc.mCcData.mBase.mOffset.set(0.0f, mCcSize.y * 0.5f);
    mCc.mCcData.mBase.mSize.set(mCcSize.x * 0.5f, mCcSize.y * 0.5f - 1.0f);
    mCc.mpFriendActor = parent;
    if (m_600 > 0) {
        mCc.mCcData.mAttack = CC_ATTACK_SHELL;
    }

    float w = mCcSize.x * 0.5f;
    float h = mCcSize.y * 0.5f;
    mBgCtr.set(this, 0.0f, 0.0f, 0.0f, 0.0f, callBackFoot, callBackHead, callBackWall, 1, 0, nullptr);
    mBgCtr.mpCheckRevFoot = checkRevFoot;
    mBgCtr.mpCheckRevHead = checkRevHead;
    mBgCtr.mpCheckRevWall = checkRevWall;
    m_544 = 0.0f;
    m_548 = 0.0f;
    m_54c = w / 8.0f;
    if (m_5ec == 0) {
        m_550 = mCcSize.y / 8.0f;
    } else {
        m_550 = h / 8.0f;
    }
    mBgCtr.mLayer = mLayer;
    mBgCtr.mFlags = 0;
    mBgCtr.mFlags2 = 1;
    mBgCtr.setAngleY3((short *) &smc_OBJBG_DUMMY_ANGLE);
    if (m_608 & 1) {
        mBgCtr.mFlags |= 0xd68;
    } else {
        mBgCtr.mFlags |= 0xc08;
    }
    u8 amiLine = 3;
    if (parent->mFlags & dEn_c::EN_FLAG_2) {
        amiLine = l_Ami_Line[parent->mAmiLayer];
    }
    mCc.mAmiLine = amiLine;
    mBc.mAmiLine = amiLine;
    mBgCtr.mAmiLine = amiLine;
}

void daIce_c::initEffect(dIceEfScale_c scale) {
    dIceEfScale_c *pScale = nullptr;
    if (scale != dIceEfScale_c::Zero) {
        pScale = &scale;
    }
    mIceEfMaker.init(ACTOR_PARAM(IceType), pScale);
}

bool daIce_c::checkInstantBreak(int a) {
    if (checkPlayerIn()) {
        return true;
    }

    if (a != 0 && checkBgIn()) {
        return true;
    }

    return false;
}

bool daIce_c::checkPlayerIn() {
    float boundLeft = mPos.x - mCcSize.x * 0.5f;
    float boundRight = mPos.x + mCcSize.x * 0.5f;
    float boundTop = mPos.y + mCcSize.y;
    float boundBottom = mPos.y;

    dCc_c *plCc;
    dAcPy_c *pl;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        pl = daPyMng_c::getCtrlPlayer(i);
        if (pl != nullptr && daPyMng_c::checkPlayer(i)) {
            plCc = &pl->mCc;
            float plLeft = plCc->getLeftPos();
            float plRight = plCc->getRightPos();
            float plTop = plCc->getLeftPos(); // [???]
            float plBottom = plCc->getRightPos(); // [???]
            if (plLeft > boundRight || plRight < boundLeft || plBottom > boundTop || plTop < boundBottom) {
                continue;
            }
            return true;
        }
    }
    return false;
}

bool daIce_c::checkBgIn() {
    u8 layer = getLayer();
    u8 amiLine = getAmiLine();
    float yTop = mPos.y + mCcSize.y - 4.0f;
    float yBottom = mPos.y + 4.0f;
    for (; yTop > yBottom; yTop -= 16.0f) {
        float xLeft = mPos.x - (mCcSize.x * 0.5f - 4.0f);
        float xRight = mPos.x + (mCcSize.x * 0.5f - 4.0f);
        while (xLeft < xRight) {
            if (dBc_c::checkBg(xLeft, yTop, layer, amiLine, 0x819)) {
                return true;
            }
            xLeft += 16.0f;
        }
        if (dBc_c::checkBg(xRight, yTop, layer, amiLine, 0x819)) {
            return true;
        }
    }
    float xLeft = mPos.x - (mCcSize.x * 0.5f - 4.0f);
    float xRight = mPos.x + (mCcSize.x * 0.5f - 4.0f);
    for (; xLeft < xRight; xLeft += 16.0f) {
        if (dBc_c::checkBg(xLeft, yBottom, layer, amiLine, 0x819)) {
            return true;
        }
    }
    return dBc_c::checkBg(xRight, yBottom, layer, amiLine, 0x819);
}

bool daIce_c::calcLandLine(float &height) {
    float landLine[40];
    int linePointCount = 0;

    float xRight;
    float yBottom;
    float xLeft;

    xLeft = mPos.x - (mCcSize.x * 0.5f - 4.0f);
    xRight = mPos.x + (mCcSize.x * 0.5f - 4.0f);
    yBottom = mPos.y + 10.0f;

    for (; xLeft < xRight; xLeft += 8.0f) {
        mVec3_c pos(xLeft, yBottom, mPos.z);
        float groundHeight = 0.0f;
        bool checkRes = dBc_c::checkGround(&pos, &groundHeight, mLayer, 1, -1);
        landLine[linePointCount] = -3e7f;
        if (checkRes) {
            landLine[linePointCount] = groundHeight;
        }
        linePointCount++;
    }

    mVec3_c pos(xRight, yBottom, mPos.z);
    float groundHeight = 0.0f;
    bool checkRes = dBc_c::checkGround(&pos, &groundHeight, mLayer, 1, -1);
    landLine[linePointCount] = -3e7f;
    if (checkRes) {
        landLine[linePointCount] = groundHeight;
    }
    linePointCount++;

    float maxHeight = -3e7;
    for (int i = 0; i < linePointCount; i++) {
        if (landLine[i] > maxHeight && landLine[i] <= yBottom) {
            maxHeight = landLine[i];
        }
    }

    bool res = false;
    if (maxHeight > mPos.y - 1.0f) {
        height = maxHeight;
        res = true;
    }
    return res;
}

void daIce_c::finalUpdate() {
    if (m_60c != 0) {
        return;
    }

    mVec3_c pos = mPos;
    if (isState(StateID_Carry)) {
        pos = calcCarryPos();
        mPos = pos;
    } else if (isState(StateID_Land) && mPos.x != m_5a8.x) {
        u8 dir = mPos.x - m_5a8.x < 0.0f ? 1 : 0;
        float side[] = { 1.0f, -1.0f };
        if (mBc.checkWallEnm(&side[dir])) {
            pos = mPos;
            mPos.x = m_5a8.x;
            mLastPos = mPos;
        }
    }
    m_59c = pos;
    if (!(mExecStop & 0xF) && m_5dc != 0) {
        shake();
    }
    if (!isState(StateID_Cover) && !isState(StateID_Melt_Normal)) {
        enemyAttach();
    }
    calcMdl();
}

bool daIce_c::isQuakeDamage() {
    if (getManager()->m_04 < 3) {
        return false;
    }
    if (isState(StateID_Freeze)) {
        if (getConnectParent()->mProfName == fProf::EN_IBARAMUSHI) {
            return true;
        }
        mVec3_c pos(mPos.x, mPos.y - 1.0f, mPos.z);
        float groundHeight = 0.0f;
        if (dBc_c::checkGround(&pos, &groundHeight, mLayer, 1, -1)) {
            if (groundHeight > pos.y - 2.0f) {
                return true;
            }
        }
    }
    if (isState(StateID_Land) || isState(StateID_Hop)) {
        if (mBc.isFoot()) {
            return true;
        }
    }

    return false;
}

void daIce_c::quakeDamage() {
    dActor_c *parent = (dActor_c *) getConnectParent();
    if (!ACTOR_PARAM(Param14)) {
        dEnemyMng_c::m_instance->breakdownSE(dEnemyMng_c::m_instance->m_154, mPos);
        dEnemyMng_c::m_instance->incQuakeComboCount(0);
        int count = dEnemyMng_c::m_instance->m_154;
        if (count >= 0) {
            if (count >= 8) {
                dMultiMng_c::mspInstance->setClapSE();
            }
            dScoreMng_c::getInstance()->UnKnownScoreSet(this, count, 0.0f, 24.0f);
        }
    }
    breakReq(dGameCom::rndInt(2), -1);
}

void daIce_c::combobreakReq(int dir, dActor_c *initiator) {
    dEn_c *parent = (dEn_c *) getConnectParent();
    int comboCount = -1;
    int plrNo = initiator->getPlrNo();
    if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
        initiator->mComboMultiplier++;
        if (initiator->mComboMultiplier >= 8) {
            initiator->mComboMultiplier = 8;
        }
        comboCount = initiator->mComboMultiplier;
    }
    if (comboCount >= 0) {
        int comboScore = parent->mCombo.getComboScore(comboCount);
        if (comboScore >= 8) {
            dMultiMng_c::mspInstance->setClapSE();
        }
        dScoreMng_c::getInstance()->ScoreSet(this, comboScore, plrNo);
    }
    breakReq(dir, -1);
    parent->mPlayerNo = plrNo;
}

void daIce_c::plcombobreakReq(int dir, dActor_c *initiator) {
    dEn_c *parent = (dEn_c *) getConnectParent();
    int comboCount = -1;
    int plrNo = initiator->getPlrNo();
    if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
        comboCount = dEnCombo_c::calcPlComboCnt(initiator);
    }
    if (comboCount >= 0) {
        int comboScore = parent->mCombo.getComboScore(comboCount);
        if (comboScore >= 8) {
            dMultiMng_c::mspInstance->setClapSE();
        }
        dScoreMng_c::getInstance()->ScoreSet(this, comboScore, plrNo);
    }
    breakReq(dir, -1);
    parent->mPlayerNo = plrNo;
}

void daIce_c::treadcombobreakReq(int dir, dActor_c *initiator) {
    dEn_c *parent = (dEn_c *) getConnectParent();

    int comboScore = parent->mCombo.getFumiScore(initiator);
    if (comboScore >= 8) {
        dMultiMng_c::mspInstance->setClapSE();
    }

    dScoreMng_c::getInstance()->ScoreSet(this, comboScore, initiator->getPlrNo());
    breakReq(dir, -1);
    parent->mPlayerNo = initiator->getPlrNo();
}

void daIce_c::starbreakReq(int dir, dActor_c *initiator) {
    dEn_c *parent = (dEn_c *) getConnectParent();

    int comboScore = parent->mCombo.getStarScore(initiator);
    if (comboScore >= 8) {
        dMultiMng_c::mspInstance->setClapSE();
    }

    dScoreMng_c::getInstance()->ScoreSet(this, comboScore, initiator->getPlrNo());
    breakReq(dir, -1);
    parent->mPlayerNo = initiator->getPlrNo();
}

void daIce_c::setBc() {
    if (ACTOR_PARAM(Param10) == 1) {
        mBc.set(this, nullptr, nullptr, nullptr);
        mBc.mpRc = nullptr;
        return;
    }

    mSensorFoot.mFlags = SENSOR_IS_LINE;
    mSensorFoot.mInfMargin = -(int) ((mCcSize.x * 0.5f - 3.0f) * 0x1000);
    mSensorFoot.mSupMargin = ((mCcSize.x * 0.5f - 3.0f) * 0x1000);
    mSensorFoot.mOffset = 0;

    mSensorHead.mFlags = SENSOR_IS_LINE;
    mSensorHead.mInfMargin = -(int) ((mCcSize.x * 0.5f - 3.0f) * 0x1000);
    mSensorHead.mSupMargin = ((mCcSize.x * 0.5f - 3.0f) * 0x1000);
    mSensorHead.mOffset = mCcSize.y * 0x1000;

    mSensorWall.mFlags = SENSOR_IS_LINE;
    mSensorWall.mInfMargin = 0x4000;
    mSensorWall.mSupMargin = (mCcSize.y - 4.0f) * 0x1000;
    mSensorWall.mOffset = (mCcSize.x * 0.5f - 0.5f) * 0x1000;

    mBc.set(this, mSensorFoot, mSensorHead, mSensorWall);
}

void daIce_c::setCarryBc() {
    if (ACTOR_PARAM(Param10) == 1) {
        mBc.set(this, nullptr, nullptr, nullptr);
        mBc.mpRc = nullptr;
        return;
    }

    daPyMng_c::getPlayer(mPlayerNo);

    mSensorFoot.mFlags = SENSOR_IS_LINE;
    mSensorFoot.mInfMargin = -(int) ((mCcSize.x * 0.5f - 8.0f) * 0x1000);
    mSensorFoot.mSupMargin = ((mCcSize.x * 0.5f - 8.0f) * 0x1000);
    mSensorFoot.mOffset = 0;

    mSensorHead.mFlags = SENSOR_IS_LINE;
    mSensorHead.mInfMargin = -0x2000;
    mSensorHead.mSupMargin = 0x2000;
    mSensorHead.mOffset = (mCcSize.y + 1.0f) * 0x1000;

    mSensorWall.mFlags = SENSOR_IS_LINE;
    mSensorWall.mInfMargin = 0x6000;
    mSensorWall.mSupMargin = (mCcSize.y - 6.0f) * 0x1000;
    if (mCcSize.x < 26.0f) {
        mSensorWall.mOffset = 0x2000;
    } else {
        mSensorWall.mOffset = mCcSize.x * 0.5f * 0x1000;
    }

    mBc.set(this, mSensorFoot, mSensorHead, mSensorWall);
}

void daIce_c::enlargeBgc() {
    const static EnlargeFunc cs_enlarge_f[] = {
       &daIce_c::enlargeBgcLand, &daIce_c::enlargeBgcWater
    };

    (this->*cs_enlarge_f[m_5ec])();
}

void daIce_c::enlargeBgcLand() {
    float width = mCcSize.x * 0.5f;
    m_544 += m_54c;
    if (m_544 >= width - 1.0f) {
        m_544 = width - 1.0f;
    }

    m_548 += m_550;
    if (m_548 >= mCcSize.y - 1.0f) {
        m_548 = mCcSize.y - 1.0f;
    }

    mBgCtr.setOfsX1(-m_544);
    mBgCtr.setOfsY1(m_548);
    mBgCtr.setOfsX2(m_544);
    mBgCtr.setOfsY2(0.0f);
}

void daIce_c::enlargeBgcWater() {
    float width = mCcSize.x * 0.5f;
    m_544 += m_54c;
    if (m_544 >= width - 1.0f) {
        m_544 = width - 1.0f;
    }

    float height = mCcSize.y * 0.5f;
    m_548 += m_550;
    if (m_548 >= height - 1.0f) {
        m_548 = height - 1.0f;
    }

    mBgCtr.setOfsX1(-m_544);
    mBgCtr.setOfsY1(height + m_548);
    mBgCtr.setOfsX2(m_544);
    mBgCtr.setOfsY2(height - m_548);
}

void daIce_c::enemyAttach() {
    dActor_c *parent = (dActor_c *) getConnectParent();
    parent->mPos = m_59c + mPosDiff;
}

void daIce_c::shake() {
    if (m_614 == 5) {
        watertogeShake();
    } else {
        normalShake();
    }
}

void daIce_c::normalShake() {
    // 2 frames left, 2 frames right
    float shakeDir = (dScStage_c::m_exeFrame & 2) ? 1.0f : -1.0f;
    m_59c.x += shakeDir;
}

void daIce_c::watertogeShake() {
    mShakeAngle1 += 0x180;
    if (mShakeAngle1 > 0x5000) {
        mShakeAngle1 = 0x5000;
    }
    mShakeAngle2 += mShakeAngle1;
    float sin = mAng(mShakeAngle2).sin();
    m_59c.x += sin;
    mAngle.z = mShakeAngle3 * sin;
}

void daIce_c::calcFloatAngle() {
    const static short cs_angle_dir[] = { -0x200, 0x200 };

    dEn_c *parent = (dEn_c *) getConnectParent();
    mFloatAngle += cs_angle_dir[parent->mBoyoMng.mDirection & 1];
    mAngle.z = mAng(mFloatAngle).sin() * 0x300;
}

void daIce_c::calcFloatSpeed() {
    if (mSpeedF > mFloatTargetSpeed) {
        mSpeedF -= mFloatAccel;
        if (mSpeedF <= mFloatTargetSpeed) {
            mSpeedF = mFloatTargetSpeed;
        }
    } else if (mSpeedF < mFloatTargetSpeed) {
        mSpeedF += mFloatAccel;
        if (mSpeedF >= mFloatTargetSpeed) {
            mSpeedF = mFloatTargetSpeed;
        }
    }
    mSpeed.x = -(mSpeedF * 4.0f) * mAngle.z.sin();
    mSpeed.y = mSpeedF * mAngle.z.cos();
    mSpeed.z = 0.0f;
    sLib::chase(&m_5d8, 0.0f, 0.05f);
    mSpeed.x += m_5d8;
}

bool daIce_c::checkPushSink(dActor_c *actor) {
    bool res = false;

    if (actor->mKind == STAGE_ACTOR_PLAYER) {
        dAcPy_c *pl = (dAcPy_c *) actor;
        if (pl->isMameAction()) {
            if (pl->isStatus(daPlBase_c::STATUS_HIP_ATTACK_FALL)) {
                m_5c0 = 1;
                res = true;
            }
        } else {
            if (pl->isStatus(daPlBase_c::STATUS_HIP_ATTACK_FALL) || pl->isStatus(daPlBase_c::STATUS_SPIN_HIP_ATTACK_FALL)) {
                m_5c0 = 1;
                res = true;
            } else if (pl->mSpeed.y < -3.0f) {
                m_5c0 = 0;
                res = true;
            }
        }
    } else if (actor->mKind == STAGE_ACTOR_YOSHI) {
        daYoshi_c *yoshi = (daYoshi_c *) actor;
        if (yoshi->isStatus(daPlBase_c::STATUS_HIP_ATTACK_FALL)) {
            m_5c0 = 1;
            res = true;
        } else if (yoshi->mSpeed.y < -3.0f) {
            m_5c0 = 0;
            res = true;
        }
    }

    return res;
}

dIceMng_c *daIce_c::getManager() {
    return mpIceMng;
}


bool daIce_c::checkUnderBreak(dActor_c *actor) {
    if (isState(StateID_Fall) || isState(StateID_Air)) {
        if (!(actor->mKind != STAGE_ACTOR_PLAYER && actor->mKind != STAGE_ACTOR_YOSHI)) {
            daPlBase_c *pl = (daPlBase_c *) actor;

            int plrNo = actor->getPlrNo();
            if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
                dQuake_c::getInstance()->startShock(actor->getPlrNo(), dQuake_c::TYPE_6, 2, 0, false);
            }

            pl->setDamage(this, daPlBase_c::DAMAGE_11);
            return true;
        }
    }
    return false;
}

bool daIce_c::checkUpperBreak(dActor_c *actor) {
    return false;
}

bool daIce_c::checkSideBreak(dActor_c *actor, int direction) {
    if (isState(StateID_Throw)) {
        if (!(actor->mKind != STAGE_ACTOR_PLAYER && actor->mKind != STAGE_ACTOR_YOSHI)) {
            daPlBase_c *pl = (daPlBase_c *) actor;
            pl->setDamage(this, daPlBase_c::DAMAGE_HIP_ATTACK);
            return true;
        }
    }

    return false;
}

void daIce_c::breakEffect() {
    if (dBc_c::checkWater(mPos.x, mPos.y + mCcSize.y * 0.7f, mLayer, nullptr) == dBc_c::WATER_CHECK_WATER) {
        mIceEfMaker.m_00 &= ~BIT_FLAG(1);
        mIceEfMaker.createEffect(dIceEfMaker_c::ICE_EF_WATER_BREAK);
    } else {
        mIceEfMaker.m_00 &= ~BIT_FLAG(1);
        mIceEfMaker.createEffect(dIceEfMaker_c::ICE_EF_BREAK);
        mIceEfMaker.hahenEffect();
    }
}

void daIce_c::meltEffect() {
    if (dBc_c::checkWater(mPos.x, mPos.y + mCcSize.y * 0.7f, mLayer, nullptr) == dBc_c::WATER_CHECK_WATER) {
        mIceEfMaker.m_00 &= ~BIT_FLAG(1);
        mIceEfMaker.createEffect(dIceEfMaker_c::ICE_EF_WATER_BREAK);
    } else {
        mIceEfMaker.m_00 &= ~BIT_FLAG(1);
        mIceEfMaker.createEffect(dIceEfMaker_c::ICE_EF_RELEASE);
        mIceEfMaker.hahenEffect();
    }
}

void daIce_c::callBackHead(dActor_c *self, dActor_c *other) {
    daIce_c *ice = (daIce_c *) self;

    if (ice->isDestroy()) {
        return;
    }

    if (ice->mBgCtr.m_d8 & 0xe || ice->checkUnderBreak(other)) {
        daPlBase_c *pl = (daPlBase_c *) other;
        int dir = !(ice->mPos.x >= other->mPos.x);
        if (!(other->mKind != STAGE_ACTOR_PLAYER && other->mKind != STAGE_ACTOR_YOSHI) && pl->isStar()) {
            ice->starbreakReq(dir, other);
        } else {
            if (other->getPlrNo() == -1) {
                ice->breakReq(dir, ice->getPlrNo());
            } else {
                ice->breakReq(dir, other->getPlrNo());
            }
        }
    }
}

void daIce_c::callBackFoot(dActor_c *self, dActor_c *other) {
    daIce_c *ice = (daIce_c *) self;

    if (ice->isDestroy()) {
        return;
    }

    if (!(ice->m_608 & 1)) {
        if (
            ice->isState(StateID_Float) ||
            ice->isState(StateID_FloatWait) ||
            ice->isState(StateID_SmallFloat) ||
            ice->isState(StateID_PushSink)
        ) {
            if (ice->checkPushSink(other)) {
                ice->changeState(StateID_PushSink);
            }
        }
    } else if (ice->mBgCtr.m_d8 & 0xe || ice->checkUpperBreak(other)) {
        daPlBase_c *pl = (daPlBase_c *) other;
        int dir = !(ice->mPos.x >= other->mPos.x);
        if (!(other->mKind != STAGE_ACTOR_PLAYER && other->mKind != STAGE_ACTOR_YOSHI)) {
            ice->treadcombobreakReq(dir, other);
        } else {
            if (other->getPlrNo() == -1) {
                ice->breakReq(dir, ice->getPlrNo());
            } else {
                ice->breakReq(dir, other->getPlrNo());
            }
        }
    }
}

void daIce_c::callBackWall(dActor_c *self, dActor_c *other, u8 dir) {
    daPlBase_c *pl = (daPlBase_c *) other;
    daIce_c *ice = (daIce_c *) self;

    if (ice->isDestroy()) {
        return;
    }

    int breakDir = !(ice->mPos.x >= other->mPos.x);
    switch (other->mKind) {
        case STAGE_ACTOR_PLAYER:
            if (ice->mBgCtr.m_d8 & 0xe || ice->checkSideBreak(pl, dir)) {
                if (pl->isStar()) {
                    ice->starbreakReq(breakDir, pl);
                } else if (pl->isStatus(daPlBase_c::STATUS_PENGUIN_SLIDE)) {
                    ice->plcombobreakReq(breakDir, pl);
                } else {
                    ice->breakReq(breakDir, ice->getPlrNo());
                }
            }
            break;
        case STAGE_ACTOR_YOSHI:
            if (ice->mBgCtr.m_d8 & 0xe || ice->checkSideBreak(pl, dir)) {
                if (pl->isStar()) {
                    ice->starbreakReq(breakDir, pl);
                } else {
                    ice->breakReq(breakDir, ice->getPlrNo());
                }
            }
            break;
        default:
            if (ice->mBgCtr.m_d8 & 0xe || ice->checkSideBreak(pl, dir)) {
                if (pl->getPlrNo() == -1) {
                    ice->breakReq(breakDir, ice->getPlrNo());
                } else {
                    ice->combobreakReq(breakDir, pl);
                }
            }
            break;
    }
}

bool daIce_c::checkRevHead(dActor_c *self, dActor_c *other) {
    return self->mPosDelta.y < 0.0f;
}

bool daIce_c::checkRevFoot(dActor_c *self, dActor_c *other) {
    return self->mPosDelta.y > 0.0f;
}

bool daIce_c::checkRevWall(dActor_c *self, dActor_c *other, u8 dir) {
    if (dir == DIR_LR_L) {
        if (self->mPosDelta.x > 0.0f || self->m_1eb.x > 0.0f) {
            return true;
        }
    } else {
        if (self->mPosDelta.x < 0.0f || self->m_1eb.x < 0.0f) {
            return true;
        }
    }
    return false;
}

void daIce_c::ccCallBack(dCc_c *self, dCc_c *other) {
    // [TODO]
}

void daIce_c::ccCallBack_Water(dCc_c *self, dCc_c *other) {
    daIce_c *ice = (daIce_c *) self->getOwner();
    dActor_c *actor = (dActor_c *) other->getOwner();

    int dir = !(ice->mPos.x <= actor->mPos.x);

    if (actor->mProfName == fProf::ICE_ACTOR) {
        daIce_c *ice2 = (daIce_c *) actor;
        if (ice->getManager() == ice2->getManager()) {
            return;
        }
    }
    if (other->mCcData.mAttack == CC_ATTACK_ICE_BREAK) {
        ice->breakReq(dir ^ 1, -1);
        self->mInfo |= CC_NO_HIT;
    }
}

void daIce_c::block_hit_init() {
    breakReq(dGameCom::rndInt(2) & 1, mPlayerNo);
}

void daIce_c::breakReq(int dir, int playerNo) {
    dEn_c *parent = (dEn_c *) getConnectParent();

    parent->mIceDeathDirection = dir;
    parent->mPlayerNo = playerNo;

    getManager()->setDestroyMode(dIceMng_c::DESTROY_BREAK);

    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        int damageScore = parent->mCombo.getDamageScore();
        dScoreMng_c::getInstance()->ScoreSet(this, damageScore, playerNo);
    }

    carryFukidashiCancel(17, -1);
    mBgCtr.release();
    parent->mPlayerNo = playerNo;
    m_60c = 1;
}

void daIce_c::floatbreakReq() {
    dEn_c *parent = (dEn_c *) getConnectParent();

    parent->mPlayerNo = getManager()->mPlrNo;
    getManager()->setDestroyMode(dIceMng_c::DESTROY_VANISH);
    m_60c = 1;
}

bool daIce_c::checkNetIn() {
    float width = mCcSize.x * 0.5f - 6.0f;
    if (dBc_c::checkWireNet(mPos.x - width, mPos.y + mCcSize.y - 6.0f, mLayer)) {
        return true;
    }
    if (dBc_c::checkWireNet(mPos.x + width, mPos.y + mCcSize.y - 6.0f, mLayer)) {
        return true;
    }
    if (dBc_c::checkWireNet(mPos.x - width, mPos.y + 6.0f, mLayer)) {
        return true;
    }
    if (dBc_c::checkWireNet(mPos.x + width, mPos.y + 6.0f, mLayer)) {
        return true;
    }
    return false;
}

void daIce_c::setCarry(dActor_c *actor) {
    mPlayerNo = actor->getPlrNo();
    mCarriedByPlrNo = mPlayerNo;
    mBgCtr.mpCarryActor = actor;
    carryFukidashiCancel(17, mCarriedByPlrNo);
    dGameCom::hideFukidashiForSession(mCarriedByPlrNo, 17);
    changeState(StateID_Carry);
}

bool daIce_c::checkPlHasami(int *plrNo) {
    int res = 0;

    if (checkPlAttach_Foot(&res, plrNo) && res != 0) {
        return true;
    } else if (checkPlAttach_Head(&res) && res != 0) {
        return true;
    } else if (checkPlAttach_Wall(0, &res) && res == 2) {
        return true;
    } else if (checkPlAttach_Wall(1, &res) && res == 2) {
        return true;
    }

    return false;
}

bool daIce_c::checkPlAttach_Head(int *ptr) {
    if (ptr != nullptr) {
        *ptr = 0;
    }

    bool res = false;
    for (dBc_c *curr = mBgCtr.mpTriggeredHead; curr != nullptr; curr = curr->mTriggeredHeadSensor) {
        dActor_c *actor = curr->mpOwner;
        if (actor != nullptr && !(actor->mKind != STAGE_ACTOR_PLAYER && actor->mKind != STAGE_ACTOR_YOSHI)) {
            daPlBase_c *pl = (daPlBase_c *) actor;
            if (ptr != nullptr && pl->isNowBgCross(daPlBase_c::BGC_HEAD)) {
                *ptr = 1;
            }
            res = true;
        }
    }
    return res;
}

bool daIce_c::checkPlAttach_Foot(int *ptr, int *plrNo) {
    if (ptr != nullptr) {
        *ptr = 0;
    }
    if (plrNo != nullptr) {
        *plrNo = -1;
    }

    bool res = false;
    for (dBc_c *curr = mBgCtr.mpTriggeredFoot; curr != nullptr; curr = curr->mTriggeredFootSensor) {
        dActor_c *actor = curr->mpOwner;
        if (actor != nullptr && !(actor->mKind != STAGE_ACTOR_PLAYER && actor->mKind != STAGE_ACTOR_YOSHI)) {
            daPlBase_c *pl = (daPlBase_c *) actor;
            if (ptr != nullptr && pl->isNowBgCross(daPlBase_c::BGC_FOOT)) {
                *ptr = 1;
            }
            if (plrNo != nullptr) {
                *plrNo = pl->getPlrNo();
            }
            res = true;
        }
    }
    return res;
}

bool daIce_c::checkPlAttach_Wall(int dir, int *ptr) {
    if (ptr != nullptr) {
        *ptr = 0;
    }

    bool res = false;
    for (dBc_c *curr = mBgCtr.mpTriggeredWall; curr != nullptr; curr = curr->getLinkW(&mBgCtr)) {
        dActor_c *actor = curr->mpOwner;
        if (actor != nullptr && !(actor->mKind != STAGE_ACTOR_PLAYER && actor->mKind != STAGE_ACTOR_YOSHI)) {
            daPlBase_c *pl = (daPlBase_c *) actor;
            if (dir == DIR_LR_R) {
                if (actor->mPos.x >= mPos.x) {
                    if (ptr != nullptr && (pl->isNowBgCross(daPlBase_c::BGC_WALL_TOUCH_R_2) || pl->isNowBgCross(daPlBase_c::BGC_SIDE_LIMIT_R))) {
                        *ptr = 1;
                        if (checkPlHasami_Wall(actor, dir)) {
                            *ptr = 2;
                        }
                    }
                    res = true;
                }
            } else {
                if (actor->mPos.x <= mPos.x) {
                    if (ptr != nullptr && (pl->isNowBgCross(daPlBase_c::BGC_WALL_TOUCH_L_2) || pl->isNowBgCross(daPlBase_c::BGC_SIDE_LIMIT_L))) {
                        *ptr = 1;
                        if (checkPlHasami_Wall(actor, dir)) {
                            *ptr = 2;
                        }
                    }
                    res = true;
                }
            }
        }
    }
    return res;
}

bool daIce_c::checkPlHasami_Wall(dActor_c *actor, int dir) {
    const static float sc_dir[] = { 1.0f, -1.0f };

    daPlBase_c *pl = (daPlBase_c *) actor;

    bool res = false;
    float margin = -2.0f;
    if (mPosDelta.x * sc_dir[dir] > 0.0f) {
        margin = 0.0f;
        res = true;
    } else if (pl->m_d80[(u8) dir] * sc_dir[dir] < 0.0f) {
        margin = -1.0f;
        res = true;
    }

    if (res && pl->checkInsideCrossBg(margin)) {
        return true;
    } else {
        return false;
    }
}

mVec3_c daIce_c::calcCarryPos() {
    float zero = 0.0f;
    dAcPy_c *pl = daPyMng_c::getPlayer(mCarriedByPlrNo);

    if (pl->isStatus(daPlBase_c::STATUS_OUT_OF_PLAY)) {
        return mPos;
    }

    mMtx_c mtx = mMtx_c::createTrans(pl->getCarryPos());
    mtx.concat(pl->getModel()->getMtx());
    mtx.concat(mMtx_c::createTrans(zero, zero, zero));
    mVec3_c res;
    mtx.multVecZero(res);
    return res;
}

BOOL daIce_c::canCarry() const {
    if (!(m_608 & 2)) {
        return false;
    }

    if (mWeightType >= ICE_WEIGHT_HEAVY) {
        return false;
    }

    if (
        isState(StateID_Land) ||
        isState(StateID_Hop) ||
        isState(StateID_Freeze)
    ) {
        return true;
    }

    return false;
}

fBaseID_e daIce_c::getParentID() {
    fBase_c *parent = getConnectParent();
    if (parent == nullptr) {
        return BASE_ID_NULL;
    }
    return parent->mUniqueID;
}

void daIce_c::yoganEffect() {
    mVec3_c efPos(mPos.x, mPos.y, 6500.0f);

    bool cmn = false;
    if (dScStage_c::m_instance != nullptr && dScStage_c::m_instance->mCurrWorld == WORLD_8 && dScStage_c::m_instance->mCurrCourse == STAGE_3) {
        cmn = true;
    }
    if (cmn) {
        mEf::createEffect("Wm_en_cmnmagmawave", 0, &efPos, nullptr, nullptr);
    } else {
        mEf::createEffect("Wm_en_magmawave", 0, &efPos, nullptr, nullptr);
    }

    mIceEfMaker.m_00 &= ~BIT_FLAG(1);
    mIceEfMaker.createEffect(dIceEfMaker_c::ICE_EF_YOGAN);
    dAudio::g_pSndObjMap->startSound(SE_OBJ_CMN_SPLASH_LAVA, efPos, 0);
}

void daIce_c::poisonEffect() {
    mVec3_c efPos(mPos.x, mPos.y, 6500.0f);

    mIceEfMaker.m_00 &= ~BIT_FLAG(1);
    mIceEfMaker.createEffect(dIceEfMaker_c::ICE_EF_POISON);
    dAudio::g_pSndObjMap->startSound(SE_OBJ_CMN_SPLASH_POISON, efPos, 0);
}
