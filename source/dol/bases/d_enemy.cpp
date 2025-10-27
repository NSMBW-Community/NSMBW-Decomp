#include <game/bases/d_enemy.hpp>
#include <game/bases/d_bc.hpp>
#include <game/bases/d_quake.hpp>
#include <game/mLib/m_ef.hpp>
#include <game/bases/d_a_player.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_score_mng.hpp>
#include <game/bases/d_ef.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <game/bases/d_effactor_mng.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_multi_mng.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <constants/sound_list.h>

const s8 l_base_speedX[] = { 1, -1 };
const u16 l_base_angleY[] = { 0x2000, -0x2000 };
const u16 l_unk_angle[] = { 0x400, -0x400 };

const float dEn_c::smc_DEADFALL_GRAVITY[] = { 1.5f, -1.5f };
const float dEn_c::smc_WATER_GRAVITY = -0.0625f;
const float dEn_c::smc_WATER_YMAXSPD = 1.5f;
const float dEn_c::smc_WATER_FALLMAXSPD = -1.5f;
const float dEn_c::smc_WATER_ROLL_DEC_RATE = 0.5f;

dEn_c::dEn_c() : m_24(0), mFootRelated(mVec3_c(0.0f, 0.0f, 0.0f)), m_46(false), mFootAttr3(false), mFootAttr1(false),
mBlockHit(), mBoyoMng(this), mIceMng(this), mCounterCont(1), mFumiProc(this) {
    mFumiProc.refresh(new NonUniqueFumiCheck_c());
    mAccelY = -0.1875f;
    mSpeedMax.set(0.0f, -4.0f, 0.0f);
    mSaveAccelY = -0.1875f;
    mSaveSpeedMaxY = -4.0f;
    mSaveMaxFallSpeed = -4.0f;
    setKind(STAGE_ACTOR_ENTITY);
    mExecStopMask = 8;
    mFlags = 0;
    mCounterCont.mCounters2[0] = 0;
    mCounterCont.mCounters2[1] = 0;
    mCounterCont.mCounters2[2] = 0;
    mCounterCont.mCounters2[3] = 0;
}

dEn_c::~dEn_c() {}

void dEn_c::postCreate(fBase_c::MAIN_STATE_e status) {
    boyonInit();
    if (dBc_c::checkWater(mPos.x, mPos.y, mLayer, nullptr)) {
        m_49 = true;
    }
    dActor_c::postCreate(status);
}

int dEn_c::preExecute() {
    if (dActor_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }
    if (!mNoRespawn && !isState(StateID_Ice) && isQuakeDamage()) {
        u32 flags = dQuake_c::m_instance->mFlags;
        if (flags & 2) {
            if (checkDispIn()) {
                setDeathInfo_Quake(0);
            }
        } else if (flags & 4) {
            if (checkDispIn()) {
                setDeathInfo_Quake(1);
            }
        } else if (flags & 1) {
            quakeAction();
        }
    }
    if (mBlockHit.mData.mIsInit) {
        if (!mNoRespawn) {
            mNoRespawn = true;
            setNicePoint_Death();
            changeState(*mBlockHit.mData.mDeathState);
        }
        mBlockHit.mData.mIsInit = false;
    } else {
        if (mCounterCont.mCounter != 0) {
            mCounterCont.mCounter--;
        }
        for (int i = 0; i < 2; i++) {
            if (mCounterCont.mCounters1[i] != 0) {
                mCounterCont.mCounters1[i]--;
            }
        }
        if (mCounterCont.mCounters2[0] != 0) {
            mCounterCont.mCounters2[0]--;
        }
        if (mCounterCont.mCounters2[1] != 0) {
            mCounterCont.mCounters2[1]--;
        }
        if (mCounterCont.mCounters2[2] != 0) {
            mCounterCont.mCounters2[2]--;
        }
        if (mCounterCont.mCounters2[3] != 0) {
            mCounterCont.mCounters2[3]--;
        }
    }
    calcBoyonScale();
    return mDeleteRequested ? NOT_READY : SUCCEEDED;
}

void dEn_c::postExecute(fBase_c::MAIN_STATE_e status) {
    if (status == SUCCESS && !(dActor_c::mExecStop & 8)) {
        mBgCollFlags = 0;
    }
    dActor_c::postExecute(status);
}

