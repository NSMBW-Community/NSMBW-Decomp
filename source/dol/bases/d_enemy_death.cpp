#include <game/bases/d_enemy.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_multi_manager.hpp>
#include <game/bases/d_score_manager.hpp>
#include <game/mLib/m_effect.hpp>
#include <constants/sound_list.h>

STATE_VIRTUAL_DEFINE(dEn_c, DieFumi);
STATE_VIRTUAL_DEFINE(dEn_c, DieFall);
STATE_VIRTUAL_DEFINE(dEn_c, DieBigFall);
STATE_VIRTUAL_DEFINE(dEn_c, DieSmoke);
STATE_VIRTUAL_DEFINE(dEn_c, DieIceVanish);
STATE_VIRTUAL_DEFINE(dEn_c, DieYoshiFumi);
STATE_VIRTUAL_DEFINE(dEn_c, DieGoal);
STATE_VIRTUAL_DEFINE(dEn_c, DieOther);

const float dEn_c::smc_DEADFALL_GRAVITY = -0.25f + 0.00625001f; // [Where is the 0.00000001 coming from?]
const float dEn_c::smc_DEADFALL_YSPEED = 3.9f;
const float dEn_c::smc_DEADFALL_YSPEED_MAX = -4.0f;
const s16 dEn_c::smc_DEADFALL_SPINSPEED = 0xc00;

bool dEn_c::hitCallback_Star(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    u8 plrNo = *player->getPlrNo();

    mVec2_c collPos = cc1->mCollPos;
    mVec3_c pos(collPos, 5500.0f);
    hitdamageEffect(pos);

    int shortCombo = 0;
    if (mCombo.mType == 2) {
        shortCombo = 1;
    }
    player->slideComboSE(player->getStarCount(), shortCombo);

    int score = mCombo.getComboScore(dEnCombo_c::calcPlStarCnt(player));

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        score,
        -1,
        dir,
        plrNo
    };

    return true;
}

bool dEn_c::hitCallback_Cannon(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    u8 plrNo = *player->getPlrNo();

    mVec2_c collPos = cc1->mCollPos;
    mVec3_c pos(collPos, 5500.0f);
    hitdamageEffect(pos);

    int shortCombo = 0;
    if (mCombo.mType == 2) {
        shortCombo = 1;
    }
    player->slideComboSE(player->m_cee, shortCombo);

    int score = mCombo.getComboScore(dEnCombo_c::calcPlComboCnt(player));

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        score,
        -1,
        dir,
        plrNo
    };

    return true;
}

bool dEn_c::hitCallback_Slip(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    u8 plrNo = *player->getPlrNo();

    mVec2_c collPos = cc1->mCollPos;
    mVec3_c pos(collPos, 5500.0f);
    hitdamageEffect(pos);

    setDeathSound_Slip(player);

    int score = mCombo.getComboScore(dEnCombo_c::calcPlComboCnt(player));

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        score,
        -1,
        dir,
        plrNo
    };

    return true;
}

bool dEn_c::hitCallback_PenguinSlide(dCc_c *cc1, dCc_c *cc2) {
    return hitCallback_Slip(cc1, cc2);
}

bool dEn_c::hitCallback_Screw(dCc_c *cc1, dCc_c *cc2) {
    return false;
}

bool dEn_c::hitCallback_WireNet(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    u8 plrNo = *player->getPlrNo();

    dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN, mPos, 0);

    mDeathInfo = (sDeathInfoData) {
        0.75f * l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        mCombo.getDamageScore(),
        -1,
        dir,
        plrNo
    };

    return true;
}

bool dEn_c::hitCallback_Large(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    u8 plrNo = *player->getPlrNo();

    float xSpeed = l_EnMuki[dir];
    xSpeed += player->mSpeed.x - mSpeed.x;
    if (xSpeed > 6.0f) {
        xSpeed = 6.0f;
    } else if (xSpeed < -6.0f) {
        xSpeed = -6.0f;
    }

    mDeathInfo = (sDeathInfoData) {
        xSpeed,
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieBigFall,
        mCombo.getDamageScore(),
        -1,
        dir,
        plrNo
    };

    return true;
}

bool dEn_c::hitCallback_Rolling(dCc_c *cc1, dCc_c *cc2) {
    return false;
}

