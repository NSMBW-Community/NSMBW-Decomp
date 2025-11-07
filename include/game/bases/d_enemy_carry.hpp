#pragma once

#include <game/bases/d_enemy.hpp>

class dEnemyCarry_c : public dEn_c {
public:
    dEnemyCarry_c();

    virtual void setThrowSpeed(dActor_c *);
    virtual void setThrowChangeState();
    virtual void setPutOnChangeState();

    STATE_VIRTUAL_FUNC_DECLARE(dEnemyCarry_c, Carry);
    STATE_VIRTUAL_FUNC_DECLARE(dEnemyCarry_c, Throw);

    void setDeathInfo_CarryBgIn(dActor_c *);

protected:
    bool mIsCarry;
    bool mIsCarried;

    static const float smc_DIR_MODE1[2]; ///< @unofficial
    static const float smc_DIR_MODE2[2]; ///< @unofficial
};
