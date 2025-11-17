#pragma once
#include <game/bases/d_a_player.hpp>
#include <game/bases/d_a_yoshi.hpp>
#include <constants/game_constants.h>

class daPyMng_c {
public:
    static void changeItemKinopioPlrNo(int &);
    static int getPlayerIndex(PLAYER_TYPE_e); ///< @unofficial
    static dAcPy_c *getPlayer(int);
    static dAcPy_c *getCtrlPlayer(int);
    static daYoshi_c *getYoshi(int);
    static void addScore(int, int);
    static void setHipAttackQuake(int, u8);
    static int getNumInGame();
    static int getItemKinopioNum();

    static bool checkPlayer(u8 plrNo) { return mActPlayerInfo & (1 << plrNo); }
    static int getRest(PLAYER_TYPE_e plrNo) { return mRest[plrNo]; }
    static int getPlayerType(int plrNo) { return mPlayerType[plrNo]; }
    static int getPlayerMode(int plrNo) { return mPlayerMode[plrNo]; }

    static int mNum;
    static u32 mPauseDisable;
    static u8 mActPlayerInfo;
    static u32 mPauseEnableInfo;
    static u32 mStopTimerInfo;
    static int mPlayerType[4];
    static int mPlayerMode[4];
    static int mRest[4];
    static u32 mCtrlPlrNo;
};
