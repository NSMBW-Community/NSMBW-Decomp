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
    static dPyMdlMng_c::ModelType_e getCourseInPlayerModelType(u8);
    static void setPlayer(int, dAcPy_c *);
    static bool fn_8005f570(PLAYER_POWERUP_e, int); ///< @unofficial
    static void setCarryOverYoshiInfo(u8 plrNo, u8 yoshiColor, int fruitCount);
    static bool addNum(int);
    static bool decNum(int);
    static bool addRest(int);
    static bool decRest(int);
    static u32 getEntryNum();
    static bool isEntryNum1() { return getEntryNum() == 1; }
    static void startYoshiBGM();
    static void stopYoshiBGM();
    static void startMissBGM(int plrNo);
    static void startStarBGM();
    static void stopStarBGM();
    static bool isCreateBalloon(int plrNo);
    static int getYoshiColor(u8 plrNo);
    static int getYoshiFruit(u8 plrNo);
    static daYoshi_c *createYoshi(mVec3_c &, int, dAcPy_c *);

    static bool checkPlayer(u8 plrNo) { return mActPlayerInfo & (1 << plrNo); }
    static int getRest(PLAYER_TYPE_e plrNo) { return mRest[plrNo]; }
    static PLAYER_TYPE_e getPlayerType(int plrNo) { return mPlayerType[plrNo]; }
    static int getPlayerMode(int plrNo) { return mPlayerMode[plrNo]; }
    static nw4r::math::VEC3 getPlayerSetPos(u8 file, u8 gotoNo);

    static int mNum;
    static u32 mPauseDisable;
    static u8 mActPlayerInfo;
    static int mPauseEnableInfo;
    static u32 mStopTimerInfo;
    static PLAYER_TYPE_e mPlayerType[4];
    static PLAYER_POWERUP_e mPlayerMode[4];
    static PLAYER_POWERUP_e mKinopioMode;
    static int mKinopioCarryCount;
    static u32 mCreateItem[4];
    static int mPlayerEntry[4];
    static int mRest[4];
    static u32 mCtrlPlrNo;
    static s16 m_star_time[4];
    static s16 m_star_count[4];
    static int mAllBalloon;
    static int mTimeUpPlayerNum;
};
