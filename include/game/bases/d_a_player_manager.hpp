#pragma once
#include <game/bases/d_a_player.hpp>
#include <game/bases/d_player_model_manager.hpp>
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
    static dPyMdlMng_c::ModelType_e getCourseInPlayerModelType(int);
    static void setPlayer(int, dAcPy_c *);
    static bool fn_8005f570(PLAYER_POWERUP_e, int); ///< @unofficial
    static void setCarryOverYoshiInfo(u8, u8, int);

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
    static PLAYER_POWERUP_e mPlayerMode[4];
    static PLAYER_POWERUP_e mKinopioMode;
    static u32 mCreateItem[4];
    static int mPlayerEntry[4];
    static int mRest[4];
    static u32 mCtrlPlrNo;
    static s16 m_star_time[4];
    static s16 m_star_count[4];
};
