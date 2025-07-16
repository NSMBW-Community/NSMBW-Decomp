#pragma once

#include <game/bases/d_wm_demo_actor.hpp>

class dCsSeqMng_c {
public:
    int GetCutName();

    u8 mPad[0x1ac];
    dWmDemoActor_c *m_1ac;
    dWmDemoActor_c *m_1b0;

    static dCsSeqMng_c *ms_instance;
};
