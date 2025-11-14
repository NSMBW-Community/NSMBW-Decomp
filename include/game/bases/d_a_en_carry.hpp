#include <game/bases/d_enemy.hpp>

/// @brief A carryable enemy.
/// [As opposed to dEnemyCarry_c, this class is used a bit more widely.]
/// @statetable
/// @ingroup bases
class daEnCarry_c : public dEn_c {
public:
    STATE_VIRTUAL_FUNC_DECLARE(daEnCarry_c, Carry); ///< The enemy is being carried.

    int acmShellPlayerNo(dActor_c *actor) const;
    void shellDamageEffect(dCc_c *cc1, dActor_c *actor);

    static bool acmIsCarryEnemy(dActor_c *actor); ///< Checks whether the given actor is a carryable enemy.

    bool mForceShellDamageEffect; ///< Whether to force the shell damage effect the next time shellDamageEffect is called.
};
