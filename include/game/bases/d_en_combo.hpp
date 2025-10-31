#pragma once

#include <game/bases/d_actor.hpp>

class dEnCombo_c {
public:
    /// @unofficial
    enum COMBO_TYPE_e {
        COMBO_0,
        COMBO_1,
        COMBO_2
    };

    dEnCombo_c(COMBO_TYPE_e type) : mType(type) {}

    int getComboScore(int);
    int getQuakeScore(int) const;
    int getDamageScore() const;

    static int calcPlStarCnt(dActor_c *actor);
    static int calcPlComboCnt(dActor_c *actor);
    static int calcPlFumiCnt(dActor_c *actor);

    COMBO_TYPE_e mType;
};
