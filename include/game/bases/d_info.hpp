#pragma once
#include <types.h>
#include <game/mLib/m_vec.hpp>

class dCyuukan_c {
public:
    virtual ~dCyuukan_c();

    bool isCyuukanStart(int, u8, u8);

    int mActivatingPlayer;
    mVec3_c mPlayerSetPos;
    u32 mIsAmbush;
    u8 mWorldNo;
    u8 mCourseNo;
    u8 mAreaNo;
    u8 mEntranceNo;
    u32 mIsKinopioInChukan;
    int mCoinCollection[3];
    int mPlayerType[2];
};

class dInfo_c {
public:
    struct StartGameInfo {
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

    dCyuukan_c *getCyuukan() { return &mCyuukan; }

    void markExtension(int ext) {
        if (ext == 0) {
            mExtensionAttached = 0;
        } else {
            mExtensionAttached = 1;
        }
    }

    char pad1[0x8];
    dCyuukan_c mCyuukan;
    char pad2[0x348];
    int mCharIDs[4];
    bool mIsWorldSelect; ///< Whether the World Select Menu is being displayed.
    int pad3[7];
    int field_3b4;
    int field_3b8;
    u8 pad4[0x1d];
    u8 mExtensionAttached;
    u8 pad5[0x724];
    bool mFukidashiActionPerformed[4][0x16];

    static dInfo_c *m_instance;
    static unsigned int mGameFlag;
    static StartGameInfo m_startGameInfo;
};
