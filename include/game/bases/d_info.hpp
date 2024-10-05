#pragma once
#include <types.h>

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

    char pad1[0x394];
    bool mIsWorldSelect; ///< Whether the World Select Menu is being displayed.
    int pad2[7];
    int field_3b4;
    char pad3[0x746]; // todo
    bool smth[4][0x16];

    static dInfo_c *m_instance;
    static StartGameInfo m_startGameInfo;
};