int dEn_c::preDraw() {
    if (dActor_c::preDraw() == NOT_READY) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

void dEn_c::hitdamageEffect(const mVec3_c &pos) {
    mVec3_c efPos = pos;
    efPos.z = 5500.0f;
    mEf::createEffect("Wm_en_hit", 0, &efPos, nullptr, nullptr);
}

void dEn_c::fumidamageEffect(const mVec3_c &pos) {
    hitdamageEffect(pos);
}

void dEn_c::hipatkEffect(const mVec3_c &pos) {
    if (mFlags & 2) {
        dEf::createEffect_change("Wm_mr_hardhit", 0, &pos, nullptr, nullptr);
    } else {
        dEf::createEffect_change("Wm_mr_softhit", 0, &pos, nullptr, nullptr);
    }
}

void dEn_c::fumidamageSE(const mVec3_c &, int) {
    dAudio::SoundEffectID_t(SE_EMY_DOWN).playEmySound(mPos, 0);
}

void dEn_c::quakeAction() {
    mSpeed.y += 2.0f;
}

void dEn_c::posMove() {
    u16 ang = mBc.getSakaMoveAngle(mSpeed.x < 0.0f);
    mVec3_c move;
    if (mFootAttr3) {
        move.set(0.0f, 0.0f, 0.0f);
    } else {
        float speed = std::fabs(mSpeed.x);
        float cos = mAng(ang).cos();
        float sin = mAng(ang).sin();
        move.x = speed * cos;
        move.y = mSpeed.y + speed * sin;
        move.z = mSpeed.z;
        if (m_49) {
            mFootRelated2.set(0.0f, 0.0f, 0.0f);
            move.x /= 2;
        } else {
            bool ok = mBlockHit.mData.mIsInit || mNoRespawn;
            if (!ok && mSpeed.y <= 0.0f) {
                move.x += mFootRelated2.x;
            }
        }
    }
    move += mFootRelated;
    mPos += move;
}

void dEn_c::Bound(float eps_y, float mul_x, float mul_y) {
    if (!mBc.isFoot()) {
        return;
    }
    if (mul_x < 1.0f) {
        mSpeed.x *= mul_x;
        if (std::fabs(mSpeed.x) < 1.0f / 16) {
            mSpeed.x = 0.0f;
        }
    }
    if (mSpeed.y < 0.0f) {
        mSpeed.y = (float) ((double) -mSpeed.y) * mul_y;
        if (mSpeed.y < eps_y) {
            mSpeed.y = 0.0f;
        }
    }
}

bool dEn_c::checkDispIn() {
    float x = getCenterPos().x;
    float y = getCenterPos().y;
    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;
    float xStart = bgParam->xStart();
    float xEnd = bgParam->xEnd();
    float yStart = bgParam->yStart();
    float yEnd = bgParam->yEnd();
    if (x < xStart) {
        return false;
    }
    if (x > xEnd) {
        return false;
    }
    if (y > yStart) {
        return false;
    }
    if (y < yEnd) {
        return false;
    }
    return true;
}

void dEn_c::normal_collcheck(dCc_c *cc1, dCc_c *cc2) {
    dEn_c *o1 = (dEn_c *) cc1->mpOwner;
    dEn_c *o2 = (dEn_c *) cc2->mpOwner;
    if (o1->mBlockHit.mData.mIsInit) {
        cc1->mFlag |= 2;
        return;
    }
    u32 kind = o2->mKind;
    if (kind == 3) {
        if (o1->EnDamageCheck(cc1, cc2)) {
            cc1->mFlag |= 2;
        } else {
            o1->Normal_VsEnHitCheck(cc1, cc2);
        }
    } else if (kind == 1) {
        if (o1->mFlags & 0x1000000 || !CeilCheck(o1->mPos.y, cc1)) {
            if (o1->PlDamageCheck(cc1, cc2)) {
                o1->mCcValue = cc1->unk3;
                cc1->mFlag |= 2;
            } else if (cc2->mCcData.mCategory != dCc_c::CAT_PLAYER_ATTACK) {
                s8 *plrNo = o2->getPlrNo();
                if ((u8) *plrNo <= 3) {
                    int pNo = *plrNo;
                    if (o1->mCounterCont.mCounters2[pNo] == 0) {
                        o1->mCounterCont.mCounters2[pNo] = 5;
                        o1->Normal_VsPlHitCheck(cc1, cc2);
                    }
                }
            }
        }
    } else if (kind == 2) {
        s8 *plrNo = o2->getPlrNo();
        if ((u8) *plrNo <= 3) {
            if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_YOSHI_EAT) {
                o1->hitYoshiEat(cc1, cc2);
            } else {
                if (!CeilCheck(o1->mPos.y, cc1)) {
                    if (o1->YoshiDamageCheck(cc1, cc2)) {
                        o1->mCcValue = cc1->unk3;
                        cc1->mFlag |= 2;
                    } else if (o1->mCounterCont.mCounters2[*plrNo] == 0) {
                        o1->mCounterCont.mCounters2[*plrNo] = 5;
                        o1->Normal_VsYoshiHitCheck(cc1, cc2);
                    }
                }
            }
        }
    } else {
        if (o1->EtcDamageCheck(cc1, cc2)) {
            o1->mCcValue = cc1->unk3;
            cc1->mFlag |= 2;
        }
    }
}

