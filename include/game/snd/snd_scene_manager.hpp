#pragma once
#include <types.h>

class SndSceneMgr {
public:
    enum ESnd_GameState {
        GAME_STATE_0,
        GAME_STATE_1
    };

    enum ESnd_GameFx {};

    enum ESnd_Scene {
        SCENE_0,
        SCENE_1
    };

    void moveMissFin();
    void setFxMode(ESnd_GameFx fx);
    void onGameStateBit(ESnd_GameState state);
    void offGameStateBit(ESnd_GameState state);

    u8 mPad1[0x10];
    int m_10;
    int m_14;

    static SndSceneMgr *sInstance;
};
