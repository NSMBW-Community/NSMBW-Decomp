#include <game/bases/d_enemy.hpp>

class daEnCarry_c : public dEn_c {
public:
    STATE_VIRTUAL_FUNC_DECLARE(daEnCarry_c, Carry);

    static bool acmIsCarryEnemy(dActor_c *actor);
    int acmShellPlayerNo(dActor_c *actor) const;
    void shellDamageEffect(dCc_c *cc1, dActor_c *actor);

    bool mIsCarryEnemy;
};