void dEn_c::Normal_VsEnHitCheck(dCc_c *cc1, dCc_c *cc2) {}

void dEn_c::Normal_VsPlHitCheck(dCc_c *cc1, dCc_c *cc2) {
    dActor_c *owner = (dActor_c *) cc2->mpOwner;
    if (!LineBoundaryCheck(owner)){
        setDamage(owner);
    }
}

void dEn_c::Normal_VsYoshiHitCheck(dCc_c *cc1, dCc_c *cc2) {
    dActor_c *owner = (dActor_c *) cc2->mpOwner;
    if (!LineBoundaryCheck(owner)){
        setDamage(owner);
    }
}

bool dEn_c::EnDamageCheck(dCc_c *cc1, dCc_c *cc2) {
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_SHELL) {
        if (hitCallback_Shell(cc1, cc2)) {
            return true;
        }
    }
    return false;
}

bool dEn_c::PlDamageCheck(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *owner = (daPlBase_c *) cc2->mpOwner;
    if (owner->mKind != 1) {
        return false;
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_STAR && hitCallback_Star(cc1, cc2)) {
        return true;
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_HIP_ATTK) {
        if (owner->isMameAction()) {
            return false;
        }
        if (hitCallback_HipAttk(cc1, cc2)) {
            return true;
        }
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_SLIP) {
        if (cc1->mCcData.mAttackCategoryInteract & (1 << dCc_c::ATTACK_SLIP) && hitCallback_Slip(cc1, cc2)) {
            return true;
        }
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_PENGUIN_SLIDE) {
        if (mActorProperties & 0x200) {
            return false;
        }
        if (hitCallback_PenguinSlide(cc1, cc2)) {
            return true;
        }
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_SPIN_FALL && hitCallback_Spin(cc1, cc2)) {
        return true;
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_CANNON && hitCallback_Cannon(cc1, cc2)) {
        return true;
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_WIRE_NET && hitCallback_WireNet(cc1, cc2)) {
        return true;
    }
    return false;
}

bool dEn_c::YoshiDamageCheck(dCc_c *cc1, dCc_c *cc2) {
    daPlBase_c *owner = (daPlBase_c *) cc2->mpOwner;
    if (owner->mKind != 2) {
        return false;
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_STAR && hitCallback_Star(cc1, cc2)) {
        return true;
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_HIP_ATTK && hitCallback_YoshiHipAttk(cc1, cc2)) {
        return true;
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_SLIP) {
        if (cc1->mCcData.mAttackCategoryInteract & (1 << dCc_c::ATTACK_SLIP) && hitCallback_Slip(cc1, cc2)) {
            return true;
        }
    }
    return false;
}

bool dEn_c::EtcDamageCheck(dCc_c *cc1, dCc_c *cc2) {
    dEn_c *owner = (dEn_c *) cc1->mpOwner;
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_SHELL && owner->hitCallback_Shell(cc1, cc2)) {
        return true;
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_FIRE) {
        if (cc1->mCcData.mAttackCategoryInteract & (1 << dCc_c::ATTACK_FIRE) && owner->hitCallback_Fire(cc1, cc2)) {
            return true;
        }
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_YOSHI_FIRE) {
        if (cc1->mCcData.mAttackCategoryInteract & (1 << dCc_c::ATTACK_YOSHI_FIRE) && owner->hitCallback_YoshiFire(cc1, cc2)) {
            return true;
        }
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_FIRE_2 && owner->hitCallback_Fire(cc1, cc2)) {
        return true;
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_ICE && cc1->mCcData.mAttackCategoryInteract & (1 << dCc_c::ATTACK_ICE) ||
        cc2->mCcData.mAttackCategory == dCc_c::ATTACK_ICE_2 && cc1->mCcData.mAttackCategoryInteract & (1 << dCc_c::ATTACK_ICE_2)) {
        if (owner->hitCallback_Ice(cc1, cc2)) {
            return true;
        }
    }
    if (cc2->mCcData.mAttackCategory == dCc_c::ATTACK_YOSHI_BULLET) {
        if (cc1->mCcData.mAttackCategoryInteract & (1 << dCc_c::ATTACK_YOSHI_BULLET) && owner->hitCallback_YoshiBullet(cc1, cc2)) {
            return true;
        }
    }
    return false;
}

