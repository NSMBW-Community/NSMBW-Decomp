#include <game/bases/d_a_en_carry.hpp>

STATE_VIRTUAL_DEFINE(daEnCarry_c, Carry);

bool daEnCarry_c::acmIsCarryEnemy(dActor_c *actor) {
    if (actor->mKind == STAGE_ACTOR_ENEMY) {
        dEn_c *enemy = (dEn_c *) actor;
        if (enemy->mFlags & dEn_c::FLAG_0) {
            return true;
        }
    }
    return actor->mProfName == fProfile::EN_MECHA_KOOPA ||
           actor->mProfName == fProfile::EN_BOMHEI;
}

int daEnCarry_c::acmShellPlayerNo(dActor_c *actor) const {
    if (isState(StateID_Carry)) {
        return mPlayerNo;
    }
    return *actor->getPlrNo();
}

void daEnCarry_c::shellDamageEffect(dCc_c *cc1, dActor_c *actor) {
    bool isHit = true;
    if (!mIsCarryEnemy && isState(StateID_Carry)) {
        isHit = false;
    }
    if (isHit) {
        mVec2_c pos2D = cc1->mCollPos;
        hitdamageEffect(mVec3_c(pos2D, 5500.0f));
    } else {
        if (acmIsCarryEnemy(actor)) {
            ((daEnCarry_c *) actor)->mIsCarryEnemy = true;
        }
    }
}

void daEnCarry_c::initializeState_Carry() {}
void daEnCarry_c::finalizeState_Carry() {}
void daEnCarry_c::executeState_Carry() {}
