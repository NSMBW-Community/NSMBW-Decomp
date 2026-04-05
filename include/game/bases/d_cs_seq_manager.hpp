#pragma once

#include <game/bases/d_wm_enemy.hpp>

class dCsSeqMng_c {
public:
    /// @unofficial
    enum CUTSCENE_e {
        SMC_DEMO_DEFAULT_CLR,
        SMC_DEMO_DEFAULT_FAIL,
        SMC_DEMO_ENEMY_CLR,
        SMC_DEMO_ENEMY_FAIL,
        SMC_DEMO_TORIDE_IN,
        SMC_DEMO_TORIDE_CLR,
        SMC_DEMO_TORIDE_FAIL,
        SMC_DEMO_TORIDE_FAIL2,
        SMC_DEMO_CASTLE_IN,
        SMC_DEMO_CASTLE_CLR,
        SMC_DEMO_CASTLE_FAIL,
        SMC_DEMO_CASTLE_FAIL2,
        SMC_DEMO_GHOST_IN,
        SMC_DEMO_GHOST_CLR,
        SMC_DEMO_GHOST_FAIL,
        SMC_DEMO_GHOST_FAIL2,
        SMC_DEMO_CANNON,
        SMC_DEMO_TRSHIP_APPEAR,
        SMC_DEMO_DOKAN,
        SMC_DEMO_DOKAN_WARP,
        SMC_DEMO_DOKAN_START,
        SMC_DEMO_W_WALKING_IN,
        SMC_DEMO_W_WALKING_IN_NORMAL,
        SMC_DEMO_W_FLYING_IN,
        SMC_DEMO_W_CANNON_IN,
        SMC_DEMO_W_CANNON_IN_NORMAL,
        SMC_DEMO_W1_TORIDE_CLR,
        SMC_DEMO_W1_CASTLE_CLR,
        SMC_DEMO_W3_CASTLE_CLR,
        SMC_DEMO_FADE_TEST,
        SMC_DEMO_VIEW_WORLD,
        SMC_DEMO_COURSE_IN,
        SMC_DEMO_KINOKO_OUT,
        SMC_DEMO_AIRSHIP_COURSE_IN,
        SMC_DEMO_AIRSHIP_COURSE_OUT,
        SMC_DEMO_START_KINOKO_IN,
        SMC_DEMO_AIRSHIP_GONEXT,
        SMC_DEMO_W36_CLEAR_NORMAL,
        SMC_DEMO_NULL,
        SMC_DEMO_ANTLION,
        SMC_DEMO_KILLER,
        SMC_DEMO_START_BATTLE,
        SMC_DEMO_SWITCH,
        SMC_DEMO_KOOPACASTLEAPPEAR,
        SMC_DEMO_KINOPIOSTART,
        SMC_DEMO_WORLDIN_NOSHIP,
        SMC_DEMO_WORLDIN_JUMP_NOSHIP,
        SMC_DEMO_PAUSE_MENU,
        SMC_DEMO_AIRSHIP_CLEAR,
        SMC_DEMO_STOCK_MENU,
        SMC_DEMO_WORLDSELECT_MENU,
        SMC_DEMO_ANTLION_STAR,
        SMC_DEMO_GAMESTART,
        SMC_DEMO_END,
        SMC_DEMO_COUNT
    };

    /// @unofficial
    enum CUTSCENE_CMD_e {
        CUTSCENE_CMD_NONE = -1,
        CUTSCENE_CMD_2 = 2,
        CUTSCENE_CMD_4 = 4,
        CUTSCENE_CMD_27 = 27,
        CUTSCENE_CMD_28 = 28,
        CUTSCENE_CMD_29 = 29,
        CUTSCENE_CMD_56 = 56,
        CUTSCENE_CMD_86 = 86,
        CUTSCENE_CMD_87 = 87,
        CUTSCENE_CMD_89 = 89,
        CUTSCENE_CMD_91 = 91,
        CUTSCENE_CMD_136 = 136,
    };

    int GetCutName();
    void FUN_801017c0(int, dWmEnemy_c *, int, int); ///< @unofficial

    u8 mPad1[0x164];
    bool m_164;
    u8 mPad2[0x47];
    dWmDemoActor_c *m_1ac;
    dWmDemoActor_c *m_1b0;

    static dCsSeqMng_c *ms_instance;
};
