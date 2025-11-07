#pragma once

#include <game/bases/d_enemy.hpp>

/// @brief A carryable enemy.
/// This class is only used as a base for daEnJumpdai_c.
/// @statetable
/// @ingroup bases
class dEnemyCarry_c : public dEn_c {
public:
    dEnemyCarry_c(); ///< Constructs a new carryable enemy.

    virtual void setThrowSpeed(dActor_c *actor); ///< Sets the throw speed based on the actor throwing it.
    virtual void setThrowChangeState(); ///< Throws this enemy.
    virtual void setPutOnChangeState(); ///< Drops this enemy.

    STATE_VIRTUAL_FUNC_DECLARE(dEnemyCarry_c, Carry); ///< The enemy is being carried.
    STATE_VIRTUAL_FUNC_DECLARE(dEnemyCarry_c, Throw); ///< The enemy is being thrown / put down.

    void setDeathInfo_CarryBgIn(dActor_c *); ///< Sets the death info for when the enemy is stuck in background collision.

protected:
    bool mWasDropped; ///< Whether the enemy was dropped or thrown. If dropped, it will not collide with other entities.
    u8 mThrowSpeed; ///< 0 => X speed = 2.5, 1 => X speed = 3.0

    static const float smc_DIR_MODE0[]; ///< @unofficial
    static const float smc_DIR_MODE1[]; ///< @unofficial
};