bool dEn_c::hitCallback_Spin(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    u8 plrNo = *player->getPlrNo();

    setDeathSound_Spin();

    mVec2_c pos2D;
    pos2D.x = player->mPos.x;
    pos2D.y = player->mPos.y;
    mVec3_c pos(pos2D, 5500.0f);
    hipatkEffect(pos);

    int score = mCombo.getComboScore(dEnCombo_c::calcPlFumiCnt(player));

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        score,
        -1,
        dir,
        plrNo
    };

    return true;
}

bool dEn_c::hitCallback_HipAttk(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    u8 plrNo = *player->getPlrNo();

    setDeathSound_HipAttk();

    mVec2_c pos2D;
    pos2D.x = player->mPos.x;
    pos2D.y = player->mPos.y;
    mVec3_c pos(pos2D, 5500.0f);
    hipatkEffect(pos);

    int score = mCombo.getComboScore(dEnCombo_c::calcPlFumiCnt(player));

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        score,
        -1,
        dir,
        plrNo
    };

    return true;
}

bool dEn_c::hitCallback_YoshiHipAttk(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    u8 plrNo = *player->getPlrNo();

    int comboScore = mCombo.getComboScore(dEnCombo_c::calcPlFumiCnt(player));
    if (0 <= comboScore && plrNo < 4) {
        dScoreMng_c::m_instance->ScoreSet(this, comboScore, plrNo, dScoreMng_c::smc_SCORE_X, dScoreMng_c::smc_SCORE_Y);
    }

    dAudio::g_pSndObjEmy->startSound(SE_EMY_YOSHI_HPDP, mPos, 0);

    setDeathInfo_YoshiFumi(player);

    return true;
}

bool dEn_c::hitCallback_YoshiBullet(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    s8 plrNo = *player->getPlrNo();

    setDeathSound_Fire();

    mVec3_c pos(cc1->getCollPosX(), cc1->getCollPosY(), 5500.0f);
    hitdamageEffect(pos);

    dActorMng_c::m_instance->FUN_80066630(getCenterPos(), dir, 1, 0);

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        mCombo.getDamageScore(),
        -1,
        dir,
        (u8) plrNo
    };

    return true;
}

bool dEn_c::hitCallback_YoshiFire(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = !(player->mSpeed.x >= 0.0f);
    mVec3_c centerPos = getCenterPos();
    dActorMng_c::m_instance->FUN_80066630(centerPos, dir, 1, 0);
    s8 plrNo = *player->getPlrNo();

    mVec2_c collPos = cc1->mCollPos;
    mVec3_c pos(collPos, 5500.0f);
    hitdamageEffect(pos);

    s8 shortCombo = false;
    if (mCombo.mType == 2) {
        shortCombo = true;
    }
    player->slideComboSE(player->mComboMultiplier, shortCombo);
    player->mComboMultiplier++;
    if (player->mComboMultiplier >= 8) {
        player->mComboMultiplier = 8;
    }
    int score = mCombo.getComboScore(player->mComboMultiplier);

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
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

bool dEn_c::hitCallback_Shell(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = player->getTrgToSrcDir_Main(getCenterX(), player->getCenterX());
    s8 plrNo = *player->getPlrNo();

    mVec2_c collPos = cc1->mCollPos;
    mVec3_c pos(collPos, 5500.0f);
    hitdamageEffect(pos);

    int comboScore = -1;
    if ((u8) plrNo <= 3) {
        s8 shortCombo = false;
        if (mCombo.mType == 2) {
            shortCombo = true;
        }
        player->slideComboSE(player->mComboMultiplier, shortCombo);
        player->mComboMultiplier++;
        if (player->mComboMultiplier >= 8) {
            player->mComboMultiplier = 8;
        }
        comboScore = mCombo.getComboScore(player->mComboMultiplier);
    } else {
        dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN, mPos, 0);
    }

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        comboScore,
        -1,
        dir,
        (u8) plrNo
    };

    return true;
}

bool dEn_c::hitCallback_Fire(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *player = (daPlBase_c *) cc2->getOwner();
    u8 dir = !(player->mSpeed.x >= 0.0f);
    s8 plrNo = *player->getPlrNo();

    setDeathSound_Fire();

    dActorMng_c::m_instance->FUN_80066630(getCenterPos(), dir, 1, 0);

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        smc_DEADFALL_YSPEED,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        mCombo.getDamageScore(),
        -1,
        dir,
        (u8) plrNo
    };

    return true;
}

