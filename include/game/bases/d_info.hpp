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
    /**
     * If this is true, 3D models in the cutscene view
     * and world map view are not drawn. [Always set to false in the game].
     */
    bool mDisable3DScenes;
    int pad2[7];
    int field_3b4;

    static dInfo_c *m_instance;
    static StartGameInfo m_startGameInfo;
};