void dEn_c::hitYoshiEat(dCc_c *cc1, dCc_c *cc2) {}

bool dEn_c::getPl_LRflag(const mVec3_c &pos) {
    mVec2_c playerPos;
    mVec2_c tmp(pos.x, pos.y);
    if (searchNearPlayer_Main(playerPos, tmp) == nullptr) {
        return false;
    }
    return playerPos.x < 0;
}

bool dEn_c::getPl_UDflag(const mVec3_c &pos) {
    mVec2_c playerPos;
    mVec2_c tmp(pos.x, pos.y);
    if (searchNearPlayer_Main(playerPos, tmp) == nullptr) {
        return false;
    }
    return playerPos.y < 0;
}

bool dEn_c::CeilCheck(float y, dCc_c *cc) {
    return dBgParameter_c::getInstance()->check(y + cc->mCcData.mOffsetY, cc->mCcData.mHeight);
}

bool dEn_c::carry_check(dActor_c *actor) {
    dAcPy_c *pl = (dAcPy_c *) actor;
    if (pl->FUN_8012e540(this, true)) {
        mPlayerNo = *actor->getPlrNo();
        return true;
    }
    return false;
}

void dEn_c::checkWallAndBg() {
    float v = l_base_speedX[mDirection];
    mBc.checkWall(&v);
    mVec3_c truePos = mPos;
    truePos += mCenterOffs;
    mBc.checkBg(truePos.x, truePos.y, mLayer, l_Ami_Line[mBackFence], 0x819);
}

int dEn_c::Enfumi_check(dCc_c *cc1, dCc_c *cc2, int step) {
    dActor_c *owner;
    FumiCcInfo_c fumiInfo(cc1, cc2);
    int fumiRes = mFumiProc.operate(fumiInfo, 1);
    owner = (dActor_c *) cc2->getOwner();
    switch (fumiRes) {
        case 1:
            if ((int) owner->mKind == 1) {
                if (step == 0) {
                    fumiSE(owner);
                    fumiEffect(owner);
                } else if (step == 1) {
                    fumistepSE(owner);
                }
                FumiJumpSet(owner);
                FumiScoreSet(owner);
            } else {
                if (step == 0) {
                    yoshifumiSE(owner);
                    yoshifumiEffect(owner);
                } else if (step == 1) {
                    yoshifumistepSE(owner);
                }
                YoshiFumiJumpSet(owner);
                YoshiFumiScoreSet(owner);
            }
            break;
        case 2:
            MameFumiJumpSet(owner);
            mamefumiSE(owner);
            mamefumiEffect(owner);
            break;
        case 3:
            if (step == 0) {
                spinfumiSE(owner);
                spinfumiEffect(owner);
            } else if (step == 1) {
                fumistepSE(owner);
            }
            SpinFumiJumpSet(owner);
            SpinFumiScoreSet(owner);
            break;
    }
    return fumiRes;
}

void dEn_c::FumiScoreSet(dActor_c *actor) {
    setFumiComboScore(actor);
}

void dEn_c::FumiJumpSet(dActor_c *actor) {
    PlayerFumiJump(actor, dAcPy_c::msc_JUMP_SPEED + 0.2815f);
}

void dEn_c::MameFumiJumpSet(dActor_c *actor) {
    float jumpSpeed = (dAcPy_c::msc_JUMP_SPEED + 0.2815f);
    jumpSpeed *= 0.8125f;
    if (mSpeed.y > 0.0f) {
        jumpSpeed += mSpeed.y * 0.2f;
    }
    PlayerFumiJump(actor, jumpSpeed);
}

void dEn_c::YoshiFumiJumpSet(dActor_c *actor) {
    PlayerFumiJump(actor, dAcPy_c::msc_JUMP_SPEED + 0.2815f);
}

void dEn_c::YoshiFumiScoreSet(dActor_c *actor) {
    FumiScoreSet(actor);
}

void dEn_c::SpinFumiJumpSet(dActor_c *actor) {
    FumiJumpSet(actor);
}

void dEn_c::SpinFumiScoreSet(dActor_c *actor) {
    FumiScoreSet(actor);
}

void dEn_c::PlayerFumiJump(dActor_c *actor, float param_2) {
    ((daPlBase_c *) actor)->vf3fc(param_2, actor->mSpeedF, 1, 0, 2);
    dEnemyMng_c::m_instance->m138 = 1;
}

