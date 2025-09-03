#include <game/bases/d_enemy.hpp>
#include <game/bases/d_bc.hpp>
#include <game/bases/d_quake.hpp>
#include <game/bases/d_a_player.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_score_manager.hpp>
#include <game/bases/d_ef.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <game/bases/d_effactor_mng.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_multi_manager.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_enemy_manager.hpp>
#include <game/mLib/m_effect.hpp>
#include <constants/sound_list.h>

const s8 l_EnMuki[] = { 1, -1 };
const s16 l_base_angleY[] = { 0x2000, -0x2000 };
const s16 l_base_angleY_add[] = { 0x400, -0x400 };
const float l_base_fall_speed_x[] = { 1.5f, -1.5f };

const float dEn_c::smc_WATER_GRAVITY = -0.0625f;
const float dEn_c::smc_WATER_YMAXSPD = 1.5f;
const float dEn_c::smc_WATER_FALLMAXSPD = -1.5f;
const float dEn_c::smc_WATER_ROLL_DEC_RATE = 0.5f;

dEn_c::dEn_c() :
    m_24(0), mFootPush(mVec3_c(0.0f, 0.0f, 0.0f)),
    mKilledByLiquid(false), mFootAttr3(false), mFootAttr1(false), mDeathInfo(),
    mBoyoMng(this), mIceMng(this),
    mTimer1(0), mTimer2(0), mCombo(dEnCombo_c::COMBO_REGULAR), mFumiProc(this)
{
    mFumiProc.refresh(new NonUniqueFumiCheck_c());

    mAccelY = -0.1875f;
    mSpeedMax.set(0.0f, -4.0f, 0.0f);
    mAirAccelY = -0.1875f;
    mAirSpeedMaxY = -4.0f;
    mAirMaxFallSpeed = -4.0f;

    setKind(STAGE_ACTOR_ENEMY);
    mExecStopMask = BIT_FLAG(STAGE_ACTOR_ENEMY);

    mFlags = 0;

    for (int i = 0; i < PLAYER_COUNT; i++) {
        mNoHitPlayer.mTimer[i] = 0;
    }
}

dEn_c::~dEn_c() {}

void dEn_c::postCreate(fBase_c::MAIN_STATE_e status) {
    boyonInit();
    if (dBc_c::checkWater(mPos.x, mPos.y, mLayer, nullptr)) {
        mInLiquid = true;
    }

    dActor_c::postCreate(status);
}