bool dEn_c::hitCallback_Ice(dCc_c *cc1, dCc_c *cc2) {
    if (mIceMng.m_0c == 0){
        daPlBase_c *player = (daPlBase_c *) cc2->getOwner();

        if (player->mSpeed.x >= 0.0f) {
            mBoyoMng.mDirection = 0;
        } else {
            mBoyoMng.mDirection = 1;
        }

        for (int i = 0; i < PLAYER_COUNT; i++) {
            dAcPy_c *pl = daPyMng_c::getPlayer(i);
            if (pl != nullptr && fManager_c::searchBaseByID(pl->mCarryActorID) == this) {
                pl->cancelCarry(this);
                break;
            }
        }
        mIceMng.mPlrNo = *player->getPlrNo();
        mStateMgr.changeToSubState(StateID_Ice);
    }

    return true;
}

void dEn_c::setDeadMode(dActor_c *actor, int i) {
    bool dead = false;
    if (mDeathInfo.mIsDead || mNoRespawn) {
        dead = true;
    }
    if (!dead && (!isState(StateID_Ice) || mIceMng.m_18 == 0)) {
        removeCc();
        if (i == 1) {
            setDeathInfo_Smoke(actor);
        } else {
            setDeathInfo_SpinFumi(actor, 0);

            // @unofficial
            static const float dir[] = { -6.0f, 6.0f };
            mVec3_c center = getCenterPos();
            center.x += dir[mDeathInfo.mDirection];
            hitdamageEffect(center);

            dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN, mPos, 0);
        }
    }
}

void dEn_c::setDeathInfo_Quake(int i) {
    mVec3_c center(getCenterX(), getCenterY(), 5500.0f);

    float ySpeed = 2.0f;
    if (i == 0) {
        EffectManager_c::FUN_800943d0(&center);
        ySpeed = 2.0f;
    } else if (i == 1) {
        EffectManager_c::FUN_800943d0(&center);
        ySpeed = 3.5f;
    }

    dEnemyMng_c::m_instance->breakdownSE(dEnemyMng_c::m_instance->m_154, mPos);

    dEnemyMng_c::m_instance->incQuakeComboCount(0);

    int comboScore = mCombo.getQuakeScore(dEnemyMng_c::m_instance->m_154);

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        ySpeed,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &StateID_DieFall,
        comboScore,
        -1,
        mDirection,
        dDeathInfo_c::smc_UNKNOWN_HIT
    };
}

void dEn_c::setDeathInfo_Smoke(dActor_c *actor) {
    u8 plrNo = mPlayerNo;
    if (actor != nullptr) {
        plrNo = *actor->getPlrNo();
    }

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        &StateID_DieSmoke,
        -1,
        -1,
        (u8) -1,
        plrNo
    };
}

void dEn_c::setDeathInfo_Fumi(dActor_c *killedBy, mVec2_c speed, const sStateIDIf_c &id, int) {
    bool dir = killedBy->getTrgToSrcDir_Main(getCenterX(), killedBy->getCenterX());
    u8 plrNo = *killedBy->getPlrNo();

    mDeathInfo = (sDeathInfoData) {
        speed.x,
        speed.y,
        smc_DEADFALL_YSPEED_MAX,
        smc_DEADFALL_GRAVITY,
        &id,
        -1,
        -1,
        dir,
        plrNo
    };
}

void dEn_c::setDeathInfo_YoshiFumi(dActor_c *killedBy) {
    bool dir = killedBy->getTrgToSrcDir_Main(getCenterX(), killedBy->getCenterX());
    u8 plrNo = *killedBy->getPlrNo();

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        &StateID_DieYoshiFumi,
        -1,
        -1,
        dir,
        plrNo
    };
}

void dEn_c::setDeathInfo_Other(dActor_c *killedBy) {
    s8 plrNo = -1;
    if (killedBy != nullptr) {
        plrNo = *killedBy->getPlrNo();
    }

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        &StateID_DieOther,
        -1,
        -1,
        (u8) -1,
        (u8) plrNo
    };
}

void dEn_c::setDeathInfo_SpinFumi(dActor_c *killedBy, int) {
    u8 dir = killedBy->getTrgToSrcDir_Main(getCenterX(), killedBy->getCenterX());
    u8 plrNo = *killedBy->getPlrNo();

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
}

void dEn_c::setDeathInfo_IceBreak() {
    killIce();

    u8 dir = mIceDeathDirection;
    u8 plrNo = mPlayerNo;

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
}

