#pragma once
#include <types.h>
#include <game/bases/d_cyuukan.hpp>
#include <game/mLib/m_vec.hpp>
#include <constants/game_constants.h>

class dInfo_c {
public:
    struct StartGameInfo_s {
        u32 mReplayDuration;
        u8 mMovieType;
        u8 mEntrance;
        u8 mArea;
        bool mIsReplay;
        u32 mScreenType;
        u8 mWorld1;
        u8 mLevel1;
        u8 mWorld2;
        u8 mLevel2;
    };

    /// @unofficial
    /// @todo Fill out this enum.
    enum GAME_FLAG_e {
        GAME_FLAG_0 = BIT_FLAG(0),
        GAME_FLAG_4 = BIT_FLAG(4),
        GAME_FLAG_IS_COIN_COURSE = BIT_FLAG(6)
    };

    struct enemy_s {
        int mSubworld;
        int mPathIndex;
        PATH_DIRECTION_e mWalkDirection;
        bool m_0c;
    };

    dInfo_c();

    void GetMapEnemyInfo(int, int, enemy_s &);
    void SetMapEnemyInfo(int, int, int, int);
    void FUN_800bbc40(int, int, int);

    u8 getCourse() const { return m_startGameInfo.mLevel1; }
    u8 getWorld() const { return m_startGameInfo.mWorld1; }
    dCyuukan_c *getCyuukan() { return &mCyuukan; }

    static dInfo_c *getInstance() { return m_instance; }

    char pad1[0x8];
    dCyuukan_c mCyuukan;
    char pad2[0x18];
    int m_54;
    u8 pad3[0x44];
    int m_9c;
    u8 pad4[0x2e4];
    int mCharIDs[4];
    bool mIsWorldSelect; ///< Whether the World Select Menu is being displayed.
    u8 pad5[30];
    bool mClearCyuukan; ///< Clear the checkpoint data if this is @p true. [Used for the backdoor entrance of 7-C]
    int mDisplayCourseWorld;
    int mDisplayCourseNum;
    u8 pad6[0x14];
    int mTextBoxMessageID;
    int mTextBoxMessageGroup;
    u8 pad7[0x1];
    bool mExtensionAttached;
    u8 m_3da;
    u8 pad8[0x7];
    int mCourseSelectPageNum;
    int mCourseSelectIndexInPage;
    u8 pad9[0x712];
    bool mFukidashiActionPerformed[4][0x16];
    u32 pad10;

    static dInfo_c *m_instance;
    static unsigned int mGameFlag; ///< See GAME_FLAG_e
    static StartGameInfo_s m_startGameInfo;
};
