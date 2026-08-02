#include <game/bases/d_enemy_toride_kokoopa.hpp>
#include <game/bases/d_en_fumi_check.hpp>
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_score_manager.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_a_player_manager.hpp>

const sCcDatNewF l_wand_cc = {
    0.0f, 0.0f,
    0.0f, 0.0f,
    CC_KIND_ENEMY,
    CC_ATTACK_NONE,
    BIT_FLAG(CC_KIND_PLAYER) | BIT_FLAG(CC_KIND_PLAYER_ATTACK) | BIT_FLAG(CC_KIND_YOSHI),
    0,
    CC_STATUS_NONE,
    dEnTorideKokoopa_c::wandCcCallback
};

STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, Jump_St);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, Jump);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, BigJump_St);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, BigJump);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, LandOn);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, AttackReady);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, AttackBegin);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, AttackSearch);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, Attack);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, AttackEnd);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, FumiHit);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, FireHit);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, StarHit);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, SlideHit);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, QuakeHit);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, ShellHit);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, ShellAtk_St);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, ShellAtk);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, ShellOut);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, DieFumi_St);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, DieFire);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, DieShell);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, DemoWait);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, DemoAwake);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, DemoAwake_Wait);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, DemoIkaku);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, DemoIkaku_Wait);
STATE_VIRTUAL_DEFINE(dEnTorideKokoopa_c, DemoEscape_St);

static const float l_shellatk_speed[2] = { 4.0f, -4.0f };

dEnTorideKokoopa_c::dEnTorideKokoopa_c() : m_6ec(nullptr), mJumpAnmInfo(nullptr),
        m_6f4(nullptr), m_6f8(nullptr), mDemoAnmInfo(nullptr), m_70c(BASE_ID_NULL),
        m_71c(0), mCurLookAngle(0), mLookAngle(0), mDrawFlags(DRAW_KOKOOPA), m_a68(1.0f), mRootJntIdx(-1),
        mShellJntIdx(-1), mEffectNames(nullptr) {

    mpVoicesInfo = nullptr;
    mFacePos = mPos;
    mFumiProc.mFumiCheck.m_00 = 5;

    mFumiProc.refresh(new KokoopaSpFumiCheck_c());
}

dEnTorideKokoopa_c::~dEnTorideKokoopa_c() {
    mWandCc.release();
}

int dEnTorideKokoopa_c::preExecute() {
    if (dEnBoss_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    if (!isState(StateID_FumiHit) && !isState(StateID_DieFumi_St)) {
        float scale = getDrawScale();
        mScale.x = scale;
        mScale.y = scale;
        mScale.z = scale;
    }

    moveAdjust_HIO();

    if (mpBossLife->mLife > 0) {
        sLib::chaseAngle(&mCurLookAngle, mLookAngle, 0x300);
    }

    return SUCCEEDED;
}

void dEnTorideKokoopa_c::postExecute(MAIN_STATE_e state) {
    if (state == SUCCESS) {
        if (mFireLoopEffectTimer > 0) {
            mFireLoopEffectTimer--;
            hitFireLoopEffect();
        }

        mWandCc.clear();
    }

    dEnBoss_c::postExecute(state);
}

int dEnTorideKokoopa_c::draw() {
    if (mDrawFlags & DRAW_KOKOOPA) {
        drawKokoopa();
    }

    if (mDrawFlags & DRAW_SHELL) {
        drawShell();
    }

    return 1;
}

void dEnTorideKokoopa_c::finalUpdate() {
    if (mDrawFlags & DRAW_KOKOOPA) {
        calcKokoopaMdl();

        mBlitzPos = calcBlitzPos();
        mFacePos = calcFacePos();

        calcCcData();
        calcRootJntPos();
    }

    if (mDrawFlags & DRAW_SHELL) {
        calcShellMdl();
        calcShellJntPos();
    }
}

mVec3_c dEnTorideKokoopa_c::calcBlitzPos() {
    mModel.getResMdl();

    mMtx_c mtx;
    mModel.getNodeWorldMtx(vf4d0(), &mtx);
    return mtx * getMagicStickEffectOffset();
}

bool dEnTorideKokoopa_c::isQuakeDamage() {
    if (!dEnBoss_c::isQuakeDamage()) {
        return false;
    }
    if (!mCc.mIsLinked) {
        return false;
    }
    if (mDrawFlags & DRAW_SHELL) {
        return false;
    }

    return !isState(StateID_ShellOut);
}

bool dEnTorideKokoopa_c::hitCallback_PenguinSlide(dCc_c *self, dCc_c *other) {
    daPlBase_c *player = (daPlBase_c *) other->mpOwner;
    daPlBase_c::DamageType_e dmg = daPlBase_c::DAMAGE_HIP_ATTACK;

    if (mDrawFlags & DRAW_SHELL) {
        dmg = daPlBase_c::DAMAGE_2;
    }

    player->setDamage(this, dmg);

    return true;
}

void dEnTorideKokoopa_c::setFumiDamage(dActor_c * other) {
    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mWandCc.release();
    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 5, playerNo);
    }

    fBase_c *blitz = getBlitz();
    if (blitz != nullptr) {
        blitz->deleteRequest();
    }

    changeState(StateID_FumiHit);
}


void dEnTorideKokoopa_c::setFumiDead(dActor_c * other) {
    u8 dir = !(mPos.x < other->mPos.x);

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mWandCc.release();

    mLookAngle = 0;
    mCurLookAngle = 0;

    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 6, playerNo);
    }

    fBase_c *blitz = getBlitz();
    if (blitz != nullptr) {
        blitz->deleteRequest();
    }

    mActorProperties &= ~8;

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        3.0f,
        -4.0f,
        -0.1875f,
        &StateID_DieFumi_St,
        -1,
        -1,
        dir,
        (u8) other->getPlrNo()
    };
}


void dEnTorideKokoopa_c::setFireDamage(dActor_c * other) {
    mFireLoopEffectTimer = 60;

    if (!mpBossLife->isDmgSection()) {
        return;
    }

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mWandCc.release();
    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 5, playerNo);
    }

    fBase_c *blitz = getBlitz();
    if (blitz != nullptr) {
        blitz->deleteRequest();
    }

    changeState(StateID_FireHit);
}


void dEnTorideKokoopa_c::setFireDead(dActor_c * other) {
    u8 dir = !(mPos.x < other->mPos.x);

    removeCc();
    mWandCc.release();

    mLookAngle = 0;
    mCurLookAngle = 0;

    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 6, playerNo);
    }

    fBase_c *blitz = getBlitz();
    if (blitz != nullptr) {
        blitz->deleteRequest();
    }

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    mFireLoopEffectTimer = 60;
    mActorProperties &= ~8;

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        3.0f,
        -4.0f,
        -0.1875f,
        &dEnBoss_c::StateID_DieFire,
        -1,
        -1,
        dir,
        (u8) other->getPlrNo()
    };
}


