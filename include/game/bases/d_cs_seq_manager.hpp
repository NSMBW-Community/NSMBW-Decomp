#pragma once

#include <game/bases/d_wm_enemy.hpp>

class dCsSeqMng_c {
public:
    int GetCutName();
    void FUN_801017c0(int, dWmEnemy_c *, int, int); ///< @unofficial

    u8 mPad1[0x164];
    bool m_164;
    u8 mPad2[0x47];
    dWmDemoActor_c *m_1ac;
    dWmDemoActor_c *m_1b0;

    static dCsSeqMng_c *ms_instance;
};