void dEn_c::setFumiComboScore(dActor_c *actor) {
    int treadCount = ((daPlBase_c *) actor)->calcAndGetTreadCount();
    if (treadCount >= 0) {
        mVec3_c pos = getCenterPos();
        float scY = dScoreMng_c::smc_SCORE_Y;
        pos.y = mPos.y + scY;
        switch (mCounterCont.mClapMode) {
            case 1: {
                dScoreMng_c *instance = dScoreMng_c::getInstance();
                instance->ScoreSet(pos, treadCount, *actor->getPlrNo(), 1);
                break;
            }
            case 2: {
                dScoreMng_c *instance = dScoreMng_c::getInstance();
                instance->ScoreSet2(pos, treadCount, *actor->getPlrNo());
                break;
            }
        }
    }
}

bool dEn_c::checkComboClap(int num) {
    static const int claps[] = { 7, 7, 4 };
    return num >= claps[mCounterCont.mClapMode];
}

void dEn_c::fumiSE(dActor_c *actor) {
    const static dAudio::SoundEffectID_t cs_combo_se[] = {
        SE_EMY_FUMU_1,
        SE_EMY_FUMU_2,
        SE_EMY_FUMU_3,
        SE_EMY_FUMU_4,
        SE_EMY_FUMU_5,
        SE_EMY_FUMU_6,
        SE_EMY_FUMU_7,
        SE_EMY_FUMU_7,
        SE_EMY_FUMU_7
    };

    int count = ((daPlBase_c *) actor)->getTreadCount();
    if (count >= ARRAY_SIZE(cs_combo_se)) {
        count = 8;
    };
    if (checkComboClap(count)) {
        dMultiMng_c::mspInstance->setClapSE();
    }

    cs_combo_se[count].playEmySound(getCenterPos(), 0);
}

void dEn_c::spinfumiSE(dActor_c *actor) {
    const static dAudio::SoundEffectID_t cs_combo_se[] = {
        SE_EMY_DOWN_SPIN_1,
        SE_EMY_DOWN_SPIN_2,
        SE_EMY_DOWN_SPIN_3,
        SE_EMY_DOWN_SPIN_4,
        SE_EMY_DOWN_SPIN_5,
        SE_EMY_DOWN_SPIN_6,
        SE_EMY_DOWN_SPIN_7,
        SE_EMY_DOWN_SPIN_7,
        SE_EMY_DOWN_SPIN_7
    };

    int count = ((daPlBase_c *) actor)->getTreadCount();
    if (count >= ARRAY_SIZE(cs_combo_se)) {
        count = 8;
    };
    if (checkComboClap(count)) {
        dMultiMng_c::mspInstance->setClapSE();
    }

    cs_combo_se[count].playEmySound(getCenterPos(), 0);
}

void dEn_c::yoshifumiSE(dActor_c *actor) {
    const static dAudio::SoundEffectID_t cs_combo_se[] = {
        SE_EMY_YOSHI_FUMU_1,
        SE_EMY_YOSHI_FUMU_2,
        SE_EMY_YOSHI_FUMU_3,
        SE_EMY_YOSHI_FUMU_4,
        SE_EMY_YOSHI_FUMU_5,
        SE_EMY_YOSHI_FUMU_6,
        SE_EMY_YOSHI_FUMU_7,
        SE_EMY_YOSHI_FUMU_7,
        SE_EMY_YOSHI_FUMU_7
    };

    int count = ((daPlBase_c *) actor)->getTreadCount();
    if (count >= ARRAY_SIZE(cs_combo_se)) {
        count = 8;
    };
    if (checkComboClap(count)) {
        dMultiMng_c::mspInstance->setClapSE();
    }

    cs_combo_se[count].playEmySound(getCenterPos(), 0);
}

void dEn_c::mamefumiSE(dActor_c *actor) {
    dAudio::SoundEffectID_t(SE_EMY_MAME_STEP).playEmySound(getCenterPos(), 0);
}

void dEn_c::fumistepSE(dActor_c *actor) {
    dAudio::SoundEffectID_t(SE_EMY_CMN_STEP).playEmySound(getCenterPos(), 0);
}

void dEn_c::yoshifumistepSE(dActor_c *actor) {
    mVec3_c centerPos = getCenterPos();
    dAudio::SoundEffectID_t(SE_EMY_YOSHI_STEP).playEmySound(centerPos, 0);
}

void dEn_c::fumiEffect(dActor_c *actor) {
    if (actor->mKind != 1 && actor->mKind != 2) {
        return;
    }
    daPlBase_c *pl = (daPlBase_c *) actor;
    mVec3_c efPos;
    efPos.x = pl->getAnkleCenterX();
    efPos.y = pl->getAnkleCenterY();
    efPos.z = 5500.0f;
    mEf::createEffect("Wm_en_hit", 0, &efPos, nullptr, nullptr);
}