void dEnTorideKokoopa_c::setStarDamage(dActor_c * other) {
    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mWandCc.release();
    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 5, playerNo);
    }

    fBase_c *blitz = getBlitz();
    if (blitz != nullptr) {
        blitz->deleteRequest();
    }

    changeState(StateID_StarHit);
}


void dEnTorideKokoopa_c::setStarDead(dActor_c * other) {
    u8 dir = !(mPos.x < other->mPos.x);

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mWandCc.release();

    mLookAngle = 0;
    mCurLookAngle = 0;

    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 6, playerNo);
    }

    fBase_c *blitz = getBlitz();
    if (blitz != nullptr) {
        blitz->deleteRequest();
    }

    mActorProperties &= ~8;

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        3.0f,
        -4.0f,
        -0.1875f,
        &StateID_DieStar,
        -1,
        -1,
        dir,
        (u8) other->getPlrNo()
    };
}


void dEnTorideKokoopa_c::setQuakeDamage() {
    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mWandCc.release();

    dScoreMng_c::m_instance->UnKnownScoreSet(this, 5, 0.0f, 24.0f);

    fBase_c *blitz = getBlitz();
    if (blitz != nullptr) {
        blitz->deleteRequest();
    }

    changeState(StateID_QuakeHit);
}

void dEnTorideKokoopa_c::setQuakeDead() {
    u8 dir = getPl_LRflag(mPos);

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mWandCc.release();

    mLookAngle = 0;
    mCurLookAngle = 0;

    dScoreMng_c::m_instance->UnKnownScoreSet(this, 6, 0.0f, 24.0f);

    fBase_c *base = getBlitz();
    if (base != nullptr) {
        base->deleteRequest();
    }

    mActorProperties &= ~8;

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        3.0f,
        -4.0f,
        -0.1875f,
        &StateID_DieStar,
        -1,
        -1,
        dir,
        (u8) -1
    };
}


void dEnTorideKokoopa_c::setShellDamage(dActor_c * other) {
    if (!mpBossLife->isDmgSection()) {
        return;
    }

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mWandCc.release();
    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 5, playerNo);
    }

    fBase_c *blitz = getBlitz();
    if (blitz != nullptr) {
        blitz->deleteRequest();
    }

    changeState(StateID_ShellHit);
}


void dEnTorideKokoopa_c::setShellDead(dActor_c * other) {
    u8 dir = !(mPos.x < other->mPos.x);

    removeCc();
    mWandCc.release();

    mLookAngle = 0;
    mCurLookAngle = 0;

    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 6, playerNo);
    }

    fBase_c *blitz = getBlitz();
    if (blitz != nullptr) {
        blitz->deleteRequest();
    }

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    mActorProperties &= ~8;

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        3.0f,
        -4.0f,
        -0.1875f,
        &dEnBoss_c::StateID_DieShell,
        -1,
        -1,
        dir,
        (u8) other->getPlrNo()
    };
}


void dEnTorideKokoopa_c::damageProc() {
    if (mpBossLife->isDmgSection()) {
        damageLVo();
    } else {
        damageSVo();
    }

    if (mpBossLife->isTwoDamage()) {
        speedUp();
    }
}

void dEnTorideKokoopa_c::deadProc() {
    deadVo();

    if (dActorMng_c::m_instance->mpCurrentBossDemo) {
        dActorMng_c::m_instance->mpCurrentBossDemo->stopBGM();
    }
}

float dEnTorideKokoopa_c::calcJumpRate() {
    float jumpDist = getJumpDist();
    float rate = 1.0f;
    if (movelimitCheck(jumpDist + 4.0f)) {
        float f1 = std::fabs(mArenaEdgeOffset[mDirection] + dGameCom::getDispCenterX() - mPos.x);
        if (f1 < 16.0f) {
            rate = -1.0f;
        } else {
            rate = f1 / jumpDist;
        }
    }
    return rate;
}

bool dEnTorideKokoopa_c::movelimitCheck(float width) {
    float selfLimit = mPos.x + l_EnMuki[mDirection] * width;
    float dispCenterX = dGameCom::getDispCenterX();
    float arenaEdgeX = mArenaEdgeOffset[mDirection];
    float arenaLimit = arenaEdgeX + dispCenterX;
    bool rate = false;
    if (mDirection == DIR_LR_R) {
        if (selfLimit >= arenaLimit) {
            rate = true;
        }
    } else {
        if (selfLimit <= arenaLimit) {
            rate = true;
        }
    }
    return rate;
}

void dEnTorideKokoopa_c::moveRevise() {
    float offs = (mSpeed.x > 0.0f) ? 1.0f : -1.0f;
    u8 u1 = 0.9f - offs;
    if (mSpeed.x == 0.0f) {
        u1 = 0;
    }

    float f3 = mArenaEdgeOffset[u1] + dGameCom::getDispCenterX();

    if (mSpeed.x >= 0.0f) {
        if (mPos.x > f3 - 1.0f) {
            mPos.x = f3 - 1.0f;
        }
    } else {
        if (mPos.x < f3 + 1.0f) {
            mPos.x = f3 + 1.0f;
        }
    }
}

void dEnTorideKokoopa_c::wandCcCallback(dCc_c *self, dCc_c *other) {
    daPlBase_c *player = (daPlBase_c *) other->mpOwner;
    if (player->mKind == dActor_c::STAGE_ACTOR_PLAYER && !player->isNoDamage()) {
        player->setDamage(self->mpOwner, daPlBase_c::DAMAGE_DEFAULT);
    }
}

int dEnTorideKokoopa_c::calcAttackTarget() {
    int target = 0;

    for (int curr = 1; curr < PLAYER_COUNT; curr++) {
        dAcPy_c *pPrev = daPyMng_c::getPlayer(target);
        dAcPy_c *pNext = daPyMng_c::getPlayer(curr);

        if (pNext == nullptr || !daPyMng_c::checkPlayer(curr)) {
            continue;
        }

        if (pPrev == nullptr || !daPyMng_c::checkPlayer(target)) {
            target = curr;
            continue;
        }

        if (std::fabs(mPos.x - pNext->mPos.x) < std::fabs(mPos.x - pPrev->mPos.x)) {
            target = curr;
        }
    }

    return target;
}

BOOL dEnTorideKokoopa_c::lockonTurn() {
    dAcPy_c *player = daPyMng_c::getPlayer(mAttackTargetPlayerId);
    if (player == nullptr) {
        return false;
    }

    if (player->mPos.x >= mPos.x) {
        mDirection = DIR_LR_R;
    } else {
        mDirection = DIR_LR_L;
    }

    return calcDirAngle(getTurnSpeed());
}

void dEnTorideKokoopa_c::calcLookAngle() {
    dAcPy_c *player = daPyMng_c::getPlayer(mAttackTargetPlayerId);
    mLookAngle = cM::atan2s(player->mPos.y - mFacePos.y, std::fabs(player->mPos.x - mFacePos.x));

    if (mLookAngle < 0) {
        mLookAngle = 0;
    } else if (mLookAngle > 0x1800) {
        mLookAngle = 0x1800;
    }
}

