#pragma once
#include <game/bases/d_audio.hpp>
#include <game/bases/d_scene.hpp>
#include <game/bases/d_quake.hpp>
#include <game/sLib/s_Phase.hpp>

class dScMovie_c : public dScene_c {
public:
    sPhase_c mChain;
    dQuake_c mQuake;
    void *mMovieData; // TODO
    void *mMovieSound; // TODO
    void *mDemoMessage; // TODO
    NMSndObjectBase mSndObject;
    u32 m_130[7];
    int mFrameCounter;
    int mIdxForCmd15;
    int mIdxForCmd2;
    int mIdxForCmd4;
    int mIdxForMovieSound;
    int mIdxForCmd6;
    u32 mState;
    u32 mCurrentSceneId;
    mVec3_c m_16C;
    mVec3_c m_178;
    mVec3_c mCameraPos;
    int m_190;
    int m_194;
    int mMovieId;
    bool mMovieEnded;
    u8 m_19D;
    u8 mPaddingMaybe[2];
    float m_1A0;

    static dScMovie_c *m_instance;
};
