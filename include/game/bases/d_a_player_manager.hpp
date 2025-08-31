#pragma once
#include <game/bases/d_a_player.hpp>
#include <constants/game_constants.h>

class daPyMng_c {
public:

    static int getPlayerIndex(PLAYER_CHARACTER_e); ///< @unofficial
    static dAcPy_c *getPlayer(int);
    static void addScore(int, int);

    static bool checkPlayer(u8 plrNo) { return mActPlayerInfo & (1 << plrNo); }
    static int getRest(PLAYER_CHARACTER_e plrNo) { return mRest[plrNo]; }
    static int getPlayerType(int plrNo) { return mPlayerType[plrNo]; }
    static int getPlayerMode(int plrNo) { return mPlayerMode[plrNo]; }

    static int mNum;
    static u8 mActPlayerInfo;

    static int mPlayerType[4];
    static int mPlayerMode[4];
    static int mRest[4];
    static u32 mCtrlPlrNo;
};