void dEn_c::spinfumiEffect(dActor_c *actor) {
    mVec3_c efPos;
    efPos.x = actor->mPos.x;
    efPos.y = actor->mPos.y;
    efPos.z = 5500.0f;
    mEf::createEffect("Wm_en_hit", 0, &efPos, nullptr, nullptr);
}

void dEn_c::yoshifumiEffect(dActor_c *actor) {}

void dEn_c::mamefumiEffect(dActor_c *actor) {}

u32 dEn_c::EnBgCheck() {
    u32 flags = 0;
    u32 footCheckRes = EnBgCheckFoot();
    if (footCheckRes) {
        flags |= 1;
        mSpeed.y = 0.0f;
    }
    u32 checkHeadRes = mBc.checkHead(footCheckRes);
    if (checkHeadRes) {
        flags |= 2;
    }
    u32 bgCheckWallRes = EnBgCheckWall();
    if (bgCheckWallRes) {
        flags |= 4;
    }
    u32 arg = checkHeadRes | bgCheckWallRes;
    u32 actArg = arg | footCheckRes;
    if (mBc.mFlags & 0x8000 && mBc.mFlags & 3 && mBc.mpRc != nullptr) {
        int v = 0;
        if (mSpeed.y > 0.0f) {
            v |= 1;
        }
        if (!mBc.mpRc->check2(actArg & 0x1f6000, v, 0)) {
            mBc.mFlags &= 0xffff7fff;
        }
    }
    return flags;
}

bool dEn_c::EnBgCheckFoot() {
    if (!mBc.hasSensorFoot()) {
        mFootRelated2.set(0.0f, 0.0f, 0.0f);
        return false;
    }
    bool res = mBc.checkFootEnm();
    mFootAttr3 = false;
    mFootAttr1 = false;
    if (mBc.isFoot()) {
        u32 footAttr = mBc.getFootAttr();
        if ((u16) footAttr == 3) {
            mFootAttr3 = true;
        }
        if ((u16) footAttr == 1) {
            mFootAttr1 = true;
        }
        mFootRelated = mBc.m_20;
        mFootRelated2.set(0.0f, 0.0f, 0.0f);
    } else {
        mFootRelated.set(0.0f, 0.0f, 0.0f);
    }
    return res;
}

u32 dEn_c::EnBgCheckWall() {
    if (!mBc.hasSensorWall()) {
        return 0;
    }
    float tmpX = mBc.m_4c;
    u8 idx = mPos.x - tmpX < 0.0f;
    float speed = l_base_speedX[idx];
    u32 flags = mBc.checkWallEnm(&speed);
    speed = l_base_speedX[idx ^ 1];
    u32 wallFlags = mBc.checkWallEnm(&speed);
    if (mBgCollFlags & 3) {
        wallFlags |= mBc.checkWallEnm(&speed);
    }
    mBc.mFlags |= flags | wallFlags;
    return flags;
}

void dEn_c::WaterCheck(mVec3_c &pos, float h) {
    if (m_46) {
        return;
    }
    int waterLineRes = WaterLineProc(pos, h);
    if (waterLineRes == 3) {
        setDeathInfo_Smoke(nullptr);
        m_46 = true;
    } else if (waterLineRes != 0 && mNoRespawn) {
        m_46 = true;
    }
}

bool dEn_c::LineBoundaryCheck(dActor_c *actor) {
    daPlBase_c *pl = (daPlBase_c *) actor;
    if ((pl->mPos.z > 0.0f && mBackFence == 1) || (pl->mPos.z < 0.0f && mBackFence == 0)) {
        if (pl->mFlags & 0x80000 || pl->mFlags & 0x100000) {
            return true;
        }
    }
    return false;
}

dBc_c::WaterCheckResult_e dEn_c::WaterLineProc(const mVec3_c &pos, float h) {
    float height = 0.0f;
    dBc_c::WaterCheckResult_e res = dBc_c::checkWater(pos.x, pos.y, mLayer, &height);
    mVec3_c waterPos(mPos.x, height, 6500.0f);
    switch (res) {
        case dBc_c::R1:
        case dBc_c::R2:
            if (!m_49) {
                m_49 = true;
                mSaveAccelY = mAccelY;
                mSaveSpeedMaxY = mSpeedMax.y;
                mSaveMaxFallSpeed = mMaxFallSpeed;
                setWaterSpeed();
                waterSplashEffect(waterPos, h);
            }
            break;
        case dBc_c::R3:
            if (!m_49) {
                m_49 = true;
                yoganSplashEffect(waterPos, h);
            }
            break;
        case dBc_c::R4:
            if (!m_49) {
                m_49 = true;
                poisonSplashEffect(waterPos, h);
            }
            break;
        default:
            if (m_49) {
                mAccelY = mSaveAccelY;
                mSpeedMax.y = mSaveSpeedMaxY;
                mMaxFallSpeed = mSaveMaxFallSpeed;
            }
            m_49 = false;
            break;
    }
    return res;
}

