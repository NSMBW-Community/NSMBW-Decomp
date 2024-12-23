#pragma once
#include <game/bases/d_a_player.hpp>

class daPyMng_c {
public:
    enum PyType {}; ///< @unofficial
    static int getPlayerIndex(PyType); ///< @unofficial
    static dAcPy_c *getPlayer(int);
    static void addScore(int, int);

    static bool checkPlayer(u8 plrNo) { return mActPlayerInfo & (1 << plrNo); }

    static int mNum;
    static u8 mActPlayerInfo;

    static int mPlayerType[4];
    static int mPlayerMode[4];
    static int mRest[4];
    static u32 mCtrlPlrNo;
};
