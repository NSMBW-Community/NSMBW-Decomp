#pragma once
#include <game/bases/d_a_player.hpp>

class daPyMng_c {
public:
    enum PyType {}; ///< @unofficial
    enum PyScrollNum { ///< @unofficial
        SCROLL_0 = 0
    };

    static int getPlayerIndex(PyType); ///< @unofficial
    static dAcPy_c *getPlayer(int);
    static mVec3_c getPlayerSetPos(u8, u8);
    static void addScore(int, int);

    /// @unofficial
    static bool checkPlayer(u8 plrNo) { return mActPlayerInfo & (1 << plrNo); }
    static u8 getActScrollInfo(); /// @unofficial
    static PyScrollNum getScrollNum();

    /// @unofficial
    static bool isScrollMode1() { return getScrollNum() == 1; }

    static int mNum;
    static int mCtrlPlrNo;
    static u8 mActPlayerInfo;
    static int mPlayerType[4];
    static int mPlayerMode[4];
};
