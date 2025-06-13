#pragma once
#include <game/bases/d_a_player.hpp>
#include <game/bases/d_a_yoshi.hpp>

class daPyMng_c {
public:
    static dAcPy_c *getPlayer(int);
    static dAcPy_c *getCtrlPlayer(int);
    static daYoshi_c *getYoshi(int);
    static void addScore(int, int);
    static void setHipAttackQuake(int, u8);
    static int getNumInGame();
    static int getItemKinopioNum();

    /// @unofficial
    static bool checkPlayer(u8 plrNo) { return mActPlayerInfo & (1 << plrNo); }

    static int mNum;
    static u32 mPauseDisable;
    static u8 mActPlayerInfo;
    static u32 mPauseEnableInfo;
    static u32 mStopTimerInfo;
    static int mPlayerType[4];
    static int mPlayerMode[4];
};
