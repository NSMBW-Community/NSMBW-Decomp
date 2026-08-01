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

dEnTorideKokoopa_c::dEnTorideKokoopa_c() : m_6ec(nullptr), mJumpAnmNames(nullptr),
        m_6f4(nullptr), m_6f8(nullptr), m_6fc(nullptr), m_70c(BASE_ID_NULL),
        m_71c(0), m_72c(0), mLookAngle(0), m_730(1), m_a68(1.0f), m_a6c(-1),
        m_a70(-1), mEffectNames(nullptr) {

    mpVoicesInfo = nullptr;
    mFacePos = mPos;
    mFumiProc.mFumiCheck.m_00 = 5;

    mFumiProc.refresh(new KokoopaSpFumiCheck_c());
}

dEnTorideKokoopa_c::~dEnTorideKokoopa_c() {
    mCc2.release();
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
        sLib::chaseAngle(&m_72c, mLookAngle, 0x300);
    }

    return SUCCEEDED;
}

void dEnTorideKokoopa_c::postExecute(MAIN_STATE_e state) {
    if (state == SUCCESS) {
        if (m_7e8 > 0) {
            m_7e8--;
            hitFireLoopEffect();
        }

        mCc2.clear();
    }

    dEnBoss_c::postExecute(state);
}

int dEnTorideKokoopa_c::draw() {
    if (m_730 & 1) {
        drawKokoopa();
    }

    if (m_730 & 2) {
        drawShell();
    }

    return 1;
}

void dEnTorideKokoopa_c::finalUpdate() {
    if (m_730 & 1) {
        calcKokoopaMdl();

        mBlitzPos = calcBlitzPos();
        mFacePos = calcFacePos();

        calcCcData();
        calcRootJntPos();
    }

    if (m_730 & 2) {
        calcShellMdl();
        calcShellJntPos();
    }
}


mVec3_c dEnTorideKokoopa_c::calcBlitzPos() {
    mVec3_c ret;
    nw4r::math::MTX34 mtx;

    mModel.getResMdl(); // [???]

    mModel.getNodeWorldMtx((ulong) vf4d0(), &mtx);
    mVec3_c offset = getMagicStickEffectOffset();
    PSMTXMultVec(mtx, offset, ret);

    return ret;
}

mVec3_c dEnTorideKokoopa_c::getMagicStickEffectOffset() const {
    return mVec3_c(0.0f, 0.0f, 18.0f);
}

bool dEnTorideKokoopa_c::isQuakeDamage() {
    if (!dEnBoss_c::isQuakeDamage()) {
        return false;
    }
    if (!mCc.mIsLinked) {
        return false;
    }
    if (m_730 & 2) {
        return false;
    }

    return !isState(StateID_ShellOut);
}

bool dEnTorideKokoopa_c::hitCallback_PenguinSlide(dCc_c *self, dCc_c *other) {
    daPlBase_c *player = (daPlBase_c *)other->mpOwner;
    daPlBase_c::DamageType_e dmg = daPlBase_c::DAMAGE_HIP_ATTACK;

    if (m_730 & 2) {
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
    mCc2.release();
    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 5, playerNo);
    }

    fBase_c *x;
    if (m_70c == BASE_ID_NULL) {
        x = nullptr;
    } else {
        x = fManager_c::searchBaseByID(m_70c);
    }

    if (x) {
        x->deleteRequest();
    }

    changeState(StateID_FumiHit);
}


void dEnTorideKokoopa_c::setFumiDead(dActor_c * other) {
    u8 dir = !(mPos.x < other->mPos.x);

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mCc2.release();

    mLookAngle = 0;
    m_72c = 0;

    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 6, playerNo);
    }

    fBase_c *x;
    if (m_70c == BASE_ID_NULL) {
        x = nullptr;
    } else {
        x = fManager_c::searchBaseByID(m_70c);
    }

    if (x) {
        x->deleteRequest();
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
    m_7e8 = 60;

    if (!mpBossLife->isDmgSection()) {
        return;
    }

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mCc2.release();
    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 5, playerNo);
    }

    fBase_c *x;
    if (m_70c == BASE_ID_NULL) {
        x = nullptr;
    } else {
        x = fManager_c::searchBaseByID(m_70c);
    }

    if (x) {
        x->deleteRequest();
    }

    changeState(StateID_FumiHit);
}