void dEn_c::setDeathInfo_IceVanish() {
    killIce();

    int n = dGameCom::rndInt(2);
    dActorMng_c::m_instance->FUN_80066630(getCenterPos(), n, 1, 0);

    u8 dir = mIceDeathDirection;
    u8 plrNo = mPlayerNo;

    mDeathInfo = (sDeathInfoData) {
        0.0f,
        0.0f,
        0.0f,
        0.0f,
        &StateID_DieIceVanish,
        -1,
        -1,
        dir,
        plrNo
    };
}

void dEn_c::initializeState_DieFumi() {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *pl = daPyMng_c::getPlayer(i);
        if (pl != nullptr && fManager_c::searchBaseByID(pl->mCarryActorID) == this) {
            pl->cancelCarry(this);
            break;
        }
    }
    removeCc();
    mBc.mFlags = 0;
    mSpeed.set(mDeathInfo.getXSpeed(), mDeathInfo.getYSpeed(), 0.0f);
    mSpeedMax.set(0.0f, mDeathInfo.getMaxYSpeed(), 0.0f);
    mFootAttr3 = 0;
    mIceDeathDirection = mDeathInfo.mDirection;
    mAccelF = 0.0f;
    mAccelY = mDeathInfo.getYAccel();
    if (mAngle.y >= 0) {
        mDirection = 0;
    } else {
        mDirection = 1;
    }
    mAngle.y = l_base_angleY[mDirection];

    int score = mDeathInfo.mScore;
    int plrNo = mDeathInfo.mKilledBy;
    if (score >= 0) {
        if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
            dScoreMng_c::m_instance->ScoreSet(this, score, plrNo, dScoreMng_c::smc_SCORE_X, dScoreMng_c::smc_SCORE_Y);
        } else if (plrNo == dDeathInfo_c::smc_UNKNOWN_HIT) {
            dScoreMng_c::m_instance->UnKnownScoreSet(this, score, 0.0f, 24.0f);
        }
    }

    if (mAmiLayer == 1) {
        mPos.z = -384.0f;
    } else {
        mPos.z = 4500.0f;
    }
}

void dEn_c::finalizeState_DieFumi() {}

void dEn_c::executeState_DieFumi() {
    s16 angDelta = smc_DEADFALL_SPINSPEED / 4;
    if (mInLiquid) {
        angDelta *= smc_WATER_ROLL_DEC_RATE;
    }
    mAngle.x += angDelta;
    calcSpeedY();
    posMove();
    WaterCheck(mPos, 1.0f);
}

void dEn_c::initializeState_DieFall() {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *pl = daPyMng_c::getPlayer(i);
        if (pl != nullptr && fManager_c::searchBaseByID(pl->mCarryActorID) == this) {
            pl->cancelCarry(this);
            break;
        }
    }
    removeCc();
    mBc.mFlags = 0;
    mSpeed.set(mDeathInfo.getXSpeed(), mDeathInfo.getYSpeed(), 0.0f);
    mSpeedMax.set(0.0f, mDeathInfo.getMaxYSpeed(), 0.0f);
    mFootAttr3 = 0;
    mIceDeathDirection = mDeathInfo.mDirection;
    mAccelF = 0.0f;
    mAccelY = mDeathInfo.getYAccel();
    if (mAngle.y >= 0) {
        mDirection = 0;
    } else {
        mDirection = 1;
    }
    mAngle.y = l_base_angleY[mDirection];

    int score = mDeathInfo.mScore;
    int plrNo = mDeathInfo.mKilledBy;
    if (score >= 0) {
        if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
            dScoreMng_c::m_instance->ScoreSet(this, score, plrNo, dScoreMng_c::smc_SCORE_X, dScoreMng_c::smc_SCORE_Y);
        } else if (plrNo == dDeathInfo_c::smc_UNKNOWN_HIT) {
            dScoreMng_c::m_instance->UnKnownScoreSet(this, score, 0.0f, 24.0f);
        }
    }

    if (mAmiLayer == 1) {
        mPos.z = -384.0f;
    } else {
        mPos.z = 4500.0f;
    }
}

void dEn_c::finalizeState_DieFall() {}

