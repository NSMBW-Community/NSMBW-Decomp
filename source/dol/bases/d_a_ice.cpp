#include <game/bases/d_a_ice.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_bg_parameter.hpp>
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

    mSensorFoot.mFlags = SENSOR_IS_MANY;
    mSensorFoot.mInfMargin = -(int) ((mCcSize.x * 0.5f - 3.0f) * 0x1000);
    mSensorFoot.mSupMargin = ((mCcSize.x * 0.5f - 3.0f) * 0x1000);
    mSensorFoot.mOffset = 0;

    mSensorHead.mFlags = SENSOR_IS_MANY;
    mSensorHead.mInfMargin = -(int) ((mCcSize.x * 0.5f - 3.0f) * 0x1000);
    mSensorHead.mSupMargin = ((mCcSize.x * 0.5f - 3.0f) * 0x1000);
    mSensorHead.mOffset = mCcSize.y * 0x1000;

    mSensorWall.mFlags = SENSOR_IS_MANY;
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

    mSensorFoot.mFlags = SENSOR_IS_MANY;
    mSensorFoot.mInfMargin = -(int) ((mCcSize.x * 0.5f - 8.0f) * 0x1000);
    mSensorFoot.mSupMargin = ((mCcSize.x * 0.5f - 8.0f) * 0x1000);
    mSensorFoot.mOffset = 0;

    mSensorHead.mFlags = SENSOR_IS_MANY;
    mSensorHead.mInfMargin = -0x2000;
    mSensorHead.mSupMargin = 0x2000;
    mSensorHead.mOffset = (mCcSize.y + 1.0f) * 0x1000;

    mSensorWall.mFlags = SENSOR_IS_MANY;
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
    daIce_c *ice = (daIce_c *) self->getOwner();
    dActor_c *actor = other->getOwner();

    int dir = !(ice->mPos.x <= actor->mPos.x);

    ProfileName otherProf = actor->mProfName;
    if (otherProf == fProf::ICE_ACTOR) {
        daIce_c *otherIce = (daIce_c *) actor;
        if (ice->getManager() == otherIce->getManager()) {
            return;
        }
    }

    if (otherProf == fProf::EN_TOGETEKKYU || otherProf == fProf::EN_BIG_TOGETEKKYU) {
        return;
    }

    if (ice->isDestroy()) {
        return;
    }

    if (other->mCcData.mAttack == CC_ATTACK_ICE_BREAK || other->mCcData.mStatus & BIT_FLAG(9)) {
        ice->breakReq(dir ^ 1, -1);
        self->mInfo |= CC_NO_HIT;
        return;
    }

    if (
        self->mCcData.mAttack == CC_ATTACK_SHELL &&
        (
            other->mCcData.mStatus & CC_STATUS_8 ||
            other->mCcData.mKind == CC_KIND_BALLOON
        )
    ) {
        ice->breakReq(dir ^ 1, ice->getPlrNo());
        self->mInfo |= CC_NO_HIT;
        return;
    }

    if (!(ice->m_608 & BIT_FLAG(0))) {
        return;
    }

    if (other->mCcData.mAttack == CC_ATTACK_YOSHI_BULLET || other->mCcData.mAttack == CC_ATTACK_YOSHI_FIRE) {
        ice->combobreakReq(dir ^ 1, actor);
        self->mInfo |= CC_NO_HIT;
        return;
    }

    if (other->mCcData.mAttack == CC_ATTACK_SHELL) {
        if (actor->checkCarried(nullptr)) {
            return;
        }

        bool isHit = actor->mKind == STAGE_ACTOR_ENEMY && ((dEn_c *) actor)->mFlags & 1;
        if (isHit) {
            return;
        }

        ice->combobreakReq(dir ^ 1, actor);
        self->mInfo |= CC_NO_HIT;
        return;
    }

    if (actor->mKind != STAGE_ACTOR_ENEMY && actor->mKind != STAGE_ACTOR_GENERIC) {
        return;
    }

    if (other->mCcData.mKind == CC_KIND_KILLER) {
        ice->breakReq(dir ^ 1, -1);
        self->mInfo |= CC_NO_HIT;
        return;
    }

    if (
        ice->isState(StateID_Carry) &&
        other->mCcData.mVsDamage & BIT_FLAG(CC_ATTACK_SHELL) &&
        actor->mProfName != fProf::EN_KANITAMA
    ) {
        ice->breakReq(dir ^ 1, ice->getPlrNo());
        self->mInfo |= CC_NO_HIT;
    }
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
        if (curr->mpOwner != nullptr && !(curr->mpOwner->mKind != STAGE_ACTOR_PLAYER && curr->mpOwner->mKind != STAGE_ACTOR_YOSHI)) {
            daPlBase_c *pl = (daPlBase_c *) curr->mpOwner;
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
        if (curr->mpOwner != nullptr && !(curr->mpOwner->mKind != STAGE_ACTOR_PLAYER && curr->mpOwner->mKind != STAGE_ACTOR_YOSHI)) {
            daPlBase_c *pl = (daPlBase_c *) curr->mpOwner;
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
        if (curr->mpOwner != nullptr && !(curr->mpOwner->mKind != STAGE_ACTOR_PLAYER && curr->mpOwner->mKind != STAGE_ACTOR_YOSHI)) {
            daPlBase_c *pl = (daPlBase_c *) curr->mpOwner;
            if (dir == DIR_LR_R) {
                if (pl->mPos.x >= mPos.x) {
                    if (ptr != nullptr && (pl->isNowBgCross(daPlBase_c::BGC_WALL_TOUCH_R_2) || pl->isNowBgCross(daPlBase_c::BGC_SIDE_LIMIT_R))) {
                        *ptr = 1;
                        if (checkPlHasami_Wall(pl, dir)) {
                            *ptr = 2;
                        }
                    }
                    res = true;
                }
            } else {
                if (pl->mPos.x <= mPos.x) {
                    if (ptr != nullptr && (pl->isNowBgCross(daPlBase_c::BGC_WALL_TOUCH_L_2) || pl->isNowBgCross(daPlBase_c::BGC_SIDE_LIMIT_L))) {
                        *ptr = 1;
                        if (checkPlHasami_Wall(pl, dir)) {
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

void daIce_c::initializeState_Freeze() {
    mCc.entry();
}

void daIce_c::finalizeState_Freeze() {
    m_59c = mInitialPos;
    enemyAttach();
}

void daIce_c::executeState_Freeze() {
    int plr = -1;
    if (checkPlHasami(&plr)) {
        breakReq(dGameCom::rndInt(2), plr);
    }
}

void daIce_c::initializeState_Hop() {
    dActor_c *parent = (dActor_c *) getConnectParent();

    mSpeed.x = parent->mSpeed.x;
    mSpeed.y = parent->mSpeed.y;
    mSpeed.z = 0.0f;
    mAccelY = -0.1875f;

    m_604 = 3;
    m_608 &= ~BIT_FLAG(1);

    mSensorFoot.mFlags |= 0x44;
    mSensorHead.mFlags |= 0x44;
    mSensorWall.mFlags |= 0x44;

    mPos.z = 1000.0f;
    m_23b = 1;
}

void daIce_c::finalizeState_Hop() {
    mSensorFoot.mFlags &= ~0x44;
    mSensorHead.mFlags &= ~0x44;
    mSensorWall.mFlags &= ~0x44;
}

void daIce_c::executeState_Hop() {
    calcSpeedY();
    posMove();
    int dir = !(mSpeed.x >= 0.0f);
    if (mSpeed.y <= 0.0f) {
        bool isFoot = mBc.isFoot();
        if (mBc.checkWall(nullptr)) {
            mSpeed.x = mSpeed.x * -0.5f;
        }
        if (mBc.checkFootEnm()) {
            if (!isFoot) {
                getManager()->set_2c(0);
                mSpeed.x = 0.0f;
                mCc.entry();
                m_608 |= BIT_FLAG(1);
                dAudio::g_pSndObjMap->startSound(SE_OBJ_PNGN_ICE_LAND, mPos, 0);
            }
            mSpeed.y = 0.0f;
        } else {
            if (checkPlAttach_Wall(dir, nullptr) || checkPlAttach_Foot(nullptr, nullptr)) {
                breakReq(dir ^ 1, getManager()->mPlrNo);
            }
        }
    }
}

void daIce_c::initializeState_Land() {
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mSensorWall.mFlags |= BIT_FLAG(30);
    mSensorHead.mFlags |= BIT_FLAG(31);
    mBgCtr.mFlags |= BIT_FLAG(11);
}

void daIce_c::finalizeState_Land() {
    mSensorWall.mFlags &= ~BIT_FLAG(30);
    mSensorHead.mFlags &= ~BIT_FLAG(31);
}

void daIce_c::executeState_Land() {
    float height = 0.0f;
    dBc_c::WATER_TYPE_e waterRes = dBc_c::checkWater(mPos.x, mPos.y, mLayer, &height);
    if (waterRes == dBc_c::WATER_CHECK_WATER) {
        if (height >= mPos.y + mCcSize.y + 8.0f) {
            getManager()->set_2c(1);
            getManager()->setNonMeltTime();
            changeState(StateID_Float);
            return;
        }
    } else if (waterRes == dBc_c::WATER_CHECK_YOGAN) {
        if (height >= mPos.y + 4.0f) {
            getManager()->set_2c(1);
            getManager()->setNonMeltTime();
            mCc.release();
            yoganEffect();
            getManager()->set_14(1);
            mSpeed.y = -12.0f;
            changeState(StateID_YoganDive);
            return;
        }
    }

    if (!mCc.isLinked()) {
        mCc.entry();
    }

    calcSpeedY();
    posMove();
    dBc_c::Flag_e isFootEnm = mBc.checkFootEnm();
    if (isFootEnm) {
        mPos += mBc.mPushForce;
        mSpeed.y = 0.0f;
    }
    if (!(mBc.mFlags & dBc_c::FLAG_15)) {
        mSensorHead.mFlags |= BIT_FLAG(31);
    }

    mBc.checkHead(isFootEnm);

    u8 dir = mPos.x - mBc.get_4c() < 0.0f;
    float dirs[] = { 1.0f, -1.0f };
    mBc.checkWallEnm(&dirs[dir]);
    mBc.checkWallEnm(&dirs[dir ^ 1]);

    if (mBc.mFlags & dBc_c::FLAG_15 && mBgCollFlags & BIT_FLAG(2)) {
        if (!someCarryCheck()) {
            mBgCollFlags &= ~BIT_FLAG(2);
        }
    }
    if (HasamareBgCheck()) {
        breakReq(dGameCom::rndInt(2), -1);
    } else {
        int plr = -1;
        if (checkPlHasami(&plr)) {
            breakReq(dGameCom::rndInt(2), plr);
        } else if (!isFootEnm) {
            mSpeed.y = 0.0f;
            changeState(StateID_Fall);
        }
    }
}

void daIce_c::initializeState_Cover() {}

void daIce_c::finalizeState_Cover() {}

void daIce_c::executeState_Cover() {
    dActor_c *parent = (dActor_c *) getConnectParent();

    mPos = parent->mPos - mPosDiff;

    int plr = -1;
    if (checkPlHasami(&plr)) {
        breakReq(dGameCom::rndInt(2), plr);
    }
}

void daIce_c::initializeState_Sink() {
    mSpeedF = -0.5f;
    mAccelF = 0.06f;
    mAngle.z = 0;
    mFloatTargetSpeed = 0.0f;
    mFloatAccel = std::fabs(mSpeedF) / 30.0f;
}

void daIce_c::finalizeState_Sink() {}

void daIce_c::executeState_Sink() {
    float prev = m_5f4;
    dBc_c::checkWater(mPos.x, mPos.y, mLayer, &m_5f4);

    if (m_5f4 - prev < 0.0f) {
        if (mPos.y >= getSmth()) {
            mPos.y = getSmth();
            changeState(StateID_FloatWait);
            return;
        }
    }

    calcFloatSpeed();
    posMove();

    getConnectParent(); // [Unused return value]
    getManager(); // [Unused return value]

    if (std::fabs(mSpeed.y) < mFloatAccel) {
        if (m_614 == ICE_TYPE_5) {
            changeState(StateID_TogeFloat);
        } else {
            changeState(StateID_Float);
        }
    } else if (ACTOR_PARAM(Param10) != 2) {
        float yPos = mPos.y;
        if (mBc.checkFoot()) {
            if  (mBc.mFlags & dBc_c::FLAG_21) {
                mPos.y = yPos;
            } else {
                int res = 0;
                if (checkPlAttach_Head(&res) && res != 0) {
                    floatbreakReq();
                }
            }
        }
    }
}

void daIce_c::initializeState_Float() {
    mFloatTargetSpeed = 0.7f;
    mFloatAccel = 0.0f;
    mSpeedF = 0.0f;
    mFloatAngle = 0;
    m_23b = 1;
}

void daIce_c::finalizeState_Float() {}

void daIce_c::executeState_Float() {
    calcFloatAngle();
    calcFloatSpeed();
    posMove();
    switch (m_23b) {
        default:
            break;
        case 1:
            mFloatAccel += 0.003f;
            if (mFloatAccel >= 0.025f) {
                mFloatAccel = 0.025f;
            }
            if (checkPlAttach_Head(nullptr)) {
                mFloatAccel = 0.05f;
                mFloatTargetSpeed = l_attach_float_speed[mWeightType];
                m_23b = 2;
            }
            break;
        case 2:
            int res = 0;
            if (!checkPlAttach_Head(&res)) {
                mFloatAccel = 0.0f;
                mFloatTargetSpeed = l_float_speed[mWeightType];
                m_23b = 1;
            } else if (res != 0) {
                m_5c0 = 2;
                changeState(StateID_PushSink);
                return;
            }
            break;
    }

    dBc_c::checkWater(mPos.x, mPos.y, mLayer, &m_5f4);

    if (mPos.y >= getSmth()) {
        changeState(StateID_FloatWait);
    } else {
        if (mBc.checkHead(0)) {
            floatbreakReq();
        }
    }
}

void daIce_c::initializeState_SmallFloat() {
    mFloatTargetSpeed = 0.5f;
    mFloatAccel = 0.0f;
    mSpeedF = 0.0f;
    mFloatAngle = 0;
    m_23b = 1;
}

void daIce_c::finalizeState_SmallFloat() {}

void daIce_c::executeState_SmallFloat() {
    calcFloatSpeed();
    posMove();
    switch (m_23b) {
        default:
            break;
        case 1:
            mFloatAccel += 0.002f;
            if (mFloatAccel >= 0.02f) {
                mFloatAccel = 0.02f;
            }
            if (checkPlAttach_Head(nullptr)) {
                mFloatAccel = 0.01f;
                mFloatTargetSpeed = l_attach_float_speed[mWeightType] * 0.75f;
                m_23b = 2;
            }
            break;
        case 2:
            int res = 0;
            if (!checkPlAttach_Head(&res)) {
                mFloatAccel = 0.0f;
                mFloatTargetSpeed = 0.5f;
                m_23b = 1;
            } else if (res != 0) {
                m_5c0 = 2;
                changeState(StateID_PushSink);
                return;
            }
            break;
    }

    float prev = m_5f4;
    dBc_c::checkWater(mPos.x, mPos.y, mLayer, &m_5f4);

    if (mPos.y >= getSmth()) {
        changeState(StateID_FloatWait);
    } else {
        if (m_5f4 > prev) {
            changeState(StateID_Float);
        } else if (mBc.checkHead(0)) {
            floatbreakReq();
        }
    }
}

void daIce_c::initializeState_FloatWait() {
    dBc_c::checkWater(mPos.x, mPos.y, mLayer, &m_5f4);
    mSpeed.x = 0.0f;
    m_5f0 = 0;
    m_5f8 = 0;
    mSensorHead.mFlags |= BIT_FLAG(7);
}

void daIce_c::finalizeState_FloatWait() {
    mSensorHead.mFlags &= ~BIT_FLAG(7);
}

void daIce_c::executeState_FloatWait() {
    mAngle.z.chaseAngle(0, 0x10);

    float prev = m_5f4;
    dBc_c::checkWater(mPos.x, mPos.y, mLayer, &m_5f4);

    float diff = m_5f4 - prev;
    if (diff > 0.0f) {
        m_5f8 = 2;
    } else if (diff < 0.0f) {
        m_5f8 = 3;
    } else {
        m_5f8 = 1;
    }

    float h = m_5f4 - mCcSize.y * 0.5f;
    switch (m_5f8) {
        case 2:
        case 3:
            sLib::addCalc(&mPos.y, h, 0.1f, 1.0f, 0.05f);
            if (diff < 0.0f) {
                float tmp = m_5f4 - mCcSize.y * 0.3f;
                if (mPos.y > tmp) {
                    mPos.y = tmp;
                }
            }
            break;
        case 1:
            m_5f0 += 0x280;
            mPos.y = h + nw4r::math::SinIdx(m_5f0) * 5.0f;
            break;
        default:
            break;
    }
    if (mBc.checkHead(0)) {
        floatbreakReq();
    } else {
        int res = 0;
        if (checkPlAttach_Head(&res) && res != 0) {
            m_5c0 = 2;
            changeState(StateID_PushSink);
        }
    }
}

void daIce_c::initializeState_PushSink() {
    const static float cs_float_speed[] = { -0.85f, -4.5f, -1.0f };

    mSpeed.set(0.0f, cs_float_speed[m_5c0], 0.0f);
    mAngle.z = 0;
}

void daIce_c::finalizeState_PushSink() {}

void daIce_c::executeState_PushSink() {
    float prev = m_5f4;
    dBc_c::checkWater(mPos.x, mPos.y, mLayer, &m_5f4);
    if (m_5f4 - prev < 0.0f && mPos.y >= getSmth()) {
        mPos.y = getSmth();
        changeState(StateID_FloatWait);
        mStateMgr.refreshState();
    } else {
        if (mSpeed.y < 0.0f) {
            float newVal;
            if (m_5c0 == 2) {
                newVal = sLib::addCalc(&mSpeed.y, 0.0f, 0.04f, 0.3f, 0.0f);
            } else {
                newVal = sLib::addCalc(&mSpeed.y, 0.0f, 0.07f, 0.5f, 0.0f);
            }
            if (newVal < 0.125f) {
                mSpeed.y = 0.0f;
            }
        }
        posMove();
        if (mSpeed.y >= 0.0f) {
            if (m_5c0 == 0 || m_5c0 == 2) {
                changeState(StateID_SmallFloat);
            } else {
                changeState(StateID_Float);
            }
        } else if (ACTOR_PARAM(Param10) != 2) {
            float prevY = mPos.y;
            if (mBc.checkFoot()) {
                if (mBc.mFlags & dBc_c::FLAG_21) {
                    mPos.y = prevY;
                } else {
                    int res = 0;
                    if (checkPlAttach_Head(&res) && res != 0) {
                        floatbreakReq();
                    }
                }
            }
        }
    }
}

void daIce_c::initializeState_TogeFloat() {
    getManager()->set_2c(0);
    mFloatTargetSpeed = 0.4f;
    mFloatAccel = 0.0f;
    mFloatAngle = 0;
    m_23b = 1;
}

void daIce_c::finalizeState_TogeFloat() {}

void daIce_c::executeState_TogeFloat() {
    getManager(); // [Unused return value]

    calcFloatSpeed();
    posMove();
    switch (m_23b) {
        default:
            break;
        case 1:
            mFloatAccel += 0.003f;
            if (mFloatAccel >= 0.03f) {
                mFloatAccel = 0.03f;
            }
            if (checkPlAttach_Head(nullptr)) {
                mFloatAccel = 0.03f;
                mFloatTargetSpeed = 0.3f;
                m_23b = 2;
            }
            break;
        case 2: {
            int res = 0;
            if (!checkPlAttach_Head(&res)) {
                mFloatAccel = 0.0f;
                mFloatTargetSpeed = l_toge_float_speed[mWeightType];
                m_23b = 1;
            } else if (res != 0) {
                mSpeedF = -0.1f;
                mFloatAccel = 0.0f;
            }
            break;
        }
    }
    dBc_c::checkWater(mPos.x, mPos.y, mLayer, &m_5f4);
    if (mPos.y >= getSmth()) {
        mPos.y = getSmth();
    }
    if (mBc.checkHead(0)) {
        floatbreakReq();
    } else if (m_5dc != 0) {
        changeState(StateID_ShakeFloat);
    }
}

void daIce_c::initializeState_ShakeFloat() {
    mShakeAngle3 = 0x300;
    mShakeAngle1 = 0x600;
    mShakeAngle2 = 0;
    m_23b = 1;
}

void daIce_c::finalizeState_ShakeFloat() {}

void daIce_c::executeState_ShakeFloat() {
    calcFloatSpeed();
    posMove();
    switch (m_23b) {
        default:
            break;
        case 1:
            mFloatAccel += 0.005f;
            if (mFloatAccel >= 0.05f) {
                mFloatAccel = 0.05f;
            }
            if (checkPlAttach_Head(nullptr)) {
                mFloatAccel = 0.05f;
                mFloatTargetSpeed = l_attach_float_speed[mWeightType];
                m_23b = 2;
            }
            break;
        case 2:
            int res = 0;
            if (!checkPlAttach_Head(&res)) {
                mFloatAccel = 0.0f;
                mFloatTargetSpeed = l_float_speed[mWeightType];
                m_23b = 1;
            } else if (res != 0) {
                mSpeedF = -0.1f;
                mFloatAccel = 0.0f;
            }
            break;
    }
    dBc_c::checkWater(mPos.x, mPos.y, mLayer, &m_5f4);
    if (mPos.y >= getSmth()) {
        mPos.y = getSmth();
    }
    if (mBc.checkHead(0)) {
        floatbreakReq();
    }
}

void daIce_c::initializeState_Air() {
    m_604 = 120;
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mCc.entry();
    mSensorFoot.mFlags |= BIT_FLAG(31);
}

void daIce_c::finalizeState_Air() {
    mSensorFoot.mFlags &= ~BIT_FLAG(31);
    m_5dc = 0;
}

void daIce_c::executeState_Air() {
    m_604--;
    if (m_604 == 0) {
        mSpeed.set(0.0f, 0.0f, 0.0f);
        changeState(StateID_Fall);
    } else if (m_604 == 40) {
        m_5dc = 1;
    }
    int plr = -1;
    if (checkPlHasami(&plr)) {
        breakReq(dGameCom::rndInt(2), plr);
    } else {
        u8 dir = mPos.x - mBc.get_4c() < 0.0f;
        float dirs[] = { 1.0f, -1.0f };
        mBc.checkWallEnm(&dirs[dir]);
        mBc.checkWallEnm(&dirs[dir ^ 1]);
        float prevY = mPos.y;
        mBc.checkFoot();
        mPos.y = prevY;
    }
}

void daIce_c::initializeState_Fall() {
    getManager(); // [Unused return value]

    m_5fc = mPos.y;
    if (getOldStateID() == StateID_Air) {
        mAccelY = -1.0f;
    } else {
        mAccelY = -0.1875f;
    }
}

void daIce_c::finalizeState_Fall() {
    mSensorWall.mFlags &= ~BIT_FLAG(30);
    getManager()->set_2c(0);
}

void daIce_c::executeState_Fall() {
    mCc.entry();
    calcSpeedY();
    posMove();
    getManager(); // [Unused return value]

    u8 dir = mPos.x - mBc.get_4c() < 0.0f;
    float dirs[] = { 1.0f, -1.0f };
    mBc.checkWallEnm(&dirs[dir]);
    mBc.checkWallEnm(&dirs[dir ^ 1]);
    if (mBc.checkFootEnm()) {
        float diff = std::fabs(mPos.y - m_5fc);
        dEn_c *parent = (dEn_c *) getConnectParent();
        if (parent->mFlags & BIT_FLAG(17) || diff >= 24.0f) {
            int dir = dGameCom::rndInt(2);
            dActorMng_c::m_instance->createUpCoin(getCenterPos(), dir, 1, 0);
            breakReq(dir ^ 1, -1);
        } else {
            if (diff >= 12.0f) {
                dAudio::g_pSndObjMap->startSound(SE_OBJ_PNGN_ICE_LAND, mPos, 0);
            }
            mSpeed.y = 0.0f;
            changeState(StateID_Land);
        }
    } else {
        float height = 0;
        dBc_c::WATER_TYPE_e waterRes = dBc_c::checkWater(mPos.x, mPos.y + mCcSize.y * 0.2f, mLayer, &height);
        if (waterRes == dBc_c::WATER_CHECK_WATER) {
            mSpeed.x = 0.0f;
            mVec3_c efPos(mPos.x, height, 6500.0f);
            waterSplashEffect(efPos, 1.0f);
            changeState(StateID_Dive);
        } else if (waterRes == dBc_c::WATER_CHECK_YOGAN) {
            mSpeed.x = 0.0f;
            yoganEffect();
            changeState(StateID_YoganDive);
        } else if (waterRes == dBc_c::WATER_CHECK_POISON) {
            mSpeed.x = 0.0f;
            poisonEffect();
            changeState(StateID_YoganDive);
        }
    }
}

void daIce_c::initializeState_Dive() {
    dIceMng_c *iceMng = getManager();
    iceMng->set_2c(1);
    iceMng->m_04 = iceMng->m_08;
    iceMng->stopMeltShake();

    m_5dc = 0;
    m_608 &= ~(BIT_FLAG(0) | BIT_FLAG(1));
    mBgCtr.mFlags = 0x408;
    mAccelF = 0.06f;
    mSpeedMax.x = 0.0f;
    mSpeed.y *= 0.8f;
    mAngle.z = 0;
    m_604 = 30;
}

void daIce_c::finalizeState_Dive() {
    mCc.mCcData.mAttack = CC_ATTACK_NONE;
    mCc.mCcData.mVsKind &= ~(BIT_FLAG(CC_KIND_BALLOON) | BIT_FLAG(CC_KIND_ITEM) | BIT_FLAG(CC_KIND_KILLER));
    mPlayerNo = -1;
}

void daIce_c::executeState_Dive() {
    float prevY = mPos.y;
    if (mSpeed.y < 0.0f && sLib::addCalc(&mSpeed.y, 0.0f, 0.085f, 0.5f, 0.0f) < 0.25f) {
        mSpeed.y = 0.0f;
    }
    calcSpeedX();
    posMove();
    m_604--;
    if (mSpeed.y >= 0.0f && std::fabs(mSpeed.x) < mAccelF) {
        mCc.mCcData.mAttack = CC_ATTACK_NONE;
        mCc.mCcData.mCallback = ccCallBack_Water;
        mSpeed.x = 0.0f;
        changeState(StateID_Float);
        mStateMgr.refreshState();
    } else {
        if (mBc.checkFoot()) {
            mPos.y = prevY;
        }
        if (mBc.checkWall(nullptr)) {
            breakReq(mDirection ^ 1, mPlayerNo);
        }
    }
}

void daIce_c::initializeState_YoganDive() {
    dIceMng_c *iceMng = getManager();
    iceMng->set_2c(1);
    iceMng->m_04 = iceMng->m_08;
    iceMng->stopMeltShake();
    m_5dc = 0;
    mBgCtr.mFlags = 8;
    m_608 &= ~(BIT_FLAG(0) | BIT_FLAG(1));
    mAccelF = 0.35f;
    mSpeedMax.x = 0.0f;
    mSpeed.y *= 0.25f;
    mPos.z = -2500.0f;
    mAngle.z = 0;
    m_604 = 30;
}

void daIce_c::finalizeState_YoganDive() {}

void daIce_c::executeState_YoganDive() {
    if (mSpeed.y < 0.0f && sLib::addCalc(&mSpeed.y, -0.33f, 0.3f, 0.75f, 0.0f) < 0.25f) {
        mSpeed.y = -0.33f;
    }
    calcSpeedX();
    posMove();
    float height = 0.0f;
    dBc_c::WATER_TYPE_e waterRes = dBc_c::checkWater(mPos.x, mPos.y, mLayer, &height);
    if (waterRes == dBc_c::WATER_CHECK_NONE && mPos.y + mCcSize.y < height) {
        dActor_c *parent = (dActor_c *) getConnectParent();
        if (parent != nullptr) {
            parent->deleteRequest();
        }
    } else {
        if (std::fabs(mSpeed.x) > 0.25f && mBc.checkWall(nullptr)) {
            breakReq(mDirection ^ 1, mPlayerNo);
        }
    }
}

void daIce_c::initializeState_Carry() {
    dIceMng_c *iceMng = getManager();
    iceMng->set_2c(1);
    iceMng->m_04 = iceMng->m_08;
    iceMng->stopMeltShake();
    m_5dc = 0;
    setCarryBc();
    mCc.mCcData.mAttack = CC_ATTACK_SHELL;
    mCc.mCcData.mVsKind &= ~BIT_FLAG(CC_KIND_ITEM);
    mCc.mCcData.mBase.mSize.set(mCcSize.x * 0.5f + 1.0f, mCcSize.y * 0.5f + 1.0f);
    mBgCtr.mFlags2 = 2;
    mSensorHead.mFlags |= BIT_FLAG(20) | BIT_FLAG(23);
    mBc.mOwningPlrNo = mCarriedByPlrNo;
}

void daIce_c::finalizeState_Carry() {
    daPyMng_c::getPlayer(mCarriedByPlrNo)->cancelCarry(this);
    mCarryingFlags &= ~(CARRY_RELEASE | CARRY_THROW);
    mCc.mCcData.mVsKind |= BIT_FLAG(CC_KIND_ITEM);
    setBc();
}

void daIce_c::executeState_Carry() {
    dAcPy_c *pl = daPyMng_c::getPlayer(mCarriedByPlrNo);
    u8 amiLine = 3;
    if (checkNetIn()) {
        if (mPos.z >= 0.0f) {
            amiLine = 1;
        } else {
            amiLine = 2;
        }
    }
    mCc.mAmiLine = amiLine;
    mBc.mAmiLine = amiLine;
    mBgCtr.mAmiLine = amiLine;
    if (mCarryingFlags & CARRY_RELEASE) {
        mDirection = mThrowDirection;
        mAccelY = -0.1875f;
        mPlayerNo = mCarriedByPlrNo;
        changeState(StateID_Throw);
        return;
    }

    mVec3_c delta = pl->mPosDelta;
    u8 dir = !(delta.x >= 0.0f);
    getConnectParent(); // [Unused return value]
    getManager(); // [Unused return value]
    if (pl->isStatus(daPlBase_c::STATUS_46) || delta.y > 0.0f) {
        mSpeed.y = 3.0f;
        bool headRes = mBc.checkHead(0);
        mSpeed.y = 0.0f;
        if (headRes) {
            mDirection = dir ^ 1;
            breakReq(dir, mCarriedByPlrNo);
            return;
        }
        int res = 0;
        if (checkPlAttach_Head(&res) && res != 0) {
            breakReq(dir, mCarriedByPlrNo);
            return;
        }
    }
    if (!pl->isStatus(daPlBase_c::STATUS_46)) {
        if (mBc.checkWall(&delta.x)) {
            mDirection = dir ^ 1;
            mPlayerNo = mCarriedByPlrNo;
            changeState(StateID_Drop);
        } else {
            int res = 0;
            if (checkPlAttach_Wall(dir, &res) && res != 0) {
                mDirection = dir ^ 1;
                mPlayerNo = mCarriedByPlrNo;
                changeState(StateID_Drop);
            }
        }
    }
}

void daIce_c::initializeState_Drop() {
    const static float cs_drop_speed_x[] = { 1.25f, -1.25f };

    mBgCtr.mpCarryActor = nullptr;
    mBgCtr.mFlags |= BIT_FLAG(12);
    mSpeed.set(cs_drop_speed_x[mDirection], 0.0f, 0.0f);
    if (mPos.z > 2000.0f) {
        mPos.z = 2000.0f;
    }
    mAccelY = -0.1875f;
}

void daIce_c::finalizeState_Drop() {
    dIceMng_c *iceMng = getManager();
    iceMng->set_2c(0);
    iceMng->m_04 = iceMng->m_08;
    iceMng->stopMeltShake();
    mCc.mCcData.mAttack = CC_ATTACK_NONE;
    mPlayerNo = -1;
}

void daIce_c::executeState_Drop() {
    calcSpeedY();
    posMove();
    getConnectParent(); // [Unused return value]
    dIceMng_c *iceMng = getManager();
    float height = 0.0f;
    dBc_c::WATER_TYPE_e waterRes = dBc_c::checkWater(mPos.x, mPos.y + mCcSize.y * 0.2f, mLayer, &height);
    if (waterRes == dBc_c::WATER_CHECK_WATER) {
        mCc.mCcData.mAttack = CC_ATTACK_NONE;
        mCc.mCcData.mCallback = ccCallBack_Water;
        iceMng->m_14 = 1;
        mVec3_c efPos(mPos.x, height, 6500.0f);
        waterSplashEffect(efPos, 1.0f);
        changeState(StateID_Dive);
    } else if (waterRes == dBc_c::WATER_CHECK_YOGAN) {
        mCc.release();
        yoganEffect();
        iceMng->m_14 = 1;
        changeState(StateID_YoganDive);
    } else if (waterRes == dBc_c::WATER_CHECK_POISON) {
        mCc.release();
        poisonEffect();
        iceMng->m_14 = 1;
        changeState(StateID_YoganDive);
    } else {
        if (mBc.checkFootEnm()) {
            breakReq(mDirection ^ 1, -1);
        } else if (mBc.checkWall(nullptr)) {
            mDirection ^= 1;
            mSpeed.x = -mSpeed.x;
        }
    }
}

void daIce_c::initializeState_Throw() {
    const static float cs_throw_speed[] = { 3.0f, -3.0f };
    const static float cs_throw_max_speed[] = { 4.0f, -4.0f };

    dAcPy_c *pl = daPyMng_c::getPlayer(mPlayerNo);

    float throwSpeed = pl->threeQuarterSpeedX() + cs_throw_speed[mDirection];
    float throwMaxSpeed = cs_throw_max_speed[mDirection];
    mAccelF = 0.0625f;
    mSpeedMax.x = throwMaxSpeed;

    // [Same deal as in dEnemyCarry_c::setThrowSpeed]
    if (throwSpeed < mSpeedMax.x) {
        throwSpeed = (throwSpeed > mSpeedMax.x) ? mSpeedMax.x : throwSpeed;
    } else if (throwSpeed > mSpeedMax.x) {
        throwSpeed = (throwSpeed < mSpeedMax.x) ? mSpeedMax.x : throwSpeed;
    }
    mSpeed.set(throwSpeed, -1.0f, 0.0f);

    clrComboCnt();
    mBgCtr.mFlags |= BIT_FLAG(12);
    mCc.mCcData.mAttack = CC_ATTACK_SHELL;
    mCc.mCcData.mVsKind |= BIT_FLAG(CC_KIND_BALLOON) | BIT_FLAG(CC_KIND_ITEM) | BIT_FLAG(CC_KIND_KILLER);
    mCc.mCcData.mVsDamage |= BIT_FLAG(CC_ATTACK_SHELL);
    mCc.mCcData.mVsDamage &= ~BIT_FLAG(CC_ATTACK_SPIN_LIFT_UP);
    mSensorHead.mFlags |= BIT_FLAG(18) | BIT_FLAG(19) | BIT_FLAG(20) | BIT_FLAG(23);
    mSensorFoot.mFlags |= BIT_FLAG(18) | BIT_FLAG(19);
    mSensorWall.mFlags |= BIT_FLAG(18) | BIT_FLAG(19) | BIT_FLAG(20) | BIT_FLAG(23);
    if (mPos.z > 2000.0f) {
        mPos.z = 2000.0f;
    }
    mBc.mOwningPlrNo = mPlayerNo;
}

void daIce_c::finalizeState_Throw() {
    mSensorHead.mFlags &= ~(BIT_FLAG(18) | BIT_FLAG(19) | BIT_FLAG(20) | BIT_FLAG(23));
    mSensorFoot.mFlags &= ~(BIT_FLAG(18) | BIT_FLAG(19));
    mSensorWall.mFlags &= ~(BIT_FLAG(18) | BIT_FLAG(19) | BIT_FLAG(20) | BIT_FLAG(23));
    mBc.mOwningPlrNo = -1;
    mBgCtr.mpCarryActor = nullptr;
}

void daIce_c::executeState_Throw() {
    calcSpeedX();
    calcSpeedY();
    posMove();
    dActor_c *parent = (dActor_c *) getConnectParent();
    getManager(); // [Unused return value]

    float height = 0.0f;
    dBc_c::WATER_TYPE_e waterRes = dBc_c::checkWater(mPos.x, mPos.y + mCcSize.y * 0.2f, mLayer, &height);
    if (waterRes == dBc_c::WATER_CHECK_WATER) {
        mVec3_c efPos(mPos.x, height, 6500.0f);
        waterSplashEffect(efPos, 1.0f);
        changeState(StateID_Dive);
    } else if (waterRes == dBc_c::WATER_CHECK_YOGAN) {
        mCc.release();
        yoganEffect();
        changeState(StateID_YoganDive);
    } else if (waterRes == dBc_c::WATER_CHECK_POISON) {
        mCc.release();
        poisonEffect();
        changeState(StateID_YoganDive);
    } else {
        bool isFoot = mBc.isFoot();
        if (mBc.checkFootEnm()) {
            if (!isFoot) {
                dAudio::SoundEffectID_t(SE_OBJ_PNGN_ICE_LAND).playMapSound(mPos, 0);
            }
            mSpeed.y = 0.0f;
            dAudio::SoundEffectID_t(SE_OBJ_PNGN_ICE_SLIDE).holdMapSound(mUniqueID, mPos, 0);
        }
        if (mBc.checkWall(nullptr)) {
            breakReq(mDirection ^ 1, mPlayerNo);
        }

        float loopX = dBgParameter_c::ms_Instance_p->getLoopScrollDispPosX(mPos.x);
        float endX = loopX + dBgParameter_c::ms_Instance_p->xSize();
        if (
            mPos.x < loopX - 64.0f ||
            mPos.x > endX + 64.0f
        ) {
            parent->deleteRequest();
        }
    }
}

void daIce_c::initializeState_Melt_Normal() {
    meltEffect();
    mBgCtr.release();
}

void daIce_c::finalizeState_Melt_Normal() {}

void daIce_c::executeState_Melt_Normal() {
    deleteRequest();
}

