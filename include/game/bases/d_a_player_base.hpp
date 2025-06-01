#pragma once
#include <game/bases/d_actor.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_ac_py_key.hpp>
#include <game/bases/d_cc.hpp>
#include <game/bases/d_effect.hpp>
#include <game/sLib/s_State.hpp>

class daPlBase_c : public dActor_c {
public:
    daPlBase_c();

    void changeState(const sStateIDIf_c &newState) { mStateMgr.changeState(newState); }
    void changeDemoState(const sStateIDIf_c &, int);

    STATE_FUNC_DECLARE(daPlBase_c, None);
    STATE_FUNC_DECLARE(daPlBase_c, DemoNone);

    u8 mPad1[0x100];
    dEf::followEffect_c mFollowEf;
    mEf::levelEffect_c mLevelEf1;
    u8 mPad2[0x20];
    mEf::levelEffect_c mLevelEfs2;
    mEf::levelEffect_c mLevelEfs3;
    mEf::levelEffect_c mLevelEfs4;
    mEf::levelEffect_c mLevelEfs5;
    mEf::levelEffect_c mLevelEfs6;
    mEf::levelEffect_c mLevelEfs7;
    dAudio::SndObjctPly_c sndObj;
    dAcPyKey_c mKey;
    dCc_c mCc1, mAttCc1, mAttCc2, mAttCc3;
    sFStateMgr_c<daPlBase_c, sStateMethodUsr_FI_c> mStateMgr;
    sFStateMgr_c<daPlBase_c, sStateMethodUsr_FI_c> mDemoStateMgr;
    char mPad[0xaa4];
};
