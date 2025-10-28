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

    static int calcPlFumiCnt(dActor_c *actor);

    COMBO_TYPE_e mType;
};
