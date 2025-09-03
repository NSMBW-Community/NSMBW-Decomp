#pragma once
#include <game/bases/d_audio.hpp>
#include <game/bases/d_scene.hpp>
#include <game/bases/d_quake.hpp>
#include <game/sLib/s_Phase.hpp>

class dScMovie_c : public dScene_c {
public:
    sPhase_c mChain;
    dQuake_c mQuake;
    void *mpMovieData; ///< @todo
    void *mpMovieSound; ///< @todo
    void *mpDemoMessage; ///< @todo
    NMSndObjectBase mSndObject;
    u32 m_130[7];
    int mFrameCounter;
    int mEndCommandIndex;
    int mCameraMoveCommandIndex;
    int mEffectCommandIndex;
    int mSoundCommandIndex;
    int mShakeCommandIndex;
    u32 mState;
    u32 mCurrentSceneId;
    mVec3_c m_16C;
    mVec3_c m_178;
    mVec3_c mCameraPos;
    int m_190;
    int m_194;
    int mMovieId;
    bool mMovieEnded;
    bool mEffectCreated;
    float m_1A0;

    static dScMovie_c *m_instance;
};