void dEnTorideKokoopa_c::setFireDead(dActor_c * other) {
    u8 dir = !(mPos.x < other->mPos.x);

    removeCc();
    mCc2.release();

    mLookAngle = 0;
    m_72c = 0;

    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 6, playerNo);
    }

    fBase_c *x;
    if (m_70c == BASE_ID_NULL) {
        x = nullptr;
    } else {
        x = fManager_c::searchBaseByID(m_70c);
    }

    if (x) {
        x->deleteRequest();
    }

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    m_7e8 = 60;
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
    mCc2.release();
    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 5, playerNo);
    }

    fBase_c *x;
    if (m_70c == BASE_ID_NULL) {
        x = nullptr;
    } else {
        x = fManager_c::searchBaseByID(m_70c);
    }

    if (x) {
        x->deleteRequest();
    }

    changeState(StateID_FumiHit);
}


void dEnTorideKokoopa_c::setStarDead(dActor_c * other) {
    u8 dir = !(mPos.x < other->mPos.x);

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mCc2.release();

    mLookAngle = 0;
    m_72c = 0;

    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 6, playerNo);
    }

    fBase_c *x;
    if (m_70c == BASE_ID_NULL) {
        x = nullptr;
    } else {
        x = fManager_c::searchBaseByID(m_70c);
    }

    if (x) {
        x->deleteRequest();
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
    mCc2.release();

    dScoreMng_c::m_instance->UnKnownScoreSet(this, 5);

    fBase_c *x;
    if (m_70c == BASE_ID_NULL) {
        x = nullptr;
    } else {
        x = fManager_c::searchBaseByID(m_70c);
    }

    if (x) {
        x->deleteRequest();
    }

    changeState(StateID_FumiHit);
}


void dEnTorideKokoopa_c::setQuakeDead() {
    u8 dir = getPl_LRflag(mPos);

    if (mAnmMatClr.children[1].getObj()) {
        mAnmMatClr.setFrame(0.0f, 1);
    }

    removeCc();
    mCc2.release();

    mLookAngle = 0;
    m_72c = 0;

    dScoreMng_c::m_instance->UnKnownScoreSet(this, 6, 0.0f, 24.0f);

    fBase_c *base;
    if (m_70c == BASE_ID_NULL) {
        base = nullptr;
    } else {
        base = fManager_c::searchBaseByID(m_70c);
    }
    if (base) {
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
    mCc2.release();
    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 5, playerNo);
    }

    fBase_c *x;
    if (m_70c == BASE_ID_NULL) {
        x = nullptr;
    } else {
        x = fManager_c::searchBaseByID(m_70c);
    }

    if (x) {
        x->deleteRequest();
    }

    changeState(StateID_FumiHit);
}