void dEn_c::executeState_DieFall() {
    const static s16 cs_spin_speed[] = { 0x100, -0x100 };

    s16 angDeltaX;
    s16 angDeltaY;
    if (mDirection == mIceDeathDirection) {
        angDeltaX = smc_DEADFALL_SPINSPEED;
        angDeltaY = -cs_spin_speed[mIceDeathDirection];
    } else {
        angDeltaX = -smc_DEADFALL_SPINSPEED;
        angDeltaY = cs_spin_speed[mIceDeathDirection];
    }
    if (mInLiquid) {
        angDeltaX *= smc_WATER_ROLL_DEC_RATE;
    }
    mAngle.x += angDeltaX;
    mAngle.y += angDeltaY;

    calcSpeedY();
    posMove();
    WaterCheck(mPos, 1.0f);
}

void dEn_c::initializeState_DieBigFall() {
    initializeState_DieFall();
}

void dEn_c::finalizeState_DieBigFall() {
    finalizeState_DieFall();
}

void dEn_c::executeState_DieBigFall() {
    mAngle.x += smc_DEADFALL_SPINSPEED;
    calcSpeedY();
    posMove();
    WaterCheck(mPos, 1.0f);
}

void dEn_c::initializeState_DieSmoke() {
    mVec3_c pos = getCenterPos();
    mEf::createEffect("Wm_en_burst_m", 0, &pos, nullptr, nullptr);
    removeCc();
}

void dEn_c::finalizeState_DieSmoke() {}

void dEn_c::executeState_DieSmoke() {
    deleteActor(1);
}

void dEn_c::initializeState_DieIceVanish() {
    removeCc();

    int score = mDeathInfo.mScore;
    int plrNo = mDeathInfo.mKilledBy;
    if (score >= 0) {
        if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
            dScoreMng_c::m_instance->ScoreSet(this, score, plrNo, dScoreMng_c::smc_SCORE_X, dScoreMng_c::smc_SCORE_Y);
        } else if (plrNo == dDeathInfo_c::smc_UNKNOWN_HIT) {
            dScoreMng_c::m_instance->UnKnownScoreSet(this, score, 0.0f, 24.0f);
        }
    }
}

void dEn_c::finalizeState_DieIceVanish() {}

void dEn_c::executeState_DieIceVanish() {
    deleteActor(1);
}

void dEn_c::initializeState_DieYoshiFumi() {
    mVec3_c pos = getCenterPos();
    mEf::createEffect("Wm_mr_yoshistep", 0, &pos, nullptr, nullptr);
    removeCc();
}

void dEn_c::finalizeState_DieYoshiFumi() {}

void dEn_c::executeState_DieYoshiFumi() {
    deleteActor(1);
}

void dEn_c::initializeState_DieGoal() {}

void dEn_c::finalizeState_DieGoal() {}

void dEn_c::executeState_DieGoal() {}

void dEn_c::initializeState_DieOther() {
    deleteActor(1);
}

void dEn_c::finalizeState_DieOther() {}

void dEn_c::executeState_DieOther() {}

void dEn_c::setDeathSound_Fire() {
    dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN, mPos, 0);
}

void dEn_c::setDeathSound_HipAttk() {
    if (mFlags & FLAG_1) {
        dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN_HPDP_H, mPos, 0);
    } else {
        dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN_HPDP_S, mPos, 0);
    }
}

void dEn_c::setDeathSound_Slip(dActor_c *killedBy) {
    const static dAudio::SoundEffectID_t cs_combo_se[] = {
        SE_EMY_DOWN_COMBO_1,
        SE_EMY_DOWN_COMBO_2,
        SE_EMY_DOWN_COMBO_3,
        SE_EMY_DOWN_COMBO_4,
        SE_EMY_DOWN_COMBO_5,
        SE_EMY_DOWN_COMBO_6,
        SE_EMY_DOWN_COMBO_7,
        SE_EMY_DOWN_COMBO_7,
        SE_EMY_DOWN_COMBO_7
    };

    int count = ((daPlBase_c *) killedBy)->m_cee;
    if (count >= ARRAY_SIZE(cs_combo_se)) {
        count = ARRAY_SIZE(cs_combo_se) - 1;
    };
    if (checkComboClap(count)) {
        dMultiMng_c::mspInstance->setClapSE();
    }

    cs_combo_se[count].playEmySound(getCenterPos(), 0);
}

void dEn_c::setDeathSound_Spin() {
    if (mFlags & FLAG_1) {
        dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN_HPDP_H, mPos, 0);
    } else {
        dAudio::g_pSndObjEmy->startSound(SE_EMY_DOWN_HPDP_S, mPos, 0);
    }
}

void dEn_c::setDeathSound_Rolling() {}