void dEnTorideKokoopa_c::changeShell() {
    setShellCc();
    shellChangeEffect();
    mAngle.x = 0;
}

void dEnTorideKokoopa_c::changeKokoopa() {
    setKokoopaCc();
    mAngle.x = 0;
}

void dEnTorideKokoopa_c::setAtkCnt() {
    bool ok = true;

    if (!mpBossLife->isNonDamage() && (mpBossLife->isOneDamage() == false)) {  //< [== false is necessary to match]
        ok = false;
    }

    if (ok) {
        mAtkCnt = 1;
    } else {
        mAtkCnt = 2;
    }
}

u32 dEnTorideKokoopa_c::getTorideFunfareTime() {
    return 40;
}

s16 dEnTorideKokoopa_c::getTurnSpeed() {
    mpBossLife->isTwoDamage(); // ???

    return 0x800;
}

BOOL dEnTorideKokoopa_c::calcDirAngle(s16 speed) {
    return mAngle.y.chaseAngle(l_EnMuki[mDirection] * defaultDirAngle(), speed);
}

void dEnTorideKokoopa_c::blitzMove(dActor_c * other) {
    if (other == nullptr) {
        return;
    }

    calcKokoopaMdl();
    mBlitzPos = calcBlitzPos();
    other->mPos = mBlitzPos;
}

void DUMMY_ORDERING() {
    dEnTorideKokoopa_c *x;
    x->getDownTime();
    x->getFumiRecoverTime();
    x->getTenmetsuTime_Fire();
    x->getTenmetsuTime_Press();
}

void dEnTorideKokoopa_c::calcRootJntPos() {
    mModel.getNodeWorldMtxMultVecZero(mRootJntIdx, mRootJntPos);
    mRootJntPos.z = 5500.0f;
}

void dEnTorideKokoopa_c::calcShellJntPos() {
    mShellModel.getNodeWorldMtxMultVecZero(mShellJntIdx, mShellJntPos);
    mShellJntPos.z = 5500.0f;
}

bool dEnTorideKokoopa_c::isTorideBoss() {
    switch (mProfName) {
        case fProfile::EN_BOSS_KAMECK:
        case fProfile::EN_BOSS_LARRY:
        case fProfile::EN_BOSS_WENDY:
        case fProfile::EN_BOSS_IGGY:
        case fProfile::EN_BOSS_LEMMY:
        case fProfile::EN_BOSS_MORTON:
        case fProfile::EN_BOSS_ROY:
        case fProfile::EN_BOSS_LUDWIG:
            return true;
        case fProfile::EN_BOSS_CASTLE_LARRY:
        case fProfile::EN_BOSS_CASTLE_ROY:
        case fProfile::EN_BOSS_CASTLE_WENDY:
        case fProfile::EN_BOSS_CASTLE_IGGY:
        case fProfile::EN_BOSS_CASTLE_LEMMY:
        case fProfile::EN_BOSS_CASTLE_MORTON:
        case fProfile::EN_BOSS_CASTLE_LUDWIG:
            return false;
    }
    return false;
}