int dEn_c::preExecute() {
    if (dActor_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    if (!mNoRespawn && !isState(StateID_Ice) && isQuakeDamage()) {
        if (dQuake_c::m_instance->mFlags & dQuake_c::FLAG_1) {
            if (checkDispIn()) {
                setDeathInfo_Quake(0);
            }
        } else if (dQuake_c::m_instance->mFlags & dQuake_c::FLAG_2) {
            if (checkDispIn()) {
                setDeathInfo_Quake(1);
            }
        } else if (dQuake_c::m_instance->mFlags & dQuake_c::FLAG_0) {
            quakeAction();
        }
    }

    if (mDeathInfo.mIsDead) {
        if (!mNoRespawn) {
            mNoRespawn = true;
            setNicePoint_Death();
            changeState(*mDeathInfo.mDeathState);
        }
        mDeathInfo.mIsDead = false;
    } else {
        if (mTimer3 != 0) {
            mTimer3--;
        }
        if (mTimer1 != 0) {
            mTimer1--;
        }
        if (mTimer2 != 0) {
            mTimer2--;
        }
        for (u32 i = 0; i < PLAYER_COUNT; i++) {
            mNoHitPlayer.update(i);
        }
    }

    calcBoyonScale();
    return mDeleteRequested ? NOT_READY : SUCCEEDED;
}

void dEn_c::postExecute(fBase_c::MAIN_STATE_e status) {
    if (status == SUCCESS && !(dActor_c::mExecStop & BIT_FLAG(ACTOR_MAP_ENEMY))) {
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
    if (mFlags & EN_IS_HARD) {
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
        float speedX = std::fabs(mSpeed.x);
        float cos = mAng(ang).cos();
        float sin = mAng(ang).sin();
        move.x = speedX * cos;
        move.y = mSpeed.y + speedX * sin;
        move.z = mSpeed.z;

        if (mInLiquid) {
            mFootPush2.set(0.0f, 0.0f, 0.0f);
            move.x /= 2;
        } else {
            bool ok = mDeathInfo.mIsDead || mNoRespawn;
            if (!ok && mSpeed.y <= 0.0f) {
                move.x += mFootPush2.x;
            }
        }
    }
    move += mFootPush;
    mPos += move;
}

void dEn_c::Bound(float epsY, float scaleX, float scaleY) {
    if (!mBc.isFoot()) {
        return;
    }

    if (scaleX < 1.0f) {
        mSpeed.x *= scaleX;
        if (std::fabs(mSpeed.x) < 1.0f / 16.0f) {
            mSpeed.x = 0.0f;
        }
    }

    if (mSpeed.y < 0.0f) {
        mSpeed.y = -mSpeed.y;
        mSpeed.y *= scaleY;
        if (mSpeed.y < epsY) {
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

void dEn_c::normal_collcheck(dCc_c *self, dCc_c *other) {
    dEn_c *actor1 = (dEn_c *) self->getOwner();
    dActor_c *actor2 = (dActor_c *) other->getOwner();

    if (actor1->mDeathInfo.mIsDead) {
        self->mInfo |= CC_NO_HIT;
        return;
    }

    u32 kind = actor2->mKind;
    if (kind == STAGE_ACTOR_ENEMY) {
        if (actor1->EnDamageCheck(self, other)) {
            self->mInfo |= CC_NO_HIT;
        } else {
            actor1->Normal_VsEnHitCheck(self, other);
        }
    } else if (kind == STAGE_ACTOR_PLAYER) {
        if (actor1->mFlags & EN_FLAG_24 || !CeilCheck(actor1->mPos.y, self)) {
            if (actor1->PlDamageCheck(self, other)) {
                actor1->mCcValue = self->mCanBounce;
                self->mInfo |= CC_NO_HIT;
            } else if (other->mCcData.mKind != CC_KIND_PLAYER_ATTACK) {
                s8 *plrNo = actor2->getPlrNo();
                if (*plrNo >= 0 && *plrNo < PLAYER_COUNT) {
                    if (actor1->mNoHitPlayer.mTimer[*plrNo] == 0) {
                        actor1->mNoHitPlayer.mTimer[*plrNo] = smc_NO_HIT_PLAYER_TIMER_DEFAULT;
                        actor1->Normal_VsPlHitCheck(self, other);
                    }
                }
            }
        }
    } else if (kind == STAGE_ACTOR_YOSHI) {
        s8 *plrNo = actor2->getPlrNo();
        if (*plrNo >= 0 && *plrNo < PLAYER_COUNT) {
            if (other->mCcData.mAttack == CC_ATTACK_YOSHI_EAT) {
                actor1->hitYoshiEat(self, other);
            } else {
                if (!CeilCheck(actor1->mPos.y, self)) {
                    if (actor1->YoshiDamageCheck(self, other)) {
                        actor1->mCcValue = self->mCanBounce;
                        self->mInfo |= CC_NO_HIT;
                    } else if (actor1->mNoHitPlayer.mTimer[*plrNo] == 0) {
                        actor1->mNoHitPlayer.mTimer[*plrNo] = smc_NO_HIT_PLAYER_TIMER_DEFAULT;
                        actor1->Normal_VsYoshiHitCheck(self, other);
                    }
                }
            }
        }
    } else {
        if (actor1->EtcDamageCheck(self, other)) {
            actor1->mCcValue = self->mCanBounce;
            self->mInfo |= CC_NO_HIT;
        }
    }
}

void dEn_c::Normal_VsEnHitCheck(dCc_c *self, dCc_c *other) {}

void dEn_c::Normal_VsPlHitCheck(dCc_c *self, dCc_c *other) {
    dActor_c *owner = (dActor_c *) other->getOwner();
    if (!LineBoundaryCheck(owner)){
        setDamage(owner);
    }
}

void dEn_c::Normal_VsYoshiHitCheck(dCc_c *self, dCc_c *other) {
    dActor_c *owner = (dActor_c *) other->getOwner();
    if (!LineBoundaryCheck(owner)){
        setDamage(owner);
    }
}

bool dEn_c::EnDamageCheck(dCc_c *self, dCc_c *other) {
    if (other->mCcData.mAttack == CC_ATTACK_SHELL) {
        if (hitCallback_Shell(self, other)) {
            return true;
        }
    }
    return false;
}

bool dEn_c::PlDamageCheck(dCc_c *self, dCc_c *other) {
    daPlBase_c *owner = (daPlBase_c *) other->getOwner();
    if (owner->mKind != STAGE_ACTOR_PLAYER) {
        return false;
    }

    if (other->mCcData.mAttack == CC_ATTACK_STAR && hitCallback_Star(self, other)) {
        return true;
    }
    if (other->mCcData.mAttack == CC_ATTACK_HIP_ATTACK) {
        if (owner->isMameAction()) {
            return false;
        }
        if (hitCallback_HipAttk(self, other)) {
            return true;
        }
    }
    if (other->mCcData.mAttack == CC_ATTACK_SLIP) {
        if (self->mCcData.mVsDamage & (1 << CC_ATTACK_SLIP) && hitCallback_Slip(self, other)) {
            return true;
        }
    }
    if (other->mCcData.mAttack == CC_ATTACK_PENGUIN_SLIDE) {
        if (mActorProperties & 0x200) {
            return false;
        }
        if (hitCallback_PenguinSlide(self, other)) {
            return true;
        }
    }
    if (other->mCcData.mAttack == CC_ATTACK_SPIN_FALL && hitCallback_Spin(self, other)) {
        return true;
    }
    if (other->mCcData.mAttack == CC_ATTACK_CANNON && hitCallback_Cannon(self, other)) {
        return true;
    }
    if (other->mCcData.mAttack == CC_ATTACK_WIRE_NET && hitCallback_WireNet(self, other)) {
        return true;
    }
    return false;
}

bool dEn_c::YoshiDamageCheck(dCc_c *self, dCc_c *other) {
    daPlBase_c *owner = (daPlBase_c *) other->getOwner();
    if (owner->mKind != STAGE_ACTOR_YOSHI) {
        return false;
    }

    if (other->mCcData.mAttack == CC_ATTACK_STAR && hitCallback_Star(self, other)) {
        return true;
    }
    if (other->mCcData.mAttack == CC_ATTACK_HIP_ATTACK && hitCallback_YoshiHipAttk(self, other)) {
        return true;
    }
    if (other->mCcData.mAttack == CC_ATTACK_SLIP) {
        if (self->mCcData.mVsDamage & (1 << CC_ATTACK_SLIP) && hitCallback_Slip(self, other)) {
            return true;
        }
    }
    return false;
}

bool dEn_c::EtcDamageCheck(dCc_c *self, dCc_c *other) {
    dEn_c *owner = (dEn_c *) self->getOwner();
    if (other->mCcData.mAttack == CC_ATTACK_SHELL && owner->hitCallback_Shell(self, other)) {
        return true;
    }
    if (other->mCcData.mAttack == CC_ATTACK_FIREBALL) {
        if (self->mCcData.mVsDamage & (1 << CC_ATTACK_FIREBALL) && owner->hitCallback_Fire(self, other)) {
            return true;
        }
    }
    if (other->mCcData.mAttack == CC_ATTACK_YOSHI_FIRE) {
        if (self->mCcData.mVsDamage & (1 << CC_ATTACK_YOSHI_FIRE) && owner->hitCallback_YoshiFire(self, other)) {
            return true;
        }
    }
    if (other->mCcData.mAttack == CC_ATTACK_FIRE_2 && owner->hitCallback_Fire(self, other)) {
        return true;
    }
    if (other->mCcData.mAttack == CC_ATTACK_ICEBALL && self->mCcData.mVsDamage & (1 << CC_ATTACK_ICEBALL) ||
        other->mCcData.mAttack == CC_ATTACK_ICE_2 && self->mCcData.mVsDamage & (1 << CC_ATTACK_ICE_2)) {
        if (owner->hitCallback_Ice(self, other)) {
            return true;
        }
    }
    if (other->mCcData.mAttack == CC_ATTACK_YOSHI_BULLET) {
        if (self->mCcData.mVsDamage & (1 << CC_ATTACK_YOSHI_BULLET) && owner->hitCallback_YoshiBullet(self, other)) {
            return true;
        }
    }
    return false;
}

void dEn_c::hitYoshiEat(dCc_c *self, dCc_c *other) {}

bool dEn_c::getPl_LRflag(const mVec3_c &pos) {
    mVec2_c playerPos;
    mVec2_c pos2D(pos.x, pos.y);
    if (searchNearPlayer_Main(playerPos, pos2D) == nullptr) {
        return false;
    }
    return playerPos.x < 0;
}

bool dEn_c::getPl_UDflag(const mVec3_c &pos) {
    mVec2_c playerPos;
    mVec2_c pos2D(pos.x, pos.y);
    if (searchNearPlayer_Main(playerPos, pos2D) == nullptr) {
        return false;
    }
    return playerPos.y < 0;
}

bool dEn_c::CeilCheck(float y, dCc_c *cc) {
    return dBgParameter_c::getInstance()->check(y + cc->mCcData.mOffset.y, cc->mCcData.mSize.y);
}

bool dEn_c::carry_check(dActor_c *actor) {
    dAcPy_c *pl = (dAcPy_c *) actor;
    if (pl->FUN_8012e540(this, true)) {
        mPlayerNo = *actor->getPlrNo();
        return true;
    }
    return false;
}

bool dEn_c::checkWallAndBg() {
    float dir = l_EnMuki[mDirection];
    mBc.checkWall(&dir);
    mVec3_c truePos = mPos;
    truePos += mCenterOffs;
    return mBc.checkBg(truePos.x, truePos.y, mLayer, l_Ami_Line[mAmiLayer], 0x819);
}

int dEn_c::Enfumi_check(dCc_c *self, dCc_c *other, int step) {
    dActor_c *owner;
    FumiCcInfo_c fumiInfo(self, other);
    int fumiRes = mFumiProc.operate(fumiInfo, 1);
    owner = (dActor_c *) other->getOwner();
    switch (fumiRes) {
        case 1:
            if ((int) owner->mKind == STAGE_ACTOR_PLAYER) {
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
    dEnemyMng_c::m_instance->m_138 = 1;
}

void dEn_c::setFumiComboScore(dActor_c *actor) {
    int treadCount = dEnCombo_c::calcPlFumiCnt(actor);
    if (treadCount >= 0) {
        mVec3_c pos = getCenterPos();
        float scY = dScoreMng_c::smc_SCORE_Y;
        pos.y = mPos.y + scY;
        switch (mCombo.mType) {
            case dEnCombo_c::COMBO_REGULAR: {
                dScoreMng_c *instance = dScoreMng_c::getInstance();
                instance->ScoreSet(pos, treadCount, *actor->getPlrNo(), 1);
                break;
            }
            case dEnCombo_c::COMBO_SHORT: {
                dScoreMng_c *instance = dScoreMng_c::getInstance();
                instance->ScoreSet2(pos, treadCount, *actor->getPlrNo());
                break;
            }
            default:
                break;
        }
    }
}

bool dEn_c::checkComboClap(int num) {
    /// @unofficial
    static const int claps[] = { 7, 7, 4 };
    return num >= claps[mCombo.mType];
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
        count = ARRAY_SIZE(cs_combo_se) - 1;
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
        count = ARRAY_SIZE(cs_combo_se) - 1;
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
        count = ARRAY_SIZE(cs_combo_se) - 1;
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
    dAudio::SoundEffectID_t(SE_EMY_YOSHI_STEP).playEmySound(getCenterPos(), 0);
}

void dEn_c::fumiEffect(dActor_c *actor) {
    if (actor->mKind != STAGE_ACTOR_PLAYER && actor->mKind != STAGE_ACTOR_YOSHI) {
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

u32 dEn_c::EnBgCheckFoot() {
    if (!mBc.hasSensorFoot()) {
        mFootPush2.set(0.0f, 0.0f, 0.0f);
        return false;
    }
    u32 res = mBc.checkFootEnm();
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
        mFootPush = mBc.mPushForce;
        mFootPush2.set(0.0f, 0.0f, 0.0f);
    } else {
        mFootPush.set(0.0f, 0.0f, 0.0f);
    }
    return res;
}

u32 dEn_c::EnBgCheckWall() {
    if (!mBc.hasSensorWall()) {
        return 0;
    }
    float tmpX = mBc.m_4c;
    u8 dir = mPos.x - tmpX < 0.0f;
    float dirVal = l_EnMuki[dir];
    u32 flags = mBc.checkWallEnm(&dirVal);
    dirVal = l_EnMuki[dir ^ 1];
    u32 wallFlags = mBc.checkWallEnm(&dirVal);
    if (mBgCollFlags & 3) {
        wallFlags |= mBc.checkWallEnm(&dirVal);
    }
    mBc.mFlags |= flags | wallFlags;
    return flags;
}

void dEn_c::WaterCheck(mVec3_c &pos, float h) {
    if (mKilledByLiquid) {
        return;
    }
    int waterLineRes = WaterLineProc(pos, h);
    if (waterLineRes == dBc_c::WATER_CHECK_YOGAN) {
        setDeathInfo_Smoke(nullptr);
        mKilledByLiquid = true;
    } else if (waterLineRes != dBc_c::WATER_CHECK_NONE && mNoRespawn) {
        mKilledByLiquid = true;
    }
}

bool dEn_c::LineBoundaryCheck(dActor_c *actor) {
    daPlBase_c *pl = (daPlBase_c *) actor;
    if ((pl->mPos.z > 0.0f && mAmiLayer == 1) || (pl->mPos.z < 0.0f && mAmiLayer == 0)) {
        if (pl->mFlags & 0x80000 || pl->mFlags & 0x100000) {
            return true;
        }
    }
    return false;
}

dBc_c::WATER_TYPE_e dEn_c::WaterLineProc(const mVec3_c &pos, float h) {
    float height = 0.0f;
    dBc_c::WATER_TYPE_e res = dBc_c::checkWater(pos.x, pos.y, mLayer, &height);
    mVec3_c waterPos(mPos.x, height, 6500.0f);
    switch (res) {
        case dBc_c::WATER_CHECK_WATER:
        case dBc_c::WATER_CHECK_WATER_BUBBLE:
            if (!mInLiquid) {
                mInLiquid = true;
                mAirAccelY = mAccelY;
                mAirSpeedMaxY = mSpeedMax.y;
                mAirMaxFallSpeed = mMaxFallSpeed;
                setWaterSpeed();
                waterSplashEffect(waterPos, h);
            }
            break;
        case dBc_c::WATER_CHECK_YOGAN:
            if (!mInLiquid) {
                mInLiquid = true;
                yoganSplashEffect(waterPos, h);
            }
            break;
        case dBc_c::WATER_CHECK_POISON:
            if (!mInLiquid) {
                mInLiquid = true;
                poisonSplashEffect(waterPos, h);
            }
            break;
        default:
            if (mInLiquid) {
                mAccelY = mAirAccelY;
                mSpeedMax.y = mAirSpeedMaxY;
                mMaxFallSpeed = mAirMaxFallSpeed;
            }
            mInLiquid = false;
            break;
    }
    return res;
}

void dEn_c::yoganSplashEffect(const mVec3_c &pos, float scale) {
    mVec3_c efPos(pos, 6500.0f);
    mVec3_c efScale(scale, scale, scale);
    u32 flags = mLayer << 16 | 5;
    dEffActorMng_c::m_instance->createWaterSplashEff(efPos, flags, -1, efScale);

    dAudio::SoundEffectID_t(SE_OBJ_CMN_SPLASH_LAVA).playMapSound(efPos, 0);

    if (scale < 1.0f) {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 16);
    } else {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 14);
    }
}

void dEn_c::poisonSplashEffect(const mVec3_c &pos, float scale) {
    mVec3_c efPos(pos, 6500.0f);
    mVec3_c efScale(scale, scale, scale);
    u32 flags = mLayer << 16 | 7;
    dEffActorMng_c::m_instance->createWaterSplashEff(efPos, flags, -1, efScale);

    dAudio::SoundEffectID_t(SE_OBJ_CMN_SPLASH_POISON).playMapSound(pos, 0);

    if (scale < 1.0f) {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 23);
    } else {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 21);
    }
}

bool dEn_c::EnLavaCheck(const mVec3_c &pos) {
    float height = 0.0f;
    dBc_c::WATER_TYPE_e res = dBc_c::checkWater(pos.x, pos.y, mLayer, &height);
    if (res == dBc_c::WATER_CHECK_YOGAN) {
        if (height < 0.0f && mLastPos.y > height && mPos.y <= height) {
            yoganSplashEffect(mVec3_c(pos.x, height, 6000.0f), 1.0f);
        }
        return true;
    }
    return false;
}

bool dEn_c::EnWaterCheck(const mVec3_c &pos) {
    if (EnWaterFlagCheck(pos) != dBc_c::WATER_CHECK_NONE) {
        return true;
    }
    mSpeedMax.y = -4.0f;
    mMaxFallSpeed = -4.0f;
    return false;
}

bool dEn_c::EnWaterFlagCheck(const mVec3_c &pos) {
    switch (dBc_c::checkWater(pos.x, pos.y, mLayer, nullptr)) {
        case dBc_c::WATER_CHECK_WATER:
        case dBc_c::WATER_CHECK_WATER_BUBBLE:
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
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && daPyMng_c::checkPlayer(i)) {
            mVec3_c playerPos = player->mPos;
            float diff = std::fabs(playerPos.x - mPos.x);
            if (diff < x) {
                return true;
            }
        }
    }
    return false;
}

bool dEn_c::Area_XY_check(float x, float y) {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && daPyMng_c::checkPlayer(i)) {
            mVec3_c playerPos = player->mPos;
            if (std::fabs(playerPos.x - mPos.x) < x && std::fabs(playerPos.y - mPos.y) < y) {
                return true;
            }
        }
    }
    return false;
}

bool dEn_c::PlayerCarryCheck(dActor_c *actor) {
    for (int i = 0; i < PLAYER_COUNT; i++) {
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
    mMtx_c mtx = player->getCarryMtx();
    mMtx_c transposeMtx = mMtx_c::createTrans(pos);
    mVec3_c res;
    mtx.concat(transposeMtx).multVecZero(res);
    return res;
}

bool dEn_c::turnangle_calc(const short *target, const short *delta) {
    mAngle.y += delta[mDirection];
    if (target[0] <= mAngle.y || mAngle.y <= target[1]) {
        mAngle.y = target[mDirection];
        return true;
    }
    return false;
}

void dEn_c::slipBound(dActor_c *actor) {
    static const float cs_jump_xspeed[] = { 1.5f, -1.5f };
    daPlBase_c *pl = (daPlBase_c *) actor;

    u8 idx = !(pl->mPos.x >= mPos.x);
    pl->vf3fc(3.0f, cs_jump_xspeed[idx], 1, 0, 0);

    int plrNo = *pl->getPlrNo();
    mNoHitPlayer.mTimer[plrNo] = 3;
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
    mNoHitPlayer.mTimer[plrNo] = smc_NO_HIT_PLAYER_TIMER_SPIT_OUT;
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
    int killedBy = mDeathInfo.mKilledBy;
    if (mDeathInfo.mKilledBy >= 0 && mDeathInfo.mKilledBy < PLAYER_COUNT) {
        dMultiMng_c::mspInstance->incEnemyDown(killedBy);
    }
}

void dEn_c::fireballInvalid(dCc_c *self, dCc_c *other) {
    dAudio::g_pSndObjMap->startSound(SE_OBJ_FIREBALL_DISAPP, other->getOwner()->mPos, 0);
}

void dEn_c::iceballInvalid(dCc_c *self, dCc_c *other) {
    dBaseActor_c *owner = other->getOwner();
    dAudio::g_pSndObjMap->startSound(SE_OBJ_PNGN_ICEBALL_DISAPP, owner->mPos, 0);

    mVec3_c efPos(owner->mPos.x, owner->mPos.y, 5500.0f);
    EffectManager_c::SetIceBallMissshitEffect(&efPos);
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
    u8 dir = mDeathFallDirection;
    s8 plrNo = *getPlrNo();

    mVec3_c efPos(mVec2_c(mPos.x, mPos.y), 5500.0f);

    hitdamageEffect(efPos);
    dAudio::SoundEffectID_t(SE_EMY_DOWN).playEmySound(mPos, 0);

    mDeathInfo = (sDeathInfoData) {
        l_base_fall_speed_x[dir],
        3.0f,
        -4.0f,
        -0.1875f,
        &StateID_DieFall,
        -1,
        -1,
        dir,
        (u8) plrNo
    };
}