void dEnTorideKokoopa_c::setShellDead(dActor_c * other) {
    u8 dir = !(mPos.x < other->mPos.x);

    removeCc();
    mCc2.release();

    mLookAngle = 0;
    m_72c = 0;

    int playerNo = other->getPlrNo();
    if (playerNo >= 0 && playerNo < PLAYER_COUNT) {
        dScoreMng_c::m_instance->ScoreSet(this, 6, playerNo);
    }

    fBase_c *x;
    if (m_70c == BASE_ID_NULL) {
        x = nullptr;
    } else {
        x = fManager_c::searchBaseByID(m_70c);
    }

    if (x) {
        x->deleteRequest();
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
    float jump_dist = getJumpDist();
    float ret = 1.0f;
    if (movelimitCheck(jump_dist + 4.0f)) {
        float f1 = std::fabs(m_7dc[mDirection] + dGameCom::getDispCenterX() - mPos.x);
        if (f1 < 16.0f) {
            ret = -1.0f;
        } else {
            ret = f1 / jump_dist;
        }
    }
    return ret;
}

bool dEnTorideKokoopa_c::movelimitCheck(float offset) {
    float f1 = mPos.x + l_EnMuki[mDirection] * offset;
    float f0 = dGameCom::getDispCenterX();
    float f2 = m_7dc[mDirection];
    f2 += f0;
    bool ret = false;
    if (mDirection == 0) {
        if (f1 >= f2) {
            ret = true;
        }
    } else if (f1 <= f2) {
        ret = true;
    }
    return ret;
}

void dEnTorideKokoopa_c::moveRevise() {
    float f2;
    if (mSpeed.x > 0.0f) {
        f2 = 1.0f;
    } else {
        f2 = -1.0f;
    }

    u8 u1 = (0.9f - f2);
    if (mSpeed.x == 0) {
        u1 = 0;
    }

    float f3 = m_7dc[u1] + dGameCom::getDispCenterX();

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
    daPlBase_c *player = (daPlBase_c *)other->mpOwner;
    if ((player->mKind == dActor_c::STAGE_ACTOR_PLAYER) && !player->isNoDamage()) {
        player->setDamage(self->mpOwner, daPlBase_c::DAMAGE_DEFAULT);
    }
}

int dEnTorideKokoopa_c::calcAttackTarget() {
    int j = 0;

    for (int i = 1; i < 4; i++) {
        dAcPy_c* p2 = daPyMng_c::getPlayer(j);
        dAcPy_c* p3 = daPyMng_c::getPlayer(i);

        if (!(p3 && (daPyMng_c::mActPlayerInfo & (1 << (i & 0xFF))))) {
            continue;
        }

        if (!(p2 && (daPyMng_c::mActPlayerInfo & (1 << (j & 0xFF))))) {
            j = i;
            continue;
        }

        float f1 = mPos.x;
        if (std::fabs(f1 - p3->mPos.x) < std::fabs(f1 - p2->mPos.x)) {
            j = i;
        }
    }

    return j;
}

BOOL dEnTorideKokoopa_c::lockonTurn() {
    dAcPy_c *player = daPyMng_c::getPlayer(m_700);
    if (player == nullptr) {
        return false;
    }

    if (player->mPos.x >= mPos.x) {
        mDirection = 0;
    } else {
        mDirection = 1;
    }

    return calcDirAngle(getTurnSpeed());
}

void dEnTorideKokoopa_c::calcLookAngle() {
    dAcPy_c *player = daPyMng_c::getPlayer(m_700);
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
    return sLib::chaseAngle((s16*)&mAngle.y, l_EnMuki[mDirection] * defaultDirAngle(), speed);
}

void dEnTorideKokoopa_c::blitzMove(dActor_c * other) {
    if (other == nullptr) {
        return;
    }

    calcKokoopaMdl();
    mBlitzPos = calcBlitzPos();
    other->mPos = mBlitzPos;
}

void dEnTorideKokoopa_c::calcRootJntPos() {
    mModel.getNodeWorldMtxMultVecZero(m_a6c, mRootJntPos);
    mRootJntPos.z = 5500.0f;
}

void dEnTorideKokoopa_c::calcShellJntPos() {
    mModel2.getNodeWorldMtxMultVecZero(m_a70, mShellJntPos);
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

    mVec3_c scale_vec;
    scale_vec.x = scale;
    scale_vec.y = scale;
    scale_vec.z = scale;

    if (mEffectNames[8] != nullptr) {
        mEf::createEffect(mEffectNames[8], 0, &pos, nullptr, &scale_vec);
    }

    if (mEffectNames[9] != nullptr) {
        mEf::createEffect(mEffectNames[9], 0, &pos, nullptr, &scale_vec);
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
    char *effect_name;
    if (isTorideBoss()) {
        effect_name = mEffectNames[15];
    } else {
        effect_name = mEffectNames[16];
    }

    if (effect_name != nullptr) {
        mVec3_c pos = getCenterPos();
        pos.z = 5500.0f;

        mEf::createEffect(effect_name, 0, &pos, nullptr, nullptr);
    }
}

void dEnTorideKokoopa_c::shellWallEffect() {
    static const s16 cs_ef_angle[] = {
        -0x8000, 0
    };

    if (mEffectNames[17] != nullptr) {
        mVec3_c wall_pos;
        mVec3_c pos;
        mVec3_c pos2;
        float f48;
        u8 idx = !(mSpeed.x >= 0.0f);
        mAng3_c angle;

        mBc.getWallOfs(&wall_pos, 0);

        pos = mPos;

        pos2.y = mPos.y + wall_pos.y;
        pos2.z = mPos.z;
        pos2.x = mPos.x + l_EnMuki[mDirection] * 24.0f;

        dBc_c::checkWall(&pos, &pos2, &f48, mLayer, 1, nullptr);

        pos.x = f48;
        pos.z = 5500.0f;
        angle.x = 0;
        angle.y = cs_ef_angle[idx];
        angle.z = 0;

        mEf::createEffect(mEffectNames[17], 0, &pos, &angle, nullptr);
    }
}

void dEnTorideKokoopa_c::notice1Vo() {
    if ((*(int *)((u8 *)m_6fc + 4) != 0) && (mpVoicesInfo != nullptr) && (mpVoicesInfo->mEntries[0].mSoundID != (ulong)SE_VOC_NO_SOUND) && mAnmChr.checkFrame(mpVoicesInfo->mEntries[0].mAnmFrame)) {
        mSndObject.startSound(mpVoicesInfo->mEntries[0].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::notice2Vo() {
    if ((*(int *)((u8 *)m_6fc + 12) != 0) && (mpVoicesInfo != nullptr)) {
        for (int i = 1; i <= 2; i++) {
            if ((mpVoicesInfo->mEntries[i].mSoundID != (ulong)SE_VOC_NO_SOUND) && mAnmChr.checkFrame(mpVoicesInfo->mEntries[i].mAnmFrame)) {
                mSndObject.startSound(mpVoicesInfo->mEntries[i].mSoundID, m_d0, 0);
            }
        }
    }
}

void dEnTorideKokoopa_c::wakeVo() {
    if (mpVoicesInfo != nullptr) {
        for (int i = 3; i <= 4; i++) {
            if ((mpVoicesInfo->mEntries[i].mSoundID != (ulong)SE_VOC_NO_SOUND) && mAnmChr.checkFrame(mpVoicesInfo->mEntries[i].mAnmFrame)) {
                mSndObject.startSound(mpVoicesInfo->mEntries[i].mSoundID, m_d0, 0);
            }
        }
    }
}

void dEnTorideKokoopa_c::escJumpVo() {
    if ((mpVoicesInfo != nullptr) && (mpVoicesInfo->mEntries[5].mSoundID != (ulong)SE_VOC_NO_SOUND)) {
        mSndObject.startSound(mpVoicesInfo->mEntries[5].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::magicShotVo() {
    if ((mpVoicesInfo != nullptr) && (mpVoicesInfo->mEntries[6].mSoundID != (ulong)SE_VOC_NO_SOUND)) {
        mSndObject.startSound(mpVoicesInfo->mEntries[6].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::shellOutVo() {
    if ((mpVoicesInfo != nullptr) && (mpVoicesInfo->mEntries[7].mSoundID != (ulong)SE_VOC_NO_SOUND) && mAnmChr.checkFrame(mpVoicesInfo->mEntries[7].mAnmFrame)) {
        mSndObject.startSound(mpVoicesInfo->mEntries[7].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::damageSVo() {
    if ((mpVoicesInfo != nullptr) && (mpVoicesInfo->mEntries[8].mSoundID != (ulong)SE_VOC_NO_SOUND)) {
        mSndObject.startSound(mpVoicesInfo->mEntries[8].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::damageLVo() {
    if ((mpVoicesInfo != nullptr) && (mpVoicesInfo->mEntries[9].mSoundID != (ulong)SE_VOC_NO_SOUND)) {
        mSndObject.startSound(mpVoicesInfo->mEntries[9].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::deadVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (mpVoicesInfo->mEntries[10].mSoundID != (ulong)SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[10].mSoundID, m_d0, 0);
    } else if (mpVoicesInfo->mEntries[9].mSoundID != (ulong)SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[9].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::loseFirstVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    if (mpVoicesInfo->mEntries[11].mSoundID != (ulong)SE_VOC_NO_SOUND) {
        mSndObject.startSound(mpVoicesInfo->mEntries[11].mSoundID, m_d0, 0);
    }
}

void dEnTorideKokoopa_c::loseSecondVo() {
    if (mpVoicesInfo == nullptr) {
        return;
    }

    for (int i = 12; i <= 14; i++) {
        if ((mpVoicesInfo->mEntries[i].mSoundID != (ulong)SE_VOC_NO_SOUND) && (mAnmChr.checkFrame(mpVoicesInfo->mEntries[i].mAnmFrame))) {
            mSndObject.startSound(mpVoicesInfo->mEntries[i].mSoundID, m_d0, 0);
        }
    }
}

bool dEnTorideKokoopa_c::checkDownJump() {
    float x = mPos.x;
    float y = mPos.y - 8.0f;
    int i = 0;

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
    m_7e4 = dGameCom::rndInt(3) + 1;
    mAccelY = getJumpGravity();
    if (m_7e4 > 1) {
        changeState(StateID_Jump_St);
    } else {
        changeState(StateID_BigJump_St);
    }
}

void dEnTorideKokoopa_c::initializeState_Jump_St() {
    if (mJumpAnmNames[0] != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(mJumpAnmNames[0]), m3d::FORWARD_ONCE);
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
    if (mJumpAnmNames[1] != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(mJumpAnmNames[1]), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    bool ok = true;
    if (!mpBossLife->isNonDamage() && (mpBossLife->isOneDamage() == false)) {
        ok = false;
    }

    mVec2_c speed;
    if (ok) {
        speed.x = *((float *)&mJumpAnmNames[5]);
        speed.y = *((float *)&mJumpAnmNames[6]);
    } else {
        speed.x = *((float *)&mJumpAnmNames[9]);
        speed.y = *((float *)&mJumpAnmNames[10]);
    }

    float speedX = l_EnMuki[mDirection] * calcJumpRate();
    mSpeed.x = speedX * speed.x;
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
    if (mJumpAnmNames[2] != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(mJumpAnmNames[2]), m3d::FORWARD_ONCE);
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
    if (mJumpAnmNames[3] != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(mJumpAnmNames[3]), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    bool ok = true;
    if (!mpBossLife->isNonDamage() && (mpBossLife->isOneDamage() == false)) {
        ok = false;
    }

    mVec2_c speed;
    if (ok) {
        speed.x = *((float *)&mJumpAnmNames[7]);
        speed.y = *((float *)&mJumpAnmNames[8]);
    } else {
        speed.x = *((float *)&mJumpAnmNames[11]);
        speed.y = *((float *)&mJumpAnmNames[12]);
    }

    float speedX = l_EnMuki[mDirection] * calcJumpRate();
    mSpeed.x = speedX * speed.x;
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
    if (mJumpAnmNames[4] != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(mJumpAnmNames[4]), m3d::FORWARD_ONCE);
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
        m_7e4--;
        if (m_7e4 > 1) {
            changeState(StateID_Jump_St);
        } else if (m_7e4 == 1) {
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
    char * anm_name = m_6f4[0];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_ONCE);
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

    m_700 = calcAttackTarget();
    lockonTurn();
    calcLookAngle();
    if (mAnmChr.isStop() || (m_6f4[0] == nullptr)) {
        changeState(StateID_AttackSearch);
    }
}


void dEnTorideKokoopa_c::initializeState_AttackSearch() {
    char * anm_name = m_6f4[1];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }
    anm_name = m_6f4[4];
    if (anm_name != nullptr) {
        mAnmMatClr.setAnm(mModel, mResFile.GetResAnmClr(anm_name), 1, m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmMatClr);
        mAnmMatClr.setFrame(0.0f, 1);
    }
    mCc2.release();
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

    m_700 = calcAttackTarget();
    int i3 = lockonTurn();
    calcLookAngle();
    blitzchargeSE();
    switch (m_23b) {
        case 1: {
            if (isCreateBlitz()) {
                m_70c = createBlitz();
                if (mpBossLife->isTwoDamage() && (mAtkCnt == 1)) {
                    m_704 = getAtkSearch2ndTime();
                } else {
                    m_704 = ((float)getAtkSearchTime()) / m_a68;
                }
                m_23b = 2;
            }
            break;
        }
        case 2: {
            dActor_c *actor;
            if (m_70c == BASE_ID_NULL) {
                actor = nullptr;
            } else {
                actor = (dActor_c *)fManager_c::searchBaseByID(m_70c);
            }

            blitzMove(actor);

            m_704--;
            if ((m_704 <= 0) && (i3 != 0)) {
                changeState(StateID_Attack);
            }
            break;
        }
    }
}

void dEnTorideKokoopa_c::initializeState_Attack() {
    char * anm_name = m_6f4[2];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }
    mCc2.set(this, (sCcDatNewF *) &l_wand_cc);
    mCc2.entry();
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

    dActor_c *actor;
    if (m_70c == BASE_ID_NULL) {
        actor = nullptr;
    } else {
        actor = (dActor_c *)fManager_c::searchBaseByID(m_70c);
    }

    if (actor == nullptr) {
        m_23b = 2;
    }

    switch (m_23b) {
        case 1: {
            blitzMove(actor);
            if (isShootBlitz()) {
                actor->mDirection = mDirection;
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
    char * anm_name = m_6f4[3];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(m_a68);
    }
    m_71c = 0;
    mTimer1 = getAtkEndTime();
    m_23b = 1;
}
void dEnTorideKokoopa_c::finalizeState_AttackEnd() {
    mCc2.release();
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
    char * anm_name = m_6f8[1];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mCc2.release();
    mCc2.release(); //< copy-paste error?
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mAngle.x = 0;
    mAngle.y = 0;
    m_a40 = getPressScale();
    mScale = m_a40;
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
            mVec3_c vec = mVec3_c(scale - m_a40.x, scale - m_a40.y, scale - m_a40.z);
            int time = getFumiRecoverTime();
            if (time <= 0) {
                m_a4c = vec;
            } else {
                m_a4c = vec / time;
            }
            m_23b = 2;
            break;
        }
        case 2: {
            mScale += m_a4c;
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
    char * anm_name = m_6f8[1]; //< copy-paste typo?
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6f8[0]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mCc2.release();
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
    char * anm_name = m_6f8[1]; //< copy-paste typo?
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6f8[0]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mCc2.release();
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
    char * anm_name = m_6f8[1]; //< copy-paste typo?
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6f8[0]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mCc2.release();
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
    char * anm_name = m_6f8[1]; //< copy-paste typo?
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6f8[0]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();
    mCc2.release();
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
    if (m_730 == 1) {
        char * anm_name = m_6f8[2];
        if (anm_name != nullptr) {
            mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_ONCE);
            mModel.setAnm(mAnmChr, 4.0f);
            mAnmChr.setRate(1.0f);
        }

        anm_name = m_6f8[4];
        if (anm_name != nullptr) {
            mAnmChr2.setAnm(mModel2, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_ONCE);
            mModel2.setAnm(mAnmChr2, 0.0f);
            mAnmChr2.setRate(1.0f);
        }
    }

    if ((*mStateMgr.getOldStateID() != StateID_FireHit) &&
            (*mStateMgr.getOldStateID() != StateID_SlideHit) &&
            (*mStateMgr.getOldStateID() != StateID_StarHit)) {
        mAccelY = -0.275f;

        if (checkDownJump()) {
            mSpeed.set(0.0f, 6.25f, 0.0f);
        } else {
            mSpeed.set(0.0f, 5.25f, 0.0f);
        }

        mSpeedMax.y = -4.0f;
    }

    m_a5c = 0x200;
    mLookAngle = 0;
    m_a3c = 4;
    m_730 |= 2;
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
    mModel2.play();

    if (mAnmChr.isStop() && (m_730 != 2)) {
        changeShell();
        m_730 = 2;
    }

    if ((*mStateMgr.getOldStateID() == StateID_FireHit) ||
            (*mStateMgr.getOldStateID() == StateID_SlideHit)) {
        if ((mSpeed.y >= 0.0f) || (m_730 == 2)) {
            calcSpeedY();
            posMove();
        }
    } else {
        calcSpeedY();
        posMove();
    }

    if (m_730 == 2) {
        m_a5c += 0x200;
        if (m_a5c > 0x1800) {
            m_a5c = 0x1800;
        }
        mAngle.y += m_a5c;
        shellatkSE();
        shellBumMarEffect();
    } else if (*mStateMgr.getOldStateID() == StateID_FireHit) {
        hitFireDamageEffect();
    }

    mAngle.z = 0;
    if (mBc.checkFootEnm()) {
        shelllandonSE();
        shellLandonEffect();

        m_a3c--;
        if (m_a3c > 0) {
            mSpeed.y = y_speeds[m_a3c];
        } else {
            mSpeed.y = 0.0f;
            changeState(StateID_ShellAtk);
        }
    }
}

void dEnTorideKokoopa_c::initializeState_ShellAtk() {

    float f4 = dGameCom::getDispCenterX() + m_7dc[0];
    float f5 = dGameCom::getDispCenterX() + m_7dc[1];
    float f6 = (f4 + f5) * 0.5f;

    if (mPos.x >= f6) {
        mDirection = 1;
    } else {
        mDirection = 0;
    }

    mActorProperties &= ~0x200;
    m_a64 = 5;
    mAccelF = 0.3f;
    mSpeed.x = l_shellatk_speed[mDirection];

    if (mPos.x - 32.0f < f5) {
        mSpeedMax.x = l_shellatk_speed[mDirection];
        m_a60 = f5 + 32.0f;
        m_a64 = 6;
    } else if (mPos.x + 32.0f > f4) {
        mSpeedMax.x = l_shellatk_speed[mDirection];
        m_a60 = f4 - 32.0f;
        m_a64 = 6;
    } else {
        m_a60 = mPos.x;
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
    float f2 = mPos.x;
    posMove();
    if (mBc.checkFootEnm()) {
        mSpeed.y = 0.0f;
    }

    if (mSpeed.x >= 0.0f) {
        mDirection = 0;
    } else {
        mDirection = 1;
    }

    mAngle.y += m_a5c;
    s16 a = 0;
    if (m_a64 > 1) {
        a = -shorts[mDirection];
    }
    sLib::chaseAngle((s16 *)&mAngle.z, a, 0x80);

    shellatkSE();
    shellBumMarEffect();
    shellAtkEffect();

    if (((f2 < m_a60) && (mPos.x >= m_a60)) || ((f2 > m_a60) && (mPos.x <= m_a60))) {
        m_a64--;
        if (m_a64 > 0) {
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
    char * anm_name = m_6f8[3];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 1.0f);
        mAnmChr.setRate(1.0f);
    }

    anm_name = m_6f8[5];
    if (anm_name != nullptr) {
        mAnmChr2.setAnm(mModel2, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_ONCE);
        mModel2.setAnm(mAnmChr2, 1.0f);
        mAnmChr2.setRate(1.0f);
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
    mModel2.play();

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
            if (!(m_730 & 1) && mAnmChr.checkFrame(getKokoopaOnFrm())) {
                m_730 |= 1;
            }
            if ((m_730 & 2) && mAnmChr.checkFrame(getShellOffFrm())) {
                m_730 &= ~2;
            }
            if (m_730 == 1) {
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
    char * anm_name = m_6f8[1];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 3.0f);
        mAnmChr.setRate(1.0f);
    }

    removeCc();

    mSpeed.set(0.0f, 0.0f, 0.0f);
    mAngle.x = 0;
    mAngle.y = 0;
    m_a40 = getPressScale();
    mScale = m_a40;
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
            mVec3_c vec = mVec3_c(scale - m_a40.x, scale - m_a40.y, scale - m_a40.z);
            int time = getFumiRecoverTime();
            if (time <= 0) {
                m_a4c = vec;
            } else {
                m_a4c = vec / time;
            }
            m_23b = 2;
            break;
        }
        case 2: {
            mScale += m_a4c;
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
    if ((m_6fc != nullptr) && (m_6fc[0] != nullptr)) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(m_6fc[0]), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);
    }

    mSpeed.set(0.0f, 0.0f, 0.0f);
    mDirection = getPl_LRflag(mPos);
    mAngle.y = l_EnMuki[mDirection] * defaultDirAngle();

    m_730 = 1;
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
    char *anm_name = m_6fc[1];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 5.0f);
        mAnmChr.setRate(1.0f);

        m_644 = mResFile.GetResAnmTexPat(m_6fc[1]);
        mAnmTexPat.setAnm(mModel, m_644, 0, m3d::FORWARD_ONCE);
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
    char *anm_name = m_6fc[2];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);

        m_644 = mResFile.GetResAnmTexPat(m_6fc[2]);
        mAnmTexPat.setAnm(mModel, m_644, 0, m3d::FORWARD_LOOP);
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
    char *anm_name = m_6fc[3];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_ONCE);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);

        m_644 = mResFile.GetResAnmTexPat(m_6fc[3]);
        mAnmTexPat.setAnm(mModel, m_644, 0, m3d::FORWARD_ONCE);
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
    char *anm_name = m_6fc[4];
    if (anm_name != nullptr) {
        mAnmChr.setAnm(mModel, mResFile.GetResAnmChr(anm_name), m3d::FORWARD_LOOP);
        mModel.setAnm(mAnmChr, 0.0f);
        mAnmChr.setRate(1.0f);

        m_644 = mResFile.GetResAnmTexPat(m_6fc[4]);
        mAnmTexPat.setAnm(mModel, m_644, 0, m3d::FORWARD_LOOP);
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