void dEn_c::yoganSplashEffect(const mVec3_c &pos, float h) {
    mVec3_c shiftedPos(pos, 6500.0f);
    mVec3_c weirdPos(h, h, h);
    u32 flags = mLayer << 16 | 5;
    dEffActorMng_c::m_instance->createWaterSplashEff(shiftedPos, flags, -1, weirdPos);

    dAudio::SoundEffectID_t(SE_OBJ_CMN_SPLASH_LAVA).playMapSound(shiftedPos, 0);

    if (h < 1.0f) {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 16);
    } else {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 14);
    }
}

void dEn_c::poisonSplashEffect(const mVec3_c &pos, float h) {
    mVec3_c shiftedPos(pos, 6500.0f);
    mVec3_c weirdPos(h, h, h);
    u32 flags = mLayer << 16 | 7;
    dEffActorMng_c::m_instance->createWaterSplashEff(shiftedPos, flags, -1, weirdPos);

    dAudio::SoundEffectID_t(SE_OBJ_CMN_SPLASH_POISON).playMapSound(pos, 0);

    if (h < 1.0f) {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 23);
    } else {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 21);
    }
}

bool dEn_c::EnLavaCheck(const mVec3_c &pos) {
    float height = 0.0f;
    dBc_c::WaterCheckResult_e res = dBc_c::checkWater(pos.x, pos.y, mLayer, &height);
    if (res == dBc_c::R3) {
        if (height < 0.0f && mLastPos.y > height && mPos.y <= height) {
            yoganSplashEffect(mVec3_c(pos.x, height, 6000.0f), 1.0f);
        }
        return true;
    }
    return false;
}

bool dEn_c::EnWaterCheck(const mVec3_c &pos) {
    if (EnWaterFlagCheck(pos) != dBc_c::R0) {
        return true;
    }
    mSpeedMax.y = -4.0f;
    mMaxFallSpeed = -4.0f;
    return false;
}

bool dEn_c::EnWaterFlagCheck(const mVec3_c &pos) {
    switch (dBc_c::checkWater(pos.x, pos.y, mLayer, nullptr)) {
        case dBc_c::R1:
        case dBc_c::R2:
            setWaterSpeed();
            return true;
        default:
            return false;
    }
}

void dEn_c::setWaterSpeed() {
    mAccelY = smc_WATER_GRAVITY;

    if (mSpeedMax.y < -smc_WATER_YMAXSPD) {
        mSpeedMax.y = -smc_WATER_YMAXSPD;
    } else if (mSpeedMax.y > smc_WATER_YMAXSPD) {
        mSpeedMax.y = smc_WATER_YMAXSPD;
    }

    if (mMaxFallSpeed < smc_WATER_FALLMAXSPD) {
        mMaxFallSpeed = smc_WATER_FALLMAXSPD;
    } else if (mMaxFallSpeed > -smc_WATER_FALLMAXSPD) {
        mMaxFallSpeed = -smc_WATER_FALLMAXSPD;
    }

    if (mSpeed.y < -smc_WATER_YMAXSPD) {
        mSpeed.y = -smc_WATER_YMAXSPD;
    } else if (mSpeed.y > smc_WATER_YMAXSPD) {
        mSpeed.y = smc_WATER_YMAXSPD;
    }
}

bool dEn_c::Area_X_check(float x) {
    for (int i = 0; i < 4; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && daPyMng_c::checkPlayer(i)) {
            mVec3_c tmp = player->mPos;
            float diff = std::fabs(tmp.x - mPos.x);
            if (diff < x) {
                return true;
            }
        }
    }
    return false;
}

bool dEn_c::Area_XY_check(float x, float y) {
    for (int i = 0; i < 4; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && daPyMng_c::checkPlayer(i)) {
            mVec3_c tmp = player->mPos;
            if (std::fabs(tmp.x - mPos.x) < x && std::fabs(tmp.y - mPos.y) < y) {
                return true;
            }
        }
    }
    return false;
}

bool dEn_c::PlayerCarryCheck(dActor_c *actor) {
    for (int i = 0; i < 4; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && fManager_c::searchBaseByID(player->mCarryActorID) == this) {
            return true;
        }
    }
    return false;
}

