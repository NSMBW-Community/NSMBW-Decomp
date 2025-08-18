#pragma once

#include <game/mLib/m_vec.hpp>

class dWmSeManager_c {
public:
    /// @unofficial
    enum EffectID_e {
        WM_SE_EMY_CS_ENEMY_BYE = 86,
        WM_SE_EMY_CS_MOVE_W1_KURIBO,
        WM_SE_EMY_CS_MOVE_W2_SANDHALL,
        WM_SE_EMY_CS_MOVE_W3_ICEBROS,
        WM_SE_EMY_CS_MOVE_W4_PUKU,
        WM_SE_EMY_CS_MOVE_W5_PAKKUN,
        WM_SE_EMY_CS_MOVE_W6_KILLER,
        WM_SE_EMY_CS_MOVE_W7_JUGEM,
        WM_SE_EMY_CS_MOVE_W8_BUBBLE_APP,
        WM_SE_EMY_CS_MOVE_W8_BUBBLE_DISAPP,
        WM_SE_INVALID = 121
    };

    /// @unofficial
    int playSound(int id, int, const mVec3_c &pos, int, int);
    int playSound(int id, const mVec3_c &pos, int);
    void endSound(int id);

    static dWmSeManager_c *m_pInstance;
};
