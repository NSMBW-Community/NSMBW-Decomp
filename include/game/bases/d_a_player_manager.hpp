#pragma once
#include <game/bases/d_a_player.hpp>
#include <constants/game_constants.h>

class daPyMng_c {
public:
    enum PyScrollNum { ///< @unofficial
        SCROLL_0 = 0
    };

    static int getPlayerIndex(PLAYER_CHARACTER_e); ///< @unofficial
    static dAcPy_c *getPlayer(int);
    static mVec3_c getPlayerSetPos(u8, u8);
    static void addScore(int, int);

    static bool checkPlayer(u8 plrNo) { return mActPlayerInfo & (1 << plrNo); }
    static u8 getActScrollInfo(); /// @unofficial
    static PyScrollNum getScrollNum();

    static int getRest(PLAYER_CHARACTER_e plrNo) { return mRest[plrNo]; }
    static bool isScrollMode1() { return getScrollNum() == 1; }

    static int mNum;
    static u8 mActPlayerInfo;

    static int mPlayerType[4];
    static int mPlayerMode[4];
    static int mRest[4];
    static u32 mCtrlPlrNo;
};