mVec3_c dEn_c::calcCarryPos(const mVec3_c &pos) {
    dAcPy_c *player = daPyMng_c::getPlayer(mPlayerNo);
    if (player->isStatus(4)) {
        return mPos;
    }
    mMtx_c mtx;
    mMtx_c transposeMtx;
    mVec3_c res;
    mVec3_c carryPos = player->getCarryPos();
    mtx.trans(carryPos);
    mtx.concat(player->getModel()->getMtx());
    transposeMtx.trans(pos.x, pos.y, pos.z);
    mtx.concat(transposeMtx).multVecZero(res);
    return res;
}

bool dEn_c::turnangle_calc(const short *ang1, const short *ang2) {
    mAngle.y += ang2[mDirection];
    if (ang1[0] <= mAngle.y || mAngle.y <= ang1[1]) {
        mAngle.y = ang1[mDirection];
        return true;
    }
    return false;
}

void dEn_c::slipBound(dActor_c *actor) {
    static const float vals[] = { 1.5f, -1.5f };
    daPlBase_c *pl = (daPlBase_c *) actor;

    u8 idx = !(pl->mPos.x >= mPos.x);
    pl->vf3fc(3.0f, vals[idx], 1, 0, 0);

    int plrNo = *pl->getPlrNo();
    mCounterCont.mCounters2[plrNo] = 3;
}

void dEn_c::setEatTongue(dActor_c *actor) {
    removeCc();
    mPreEatScale = mScale;
    changeState(StateID_EatIn);
}

void dEn_c::setEatTongueOff(dActor_c *actor) {
    reviveCc();
    setAfterEatScale();
    changeState(*mStateMgr.getOldStateID());
}

bool dEn_c::setEatSpitOut(dActor_c *actor) {
    calcSpitOutPos(actor);
    int plrNo = *actor->getPlrNo();
    mCounterCont.mCounters2[plrNo] = 16;
    mDirection = actor->mDirection;
    reviveCc();
    setAfterEatScale();
    changeState(StateID_EatOut);
    return true;
}

void dEn_c::calcEatInScale(dActor_c *actor) {
    dActor_c::calcEatInScale(actor);
    mBoyoMng.mCounter = 0;
}

void dEn_c::setNicePoint_Death() {
    int v = mBlockHit.mData.m1d;
    if (mBlockHit.mData.m1d >= 0 && mBlockHit.mData.m1d < PLAYER_COUNT) {
        dMultiMng_c::mspInstance->incEnemyDown(v);
    }
}

void dEn_c::fireballInvalid(dCc_c *cc1, dCc_c *cc2) {
    dAudio::g_pSndObjMap->startSound(SE_OBJ_FIREBALL_DISAPP, cc2->mpOwner->mPos, 0);
}

void dEn_c::iceballInvalid(dCc_c *cc1, dCc_c *cc2) {
    dBaseActor_c *owner = cc2->mpOwner;
    dAudio::g_pSndObjMap->startSound(SE_OBJ_PNGN_ICEBALL_DISAPP, owner->mPos, 0);

    mVec3_c shiftedPos;
    float x, y;
    y = owner->mPos.y;
    x = owner->mPos.x;
    shiftedPos.set(x, y, 5500.0f);
    EffectManager_c::SetIceBallMissshitEffect(&shiftedPos);
}

void dEn_c::setDamage(dActor_c *actor) {
    daPlBase_c *pl = (daPlBase_c *) actor;
    pl->setDamage(this, daPlBase_c::DAMAGE_NONE);
}

void dEn_c::boyonInit() {
    mBoyoMng.mScale = mBoyoMng.mpOwner->mScale;
}

void dEn_c::boyonBegin() {
    mBoyoMng.begin(dEnBoyoMng_c::smc_BOYON_TIME, dEnBoyoMng_c::smc_DELTA_SCALE);
}

void dEn_c::block_hit_init() {
    bool tmp = blockHitRelated;
    s8 plrNo = *getPlrNo();

    mVec2_c pos2(mPos.x, mPos.y);
    mVec3_c shiftedPos(pos2, 5500.0f);

    hitdamageEffect(shiftedPos);
    dAudio::SoundEffectID_t(SE_EMY_DOWN).playEmySound(mPos, 0);

    block_hit_data_s data = {
        smc_DEADFALL_GRAVITY[(u8) (int) tmp],
        3.0f,
        -4.0f,
        -0.1875f,
        &StateID_DieFall,
        -1,
        -1,
        tmp,
        plrNo
    };
    mBlockHit.set(data);
}
