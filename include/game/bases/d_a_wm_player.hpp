#pragma once

#include <game/bases/d_wm_demo_actor.hpp>

class daWmPlayer_c : public dWmDemoActor_c {
public:
    void setEnemyDieByStar(dWmEnemy_c *);

    static bool checkWalkPlayers();
    static bool isPlayerStarMode();

    u8 mPad1[0x8];
    bool m_18c;
    u8 mPad2[0x9f];
    int m_22c;
    int m_230;
    int m_234;
    u8 mPad4[0xc4];
    bool m_2fc;

    static daWmPlayer_c *ms_instance;
};