void dEnTorideKokoopa_c::jumpEffect() {
    mVec3_c pos;
    if (mEffectNames[1] != nullptr) {
        pos.set(mPos.x, mPos.y, 5500.0f);
        mEf::createEffect(mEffectNames[1], 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::jumpRootEffect() {
    mVec3_c pos;
    if (mEffectNames[7] != nullptr) {
        pos.x = mRootJntPos.x;
        pos.y = mRootJntPos.y;
        pos.z = mPos.z - 64.0f;

        mLevelEffects[0].createEffect(mEffectNames[7], 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::landonEffect() {
    mVec3_c pos;
    if (mEffectNames[1] != nullptr) {
        pos.set(mPos.x, mPos.y, 5500.0f);
        mEf::createEffect(mEffectNames[1], 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::shellLandonEffect() {
    mVec3_c pos;

    pos.x = mPos.x;
    pos.y = mPos.y;
    pos.z = 5500.0f;

    if (mEffectNames[8] != nullptr) {
        mEf::createEffect(mEffectNames[8], 0, &pos, nullptr, nullptr);
    }
    if (mEffectNames[9] != nullptr) {
        mEf::createEffect(mEffectNames[9], 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::hitFireLoopEffect() {
    mVec3_c pos;
    if (mEffectNames[3] != nullptr) {
        pos.x = mRootJntPos.x;
        pos.y = mRootJntPos.y;
        pos.z = 5500.0f;

        mLevelEffect.createEffect(mEffectNames[3], 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::hitFireDamageEffect() {
    mVec3_c pos;
    if (mEffectNames[5] != nullptr) {
        pos.x = mRootJntPos.x;
        pos.y = mRootJntPos.y;
        pos.z = 5500.0f;

        mLevelEffect2.createEffect(mEffectNames[5], 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::shellChangeEffect() {
    mVec3_c pos;
    if (mEffectNames[6] != nullptr) {
        pos.x = mShellJntPos.x;
        pos.y = mShellJntPos.y + getShellChangeEffectOffsetY();
        pos.z = 5500.0f;

        mEf::createEffect(mEffectNames[6], 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::shellBumMarEffect() {
    mVec3_c pos; //< unused
    if (mEffectNames[0] != nullptr) {
        pos.x = mShellJntPos.x;
        pos.y = mShellJntPos.y;
        pos.z = 5500.0f;

        mLevelEffects[0].createEffect(mEffectNames[0], 0, &mShellJntPos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::shellAtkEffect() {
    mVec3_c pos; //< unused
    pos.x = mShellJntPos.x;
    pos.y = mShellJntPos.y;
    pos.z = 5500.0f;

    if (mSpeed.x > 0.0f) {
        if (mEffectNames[10] != nullptr) {
            mLevelEffects[1].createEffect(mEffectNames[10], 0, &mShellJntPos, nullptr, nullptr);
        }
    } else if (mSpeed.x < 0.0f) {
        if (mEffectNames[10] != nullptr) { //< copy-paste error? should be 11 instead of 10
            mLevelEffects[1].createEffect(mEffectNames[11], 0, &mShellJntPos, nullptr, nullptr);
        }
    }
    if (std::fabs(mSpeed.x) < mAccelF * 0.5f) {
        if (mSpeedMax.x >= 0.0f) {
            if (mEffectNames[12] != nullptr) {
                mEf::createEffect(mEffectNames[12], 0, &mShellJntPos, nullptr, nullptr);
            }
        } else {
            if (mEffectNames[13] != nullptr) {
                mEf::createEffect(mEffectNames[13], 0, &mShellJntPos, nullptr, nullptr);
            }
        }
    }
    if (mEffectNames[14] != nullptr) {
        mLevelEffects[2].createEffect(mEffectNames[14], 0, &mShellJntPos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::downFallEffect() {
    mVec3_c pos;
    if (mEffectNames[18] != nullptr) {
        pos.x = mRootJntPos.x;
        pos.y = mRootJntPos.y;
        pos.z = mPos.z - 64.0f;

        mLevelEffects[0].createEffect(mEffectNames[18], 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::downLandOnEffect(float scale) {
    mVec3_c pos;
    pos.x = mPos.x;
    pos.y = mPos.y;
    pos.z = 5500.0f;

    mVec3_c scaleVec;
    scaleVec.x = scale;
    scaleVec.y = scale;
    scaleVec.z = scale;

    if (mEffectNames[8] != nullptr) {
        mEf::createEffect(mEffectNames[8], 0, &pos, nullptr, &scaleVec);
    }

    if (mEffectNames[9] != nullptr) {
        mEf::createEffect(mEffectNames[9], 0, &pos, nullptr, &scaleVec);
    }
}

void dEnTorideKokoopa_c::fumidmgEffect() {
    if (mEffectNames[2] != nullptr) {
        mVec3_c pos = getCenterPos();
        pos.z = 5500.0f;

        mEf::createEffect(mEffectNames[2], 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::fumideadEffect() {
    char *effectName;
    if (isTorideBoss()) {
        effectName = mEffectNames[15];
    } else {
        effectName = mEffectNames[16];
    }

    if (effectName != nullptr) {
        mVec3_c pos = getCenterPos();
        pos.z = 5500.0f;

        mEf::createEffect(effectName, 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::shellWallEffect() {
    static const s16 cs_ef_angle[] = {
        DEG_TO_ANGLE(-180), 0
    };

    if (mEffectNames[17] != nullptr) {
        u8 idx = !(mSpeed.x >= 0.0f);

        mVec3_c wall_pos;
        mBc.getWallOfs(&wall_pos, 0);

        mVec3_c pos(
            mPos.x,
            mPos.y + wall_pos.y,
            mPos.z
        );
        mVec3_c pos2(
            pos.x + l_EnMuki[mDirection] * 24.0f,
            pos.y,
            pos.z
        );
        float wallX;

        dBc_c::checkWall(&pos, &pos2, &wallX, mLayer, 1, nullptr);

        pos.x = wallX;
        pos.z = 5500.0f;

        mAng3_c angle(0, cs_ef_angle[idx], 0);

        mEf::createEffect(mEffectNames[17], 0, &pos, &angle, nullptr);
    }
}

void dEnTorideKokoopa_c::notice1Vo() {
    if (mDemoAnmInfo->mAwakeName == nullptr) {
        return;
    }

    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (
        mpVoicesInfo->mEntries[0].mSoundID != SE_VOC_NO_SOUND &&
        mAnmChr.checkFrame(mpVoicesInfo->mEntries[0].mAnmFrame)
    ) {
        mSndObject.startSound(mpVoicesInfo->mEntries[0].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::notice2Vo() {
    if (mDemoAnmInfo->mIkakuName == nullptr) {
        return;
    }

    if (mpVoicesInfo == nullptr) {
        return;
    }

    for (int i = 1; i <= 2; i++) {
        if (mpVoicesInfo->mEntries[i].mSoundID != SE_VOC_NO_SOUND && mAnmChr.checkFrame(mpVoicesInfo->mEntries[i].mAnmFrame)) {
            mSndObject.startSound(mpVoicesInfo->mEntries[i].mSoundID, m_d0, 0);
        }
    }
}

void dEnTorideKokoopa_c::wakeVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    for (int i = 3; i <= 4; i++) {
        if (mpVoicesInfo->mEntries[i].mSoundID != SE_VOC_NO_SOUND && mAnmChr.checkFrame(mpVoicesInfo->mEntries[i].mAnmFrame)) {
            mSndObject.startSound(mpVoicesInfo->mEntries[i].mSoundID, m_d0, 0);
        }
    }
}

void dEnTorideKokoopa_c::escJumpVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (mpVoicesInfo->mEntries[5].mSoundID != SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[5].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::magicShotVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (mpVoicesInfo->mEntries[6].mSoundID != SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[6].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::shellOutVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (mpVoicesInfo->mEntries[7].mSoundID != SE_VOC_NO_SOUND && mAnmChr.checkFrame(mpVoicesInfo->mEntries[7].mAnmFrame)) {
        mSndObject.startSound(mpVoicesInfo->mEntries[7].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::damageSVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (mpVoicesInfo->mEntries[8].mSoundID != SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[8].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::damageLVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (mpVoicesInfo->mEntries[9].mSoundID != SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[9].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::deadVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (mpVoicesInfo->mEntries[10].mSoundID != SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[10].mSoundID, m_d0, 0);
    } else if (mpVoicesInfo->mEntries[9].mSoundID != SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[9].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::loseFirstVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (mpVoicesInfo->mEntries[11].mSoundID != SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[11].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::loseSecondVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    for (int i = 12; i <= 14; i++) {
        if (mpVoicesInfo->mEntries[i].mSoundID != SE_VOC_NO_SOUND && mAnmChr.checkFrame(mpVoicesInfo->mEntries[i].mAnmFrame)) {
            mSndObject.startSound(mpVoicesInfo->mEntries[i].mSoundID, m_d0, 0);
        }
    }
}

bool dEnTorideKokoopa_c::checkDownJump() {
    float x = mPos.x;
    float y = mPos.y - 8.0f;

    for (int i = 0; i < 2; i++, y -= 16.0f) {
        u32 type = dBc_c::getUnitType(x, y, mLayer);

        if (type == 1 || type == 0x8000) {
            return true;
        }
    }

    return false;
}

bool dEnTorideKokoopa_c::isCreateBlitz() const {
    if (isState(StateID_AttackSearch)) {
        if (mAnmChr.checkFrame(getCreateBlitzFrm())) {
            return true;
        }
    }
    return false;
}

bool dEnTorideKokoopa_c::isShootBlitz() const {
    if (isState(StateID_Attack)) {
        float frame = getShootFrm();

        if (frame > mAnmChr.mFrameMax) {
            frame = mAnmChr.mFrameMax;
        }

        if (mAnmChr.checkFrame(frame)) {
            return true;
        }
    }

    return false;
}

void dEnTorideKokoopa_c::setBeginMoveState() {
    mNumJumps = dGameCom::rndInt(3) + 1;
    mAccelY = getJumpGravity();
    if (mNumJumps > 1) {
        changeState(StateID_Jump_St);
    } else {
        changeState(StateID_BigJump_St);
    }
}

void dEnTorideKokoopa_c::initializeState_Jump_St() {
    if (mJumpAnmInfo->mJumpStName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(mJumpAnmInfo->mJumpStName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }

    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    mSpeed.z = 0.0f;
    mDirection = getPl_LRflag(mPos);
}
void dEnTorideKokoopa_c::finalizeState_Jump_St() {}
void dEnTorideKokoopa_c::executeState_Jump_St() {
    mModel.play();
    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm() != 0) {
        mSpeed.y = 0.0f;
    }

    BOOL a = calcDirAngle(getTurnSpeed());
    if (mAnmChr.isStop() && a) {
        changeState(StateID_Jump);
    }
}

void dEnTorideKokoopa_c::initializeState_Jump() {
    if (mJumpAnmInfo->mJumpName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(mJumpAnmInfo->mJumpName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    bool ok = true;
    if (!mpBossLife->isNonDamage() && mpBossLife->isOneDamage() == false) {
        ok = false;
    }

    mVec2_c speed;
    if (ok) {
        speed = mJumpAnmInfo->mJumpSpeed0;
    } else {
        speed = mJumpAnmInfo->mJumpSpeed1;
    }

    float rate = l_EnMuki[mDirection] * calcJumpRate();
    mSpeed.x = rate * speed.x;
    mSpeed.y = speed.y;

    jumpEffect();
    jumpSE();
}
void dEnTorideKokoopa_c::finalizeState_Jump() {}
void dEnTorideKokoopa_c::executeState_Jump() {
    mModel.play();
    calcSpeedY();
    posMove();
    jumpRootEffect();
    moveRevise();
    calcDirAngle(0x400);
    if (mBc.checkFootEnm() != 0) {
        changeState(StateID_LandOn);
    }
}

void dEnTorideKokoopa_c::initializeState_BigJump_St() {
    const char *anmName = mJumpAnmInfo->mBigJumpStName;
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }

    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    mSpeed.z = 0.0f;
    mDirection = getPl_LRflag(mPos);
}
void dEnTorideKokoopa_c::finalizeState_BigJump_St() {}
void dEnTorideKokoopa_c::executeState_BigJump_St() {
    mModel.play();
    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm() != 0) {
        mSpeed.y = 0.0f;
    }

    BOOL a = calcDirAngle(getTurnSpeed());
    if (mAnmChr.isStop() && a) {
        changeState(StateID_BigJump);
    }
}

void dEnTorideKokoopa_c::initializeState_BigJump() {
    const char *anmName = mJumpAnmInfo->mBigJumpName;
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    bool ok = true;
    if (!mpBossLife->isNonDamage() && (mpBossLife->isOneDamage() == false)) {
        ok = false;
    }

    mVec2_c speed;
    if (ok) {
        speed = mJumpAnmInfo->mBigJumpSpeed0;
    } else {
        speed = mJumpAnmInfo->mBigJumpSpeed1;
    }

    float rate = l_EnMuki[mDirection] * calcJumpRate();
    mSpeed.x = rate * speed.x;
    mSpeed.y = speed.y;

    jumpEffect();
    jumpSE();
}
void dEnTorideKokoopa_c::finalizeState_BigJump() {}
void dEnTorideKokoopa_c::executeState_BigJump() {
    mModel.play();
    calcSpeedY();
    posMove();
    jumpRootEffect();
    moveRevise();
    calcDirAngle(0x400);
    if (mBc.checkFootEnm() != 0) {
        changeState(StateID_LandOn);
    }
}

void dEnTorideKokoopa_c::initializeState_LandOn() {
    const char *anmName = mJumpAnmInfo->mLandOnName;
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }

    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    mSpeed.z = 0.0f;
    landonEffect();
    landonSE();
}
void dEnTorideKokoopa_c::finalizeState_LandOn() {}
void dEnTorideKokoopa_c::executeState_LandOn() {
    mModel.play();
    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm() != 0) {
        mSpeed.y = 0.0f;
    }

    if (mAnmChr.isStop()) {
        mNumJumps--;
        if (mNumJumps > 1) {
            changeState(StateID_Jump_St);
        } else if (mNumJumps == 1) {
            changeState(StateID_BigJump_St);
        } else {
            changeState(StateID_AttackReady);
        }
    }
}

void dEnTorideKokoopa_c::initializeState_AttackReady() {
    if (m_6ec[1] != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6ec[1]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }

    mSpeed.x = 0.0f;
    mSpeed.y = 0.0f;
    mSpeed.z = 0.0f;
}
void dEnTorideKokoopa_c::finalizeState_AttackReady() {}
void dEnTorideKokoopa_c::executeState_AttackReady() {
    mModel.play();
    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm() != 0) {
        mSpeed.y = 0.0f;
    }

    mDirection = getPl_LRflag(mPos);

    if (calcDirAngle(getTurnSpeed())) {
        setAtkCnt();
        changeState(StateID_AttackBegin);
    }
}

void dEnTorideKokoopa_c::initializeState_AttackBegin() {
    const char *anmName = m_6f4[0];
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }
}
void dEnTorideKokoopa_c::finalizeState_AttackBegin() {}
void dEnTorideKokoopa_c::executeState_AttackBegin() {
    mModel.play();
    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm() != 0) {
        mSpeed.y = 0.0f;
    }

    mAttackTargetPlayerId = calcAttackTarget();
    lockonTurn();
    calcLookAngle();
    if (mAnmChr.isStop() || (m_6f4[0] == nullptr)) {
        changeState(StateID_AttackSearch);
    }
}


void dEnTorideKokoopa_c::initializeState_AttackSearch() {
    const char *anmName = m_6f4[1];
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }
    anmName = m_6f4[4];
    if (anmName != nullptr) {
        mAnmMatClr.setAnm(mModel, mResFile.GetResAnmClr(anmName), 1, m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmMatClr);
        mAnmMatClr.setFrame(0.0f, 1);
    }
    mWandCc.release();
    m_23b = 1;
}
void dEnTorideKokoopa_c::finalizeState_AttackSearch() {}
void dEnTorideKokoopa_c::executeState_AttackSearch() {
    mModel.play();
    if (m_6f4[4] != nullptr) {
        mAnmMatClr.play(1);
    }
    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm() != 0) {
        mSpeed.y = 0.0f;
    }

    mAttackTargetPlayerId = calcAttackTarget();
    int turnResult = lockonTurn();
    calcLookAngle();
    blitzchargeSE();
    switch (m_23b) {
        case 1: {
            if (isCreateBlitz()) {
                m_70c = createBlitz();
                if (mpBossLife->isTwoDamage() && (mAtkCnt == 1)) {
                    mAttackSearchTimer = getAtkSearch2ndTime();
                } else {
                    mAttackSearchTimer = ((float)getAtkSearchTime()) / m_a68;
                }
                m_23b = 2;
            }
            break;
        }
        case 2: {
            blitzMove(getBlitz());

            mAttackSearchTimer--;
            if (!(mAttackSearchTimer > 0) && (turnResult != 0)) {
                changeState(StateID_Attack);
            }
            break;
        }
    }
}

void dEnTorideKokoopa_c::initializeState_Attack() {
    const char *anmName = m_6f4[2];
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }
    mWandCc.set(this, (sCcDatNewF *) &l_wand_cc);
    mWandCc.entry();
    m_23b = 1;
}
void dEnTorideKokoopa_c::finalizeState_Attack() {}
void dEnTorideKokoopa_c::executeState_Attack() {
    mModel.play();
    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm() != 0) {
        mSpeed.y = 0.0f;
    }

    dActor_c *blitz = getBlitz();
    if (blitz == nullptr) {
        m_23b = 2;
    }

    switch (m_23b) {
        case 1: {
            blitzMove(blitz);
            if (isShootBlitz()) {
                blitz->mDirection = mDirection;
                calcWandCcData();
                setBlitzTarget();
                blitzShoot();
                magicShotVo();

                m_70c = BASE_ID_NULL;
                if (m_6f4[4] != 0) {
                    mAnmMatClr.setFrame(0.0f, 1);
                }
                m_23b = 2;
            }
            break;
        }
        case 2: {
            if (mAnmChr.isStop()) {
                mAtkCnt--;
                if (mAtkCnt > 0) {
                    changeState(StateID_AttackBegin);
                } else {
                    changeState(StateID_AttackEnd);
                }
            }
            break;
        }
    }
}

void dEnTorideKokoopa_c::initializeState_AttackEnd() {
    const char *anmName = m_6f4[3];
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }
    m_71c = 0;
    mTimer1 = getAtkEndTime();
    m_23b = 1;
}
void dEnTorideKokoopa_c::finalizeState_AttackEnd() {
    mWandCc.release();
}
void dEnTorideKokoopa_c::executeState_AttackEnd() {
    mModel.play();
    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm() != 0) {
        mSpeed.y = 0.0f;
    }

    if ((m_71c != 0) && beginDance()) {
        return;
    }

    switch (m_23b) {
        case 1: {
            if (mTimer1 == 0) {
                mTimer1 = getAtkEndTime_Wait();
                mLookAngle = 0;
                m_23b = 2;
            }
            break;
        }
        case 2: {
            if (mTimer1 == 0) {
                setBeginMoveState();
            }
            break;
        }
    }
}


void dEnTorideKokoopa_c::initializeState_FumiHit() {
    const char *anmName = m_6f8[1];
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mWandCc.release();
    mWandCc.release(); //< copy-paste error?
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mAngle.x = 0;
    mAngle.y = 0;
    mPressScale = getPressScale();
    mScale = mPressScale;
    mTimer1 = getPressTime();
    m_23b = 1;
}
void dEnTorideKokoopa_c::finalizeState_FumiHit() {}
void dEnTorideKokoopa_c::executeState_FumiHit() {
    mModel.play();
    mBc.checkFootEnm(); //< copy-paste error?

    float scale = getDrawScale();

    switch (m_23b) {
        case 1: {
            if (mTimer1 != 0) {
                break;
            }
            mVec3_c vec = mVec3_c(scale - mPressScale.x, scale - mPressScale.y, scale - mPressScale.z);
            int time = getFumiRecoverTime();
            if (time <= 0) {
                mRecoverScaleStep = vec;
            } else {
                mRecoverScaleStep = vec / time;
            }
            m_23b = 2;
            break;
        }
        case 2: {
            mScale += mRecoverScaleStep;
            if (mScale.y >= scale) {
                mScale.x = scale;
                mScale.y = scale;
                mScale.z = scale;

                changeState(StateID_ShellAtk_St);
                mStateMgr.refreshState();
            }
            break;
        }
    }
}

void dEnTorideKokoopa_c::initializeState_FireHit() {
    const char *anmName = m_6f8[1]; //< copy-paste typo?
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6f8[0]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mWandCc.release();
    mAccelY = -0.275f;
    mSpeed.set(0.0f, 6.5f, 0.0f);
    mActorProperties &= ~8;
    mAngle.x = 0;
}
void dEnTorideKokoopa_c::finalizeState_FireHit() {}
void dEnTorideKokoopa_c::executeState_FireHit() {
    mModel.play();
    calcSpeedY();
    posMove();

    hitFireDamageEffect();
    if (mSpeed.y <= 2.75f) {
        changeState(StateID_ShellAtk_St);
    }
}

void dEnTorideKokoopa_c::initializeState_StarHit() {
    const char *anmName = m_6f8[1]; //< copy-paste typo?
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6f8[0]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mWandCc.release();
    mAccelY = -0.275f;
    mSpeed.set(0.0f, 6.5f, 0.0f);
    mActorProperties &= ~8;
}
void dEnTorideKokoopa_c::finalizeState_StarHit() {}
void dEnTorideKokoopa_c::executeState_StarHit() {
    mModel.play();
    calcSpeedY();
    posMove();

    if (mSpeed.y <= 2.75f) {
        changeState(StateID_ShellAtk_St);
    }
}

void dEnTorideKokoopa_c::initializeState_QuakeHit() {
    initializeState_StarHit();
    mSpeed.set(0.0f, 5.25f, 0.0f);
}
void dEnTorideKokoopa_c::finalizeState_QuakeHit() {
    finalizeState_StarHit();
}
void dEnTorideKokoopa_c::executeState_QuakeHit() {
    executeState_StarHit();
}

void dEnTorideKokoopa_c::initializeState_SlideHit() {
    const char *anmName = m_6f8[1]; //< copy-paste typo?
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6f8[0]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mWandCc.release();
    mAccelY = -0.275f;
    mSpeed.set(0.0f, 6.5f, 0.0f);
    mActorProperties &= ~8;
}
void dEnTorideKokoopa_c::finalizeState_SlideHit() {}
void dEnTorideKokoopa_c::executeState_SlideHit() {
    mModel.play();
    calcSpeedY();
    posMove();

    if (mSpeed.y <= 2.75f) {
        changeState(StateID_ShellAtk_St);
    }
}

void dEnTorideKokoopa_c::initializeState_ShellHit() {
    const char *anmName = m_6f8[1]; //< copy-paste typo?
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6f8[0]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mWandCc.release();
    mAccelY = -0.275f;
    mSpeed.set(0.0f, 6.5f, 0.0f);
    mActorProperties &= ~8;
}
void dEnTorideKokoopa_c::finalizeState_ShellHit() {}
void dEnTorideKokoopa_c::executeState_ShellHit() {
    mModel.play();
    calcSpeedY();
    posMove();

    if (mSpeed.y <= 2.75f) {
        changeState(StateID_ShellAtk_St);
    }
}


void dEnTorideKokoopa_c::initializeState_ShellAtk_St() {
    if (mDrawFlags == DRAW_KOKOOPA) {
        const char *anmName = m_6f8[2];
        if (anmName != nullptr) {
            mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
            mModel.setAnm(mAnmChr, 4.0f);
            mAnmChr.setRate(1.0f);
        }

        anmName = m_6f8[4];
        if (anmName != nullptr) {
            mShellAnmChr.setAnm(mShellModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
            mShellModel.setAnm(mShellAnmChr, 0.0f);
            mShellAnmChr.setRate(1.0f);
        }
    }

    if (
        *mStateMgr.getOldStateID() != StateID_FireHit &&
        *mStateMgr.getOldStateID() != StateID_SlideHit &&
        *mStateMgr.getOldStateID() != StateID_StarHit
    ) {
        mAccelY = -0.275f;

        if (checkDownJump()) {
            mSpeed.set(0.0f, 6.25f, 0.0f);
        } else {
            mSpeed.set(0.0f, 5.25f, 0.0f);
        }

        mSpeedMax.y = -4.0f;
    }

    mShellRotStep = 0x200;
    mLookAngle = 0;
    mNumLandFramesLeft = 4;
    mDrawFlags |= DRAW_SHELL;
    mActorProperties &= ~8;
    shellinSE();
}
void dEnTorideKokoopa_c::finalizeState_ShellAtk_St() {
    reviveCc();
}
void dEnTorideKokoopa_c::executeState_ShellAtk_St() {
    const static float y_speeds[] = {
        0.0f, 1.5f, 2.75f, 4.0f
    }; ///< @unofficial
    mModel.play();
    mShellModel.play();

    if (mAnmChr.isStop() && (mDrawFlags != DRAW_SHELL)) {
        changeShell();
        mDrawFlags = DRAW_SHELL;
    }

    if (
        *mStateMgr.getOldStateID() == StateID_FireHit ||
        *mStateMgr.getOldStateID() == StateID_SlideHit
    ) {
        if (mSpeed.y >= 0.0f || mDrawFlags == DRAW_SHELL) {
            calcSpeedY();
            posMove();
        }
    } else {
        calcSpeedY();
        posMove();
    }

    if (mDrawFlags == DRAW_SHELL) {
        mShellRotStep += 0x200;
        if (mShellRotStep > 0x1800) {
            mShellRotStep = 0x1800;
        }
        mAngle.y += mShellRotStep;
        shellatkSE();
        shellBumMarEffect();
    } else if (*mStateMgr.getOldStateID() == StateID_FireHit) {
        hitFireDamageEffect();
    }

    mAngle.z = 0;
    if (mBc.checkFootEnm()) {
        shelllandonSE();
        shellLandonEffect();

        mNumLandFramesLeft--;
        if (mNumLandFramesLeft > 0) {
            mSpeed.y = y_speeds[mNumLandFramesLeft];
        } else {
            mSpeed.y = 0.0f;
            changeState(StateID_ShellAtk);
        }
    }
}

void dEnTorideKokoopa_c::initializeState_ShellAtk() {
    float arenaRight = dGameCom::getDispCenterX() + mArenaEdgeOffset[0];
    float arenaLeft = dGameCom::getDispCenterX() + mArenaEdgeOffset[1];
    float arenaMid = (arenaRight + arenaLeft) * 0.5f;

    if (mPos.x >= arenaMid) {
        mDirection = 1;
    } else {
        mDirection = 0;
    }

    mActorProperties &= ~0x200;
    mShellAtkRemNumSweeps = 5;
    mAccelF = 0.3f;
    mSpeed.x = l_shellatk_speed[mDirection];

    if (mPos.x - 32.0f < arenaLeft) {  // Close to left edge -> go right immediately
        mSpeedMax.x = l_shellatk_speed[mDirection];
        mShellAtkSweepStartXPos = arenaLeft + 32.0f;
        mShellAtkRemNumSweeps = 6;
    } else if (mPos.x + 32.0f > arenaRight) {  // Close to right edge -> go left immediately
        mSpeedMax.x = l_shellatk_speed[mDirection];
        mShellAtkSweepStartXPos = arenaRight - 32.0f;
        mShellAtkRemNumSweeps = 6;
    } else {
        mShellAtkSweepStartXPos = mPos.x;
        mSpeedMax.x = l_shellatk_speed[mDirection ^ 1];
    }
}
void dEnTorideKokoopa_c::finalizeState_ShellAtk() {
    mActorProperties |= 0x200;
}
void dEnTorideKokoopa_c::executeState_ShellAtk() {
    static const s16 shorts[] = { 910, -910 }; ///< @unofficial

    calcSpeedX();
    calcSpeedY();
    float prev_x_pos = mPos.x;
    posMove();
    if (mBc.checkFootEnm()) {
        mSpeed.y = 0.0f;
    }

    if (mSpeed.x >= 0.0f) {
        mDirection = 0;
    } else {
        mDirection = 1;
    }

    mAngle.y += mShellRotStep;
    s16 a = 0;
    if (mShellAtkRemNumSweeps > 1) {
        a = -shorts[mDirection];
    }
    mAngle.z.chaseAngle(a, 0x80);

    shellatkSE();
    shellBumMarEffect();
    shellAtkEffect();

    if (((prev_x_pos < mShellAtkSweepStartXPos) && (mPos.x >= mShellAtkSweepStartXPos)) || ((prev_x_pos > mShellAtkSweepStartXPos) && (mPos.x <= mShellAtkSweepStartXPos))) {
        // Completed sweep
        mShellAtkRemNumSweeps--;
        if (mShellAtkRemNumSweeps > 0) {
            mSpeedMax.x = -mSpeedMax.x;
        } else {
            mSpeed.x = 0.0f;
            mDirection = getPl_LRflag(mPos);
            mAngle.y = defaultDirAngle() * l_EnMuki[mDirection];
            mAngle.z = 0;

            changeState(StateID_ShellOut);
        }
    }
}


void dEnTorideKokoopa_c::initializeState_ShellOut() {
    const char *anmName = m_6f8[3];
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 1.0f);
        mAnmChr.setRate(1.0f);
    }

    anmName = m_6f8[5];
    if (anmName != nullptr) {
        mShellAnmChr.setAnm(mShellModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
        mShellModel.setAnm(mShellAnmChr, 1.0f);
        mShellAnmChr.setRate(1.0f);
    }

    mSpeedMax.x = 0.0f;
    mAccelF = 0.0f;
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mActorProperties |= 8;

    shelloutSE();
    shellOutVo();

    if (mpBossLife->isTwoDamage()) {
        SndSceneMgr::sInstance->fn_8019bd90(0x800);
    }

    m_23b = 1;
}
void dEnTorideKokoopa_c::finalizeState_ShellOut() {}
void dEnTorideKokoopa_c::executeState_ShellOut() {
    mModel.play();
    mShellModel.play();

    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm()) {
        mSpeed.y = 0.0f;
    }

    if (checkGetUp()) {
        getupSE();
    }
    shellOutVo();

    switch (m_23b) {
        case 1: {
            if (!(mDrawFlags & DRAW_KOKOOPA) && mAnmChr.checkFrame(getKokoopaOnFrm())) {
                mDrawFlags |= DRAW_KOKOOPA;
            }
            if ((mDrawFlags & DRAW_SHELL) && mAnmChr.checkFrame(getShellOffFrm())) {
                mDrawFlags &= ~DRAW_SHELL;
            }
            if (mDrawFlags == DRAW_KOKOOPA) {
                changeKokoopa();
                m_23b = 2;
            }
            break;
        }
        case 2: {
            if (mAnmChr.isStop()) {
                setBeginMoveState();
            }
            break;
        }
    }

}


void dEnTorideKokoopa_c::initializeState_DieFumi_St() {
    const char *anmName = m_6f8[1];
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 3.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();

    mSpeed.set(0.0f, 0.0f, 0.0f);
    mAngle.x = 0;
    mAngle.y = 0;
    mPressScale = getPressScale();
    mScale = mPressScale;
    mTimer1 = getPressTime();

    m_23b = 1;
}
void dEnTorideKokoopa_c::finalizeState_DieFumi_St() {}
void dEnTorideKokoopa_c::executeState_DieFumi_St() {
    mModel.play();
    mBc.checkFootEnm(); //< copy-paste error?

    float scale = getDrawScale();

    switch (m_23b) {
        case 1: {
            if (mTimer1 != 0) {
                break;
            }
            mVec3_c vec = mVec3_c(scale - mPressScale.x, scale - mPressScale.y, scale - mPressScale.z);
            int time = getFumiRecoverTime();
            if (time <= 0) {
                mRecoverScaleStep = vec;
            } else {
                mRecoverScaleStep = vec / time;
            }
            m_23b = 2;
            break;
        }
        case 2: {
            mScale += mRecoverScaleStep;
            if (mScale.y >= scale) {
                mScale.x = scale;
                mScale.y = scale;
                mScale.z = scale;

                changeState(StateID_DieFumi);
            }
            break;
        }
    }
}


void dEnTorideKokoopa_c::initializeState_DieFire() {
    dEnBoss_c::initializeState_DieFire();
    mTimer1 = 60;
}
void dEnTorideKokoopa_c::finalizeState_DieFire() {}
void dEnTorideKokoopa_c::executeState_DieFire() {
    if (mTimer1 != 0) {
        hitFireDamageEffect();
    }
    dEnBoss_c::executeState_DieFire();
}

void dEnTorideKokoopa_c::initializeState_DieShell() {
    dEnBoss_c::initializeState_DieShell();
    mTimer1 = 60;
}
void dEnTorideKokoopa_c::finalizeState_DieShell() {}
void dEnTorideKokoopa_c::executeState_DieShell() {
    if (mTimer1 != 0) {
        hitShellDamageEffect();
    }
    dEnBoss_c::executeState_DieShell();
}


void dEnTorideKokoopa_c::initializeState_DemoWait() {
    if ((mDemoAnmInfo != nullptr) && (mDemoAnmInfo->mWaitName != nullptr)) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(mDemoAnmInfo->mWaitName), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    mSpeed.set(0.0f, 0.0f, 0.0f);
    mDirection = getPl_LRflag(mPos);
    mAngle.y = l_EnMuki[mDirection] * defaultDirAngle();

    mDrawFlags = DRAW_KOKOOPA;
}
void dEnTorideKokoopa_c::finalizeState_DemoWait() {}
void dEnTorideKokoopa_c::executeState_DemoWait() {
    mModel.play();

    calcSpeedY();
    posMove();
    if (mBc.checkFootEnm()) {
        mSpeed.y = 0.0f;
    }
}

void dEnTorideKokoopa_c::initializeState_DemoAwake() {
    const char *anmName = mDemoAnmInfo->mAwakeName;
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 5.0f);
        mAnmChr.setRate(1.0f);

        mResAnmTexPat = mResFile.GetResAnmTexPat(mDemoAnmInfo->mAwakeName);
        mAnmTexPat.setAnm(mModel, mResAnmTexPat, 0, m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmTexPat, 0.0f);
        mAnmTexPat.setRate(1.0f, 0);
    }

    notice1Vo();
}
void dEnTorideKokoopa_c::finalizeState_DemoAwake() {}
void dEnTorideKokoopa_c::executeState_DemoAwake() {
    mModel.play();
    mAnmTexPat.play();

    awakeSE();

    calcSpeedY();
    posMove();

    notice1Vo();

    if (mBc.checkFootEnm()) {
        mSpeed.y = 0.0f;
    }

    if (mAnmChr.isStop()) {
        changeState(StateID_DemoAwake_Wait);
    }
}


void dEnTorideKokoopa_c::initializeState_DemoAwake_Wait() {
    const char *anmName = mDemoAnmInfo->mAwakeWaitName;
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);

        mResAnmTexPat = mResFile.GetResAnmTexPat(mDemoAnmInfo->mAwakeWaitName);
        mAnmTexPat.setAnm(mModel, mResAnmTexPat, 0, m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmTexPat, 0.0f);
        mAnmTexPat.setRate(1.0f, 0);
    }
}
void dEnTorideKokoopa_c::finalizeState_DemoAwake_Wait() {}
void dEnTorideKokoopa_c::executeState_DemoAwake_Wait() {
    mModel.play();
    mAnmTexPat.play();

    calcSpeedY();
    posMove();

    if (mBc.checkFootEnm()) {
        mSpeed.y = 0.0f;
    }
}


void dEnTorideKokoopa_c::initializeState_DemoIkaku() {
    const char *anmName = mDemoAnmInfo->mIkakuName;
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);

        mResAnmTexPat = mResFile.GetResAnmTexPat(mDemoAnmInfo->mIkakuName);
        mAnmTexPat.setAnm(mModel, mResAnmTexPat, 0, m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmTexPat, 0.0f);
        mAnmTexPat.setRate(1.0f, 0);
    }
    notice2Vo();
}
void dEnTorideKokoopa_c::finalizeState_DemoIkaku() {}
void dEnTorideKokoopa_c::executeState_DemoIkaku() {
    mModel.play();
    mAnmTexPat.play();

    ikakuSE();
    ikakuEffect();
    notice2Vo();

    calcSpeedY();
    posMove();

    if (mBc.checkFootEnm()) {
        mSpeed.y = 0.0f;
    }

    if (mAnmChr.isStop()) {
        changeState(StateID_DemoIkaku_Wait);
    }
}

void dEnTorideKokoopa_c::initializeState_DemoIkaku_Wait() {
    const char *anmName = mDemoAnmInfo->mIkakuWaitName;
    if (anmName != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anmName), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);

        mResAnmTexPat = mResFile.GetResAnmTexPat(mDemoAnmInfo->mIkakuWaitName);
        mAnmTexPat.setAnm(mModel, mResAnmTexPat, 0, m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmTexPat, 0.0f);
        mAnmTexPat.setRate(1.0f, 0);
    }
}
void dEnTorideKokoopa_c::finalizeState_DemoIkaku_Wait() {}
void dEnTorideKokoopa_c::executeState_DemoIkaku_Wait() {
    mModel.play();
    mAnmTexPat.play();

    calcSpeedY();
    posMove();

    if (mBc.checkFootEnm()) {
        mSpeed.y = 0.0f;
    }
}

void dEnTorideKokoopa_c::initializeState_DemoEscape_St() {}
void dEnTorideKokoopa_c::finalizeState_DemoEscape_St() {}
void dEnTorideKokoopa_c::executeState_DemoEscape_St() {}


void dEnTorideKokoopa_c::setBattleReady() {
    setAtkCnt();
    changeState(StateID_AttackBegin);
}

void dEnTorideKokoopa_c::tenmetsuFin() {
    if (mAnmMatClr.children[0].getObj()) {
        mAnmMatClr.setFrame(0.0f, 0);
    }
}

void dEnTorideKokoopa_c::tenmetsuProc() {
    mAnmMatClr.play(0);
}
