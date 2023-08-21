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
    
    char filler[0x3b4];
    int field_3b4;

    static dInfo_c *m_instance;
    static StartGameInfo m_startGameInfo;
};
