#pragma once
#include <game/bases/d_actor.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_ac_py_key.hpp>
#include <game/bases/d_cc.hpp>
#include <game/bases/d_effect.hpp>
#include <game/sLib/s_State.hpp>

class dPyMdlMng_c;

class daPlBase_c : public dActor_c {
public:
    enum DamageType_e {
        DAMAGE_NONE = 0
    };

    enum STATUS_e {
        STATUS_01 = 0x01,
        STATUS_03 = 0x03,
        STATUS_1A = 0x1a,
        STATUS_1B = 0x1b,
        STATUS_2D = 0x2d,
        STATUS_2E = 0x2e,
        STATUS_40 = 0x40,
        STATUS_41 = 0x41,
        STATUS_42 = 0x42,
        STATUS_59 = 0x59,
        STATUS_5B = 0x5b,
        STATUS_5C = 0x5c,
        STATUS_61 = 0x61,
        STATUS_62 = 0x62,
        STATUS_6F = 0x6f,
        STATUS_7D = 0x7d,
        STATUS_82 = 0x82,
        STATUS_83 = 0x83,
        STATUS_84 = 0x84,
        STATUS_89 = 0x89,
        STATUS_8B = 0x8b,
        STATUS_8C = 0x8c,
        STATUS_8D = 0x8d,
        STATUS_97 = 0x97,
        STATUS_98 = 0x98,
        STATUS_A4 = 0xa4,
        STATUS_B5 = 0xb5,
        STATUS_B7 = 0xb7,
        STATUS_B9 = 0xb9,
        STATUS_BB = 0xbb,
        STATUS_BE = 0xbe,
    };

    daPlBase_c();
    virtual ~daPlBase_c();

    virtual int create();
    virtual int preExecute();
    virtual int execute();
    virtual void postExecute(fBase_c::MAIN_STATE_e status);
    virtual int doDelete();
    virtual int preDraw();
    virtual int draw();

    virtual void executeMain() {}

    virtual void setZPosition();

    virtual void changeState(const sStateIDIf_c &, void *);
    void executeState();
    void changeDemoState(const sStateIDIf_c &, int);

    void onStatus(int);
    void offStatus(int);
    bool isStatus(int);
    void releaseCcData();
    void clearFollowMameKuribo();
    void clearCcPlayerRev();
    void clearCcData();
    void clearComboCount();
    void updateEndingDance();

    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, None);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Walk);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Jump);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, SitJump);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Fall);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Land);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Crouch);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Slip);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Turn);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, HipAttack);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Swim);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, JumpDai);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, PlayerJumpDai);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Funsui);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Kani);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, Cloud);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, AnimePlay);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, WaitJump);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoNone);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoStartWait);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoWait);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanU);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanD);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanR);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInDokanL);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanU);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanD);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanR);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanL);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutDokanRoll);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoInWaterTank);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoOutWaterTank);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoRailDokan);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoDown);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoNextGotoBlock);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoGoal);
    STATE_VIRTUAL_FUNC_DECLARE(daPlBase_c, DemoControl);

    u8 mPad1[0x20];
    u32 m_20;
    u8 mPad2[0x34];
    float m_58;
    u8 mPad3[0xa4];
    dEf::followEffect_c mFollowEf;
    mEf::levelEffect_c mLevelEf1;
    u8 mPad4[0x1c];
    u32 m_344;
    mEf::levelEffect_c mLevelEfs2;
    mEf::levelEffect_c mLevelEfs3;
    mEf::levelEffect_c mLevelEfs4;
    mEf::levelEffect_c mLevelEfs5;
    mEf::levelEffect_c mLevelEfs6;
    mEf::levelEffect_c mLevelEfs7;
    dPyMdlMng_c *mpMdlMng;
    dAudio::SndObjctPly_c mSndObj;
    dAcPyKey_c mKey;
    char mPad5[0x2d];
    bool m_ca1;
    mVec3_c m_ca4;
    mVec3_c m_cb0;
    char mPad7[0x88];
    u32 m_d44;
    char mPad8[0x88];
    dCc_c mCc1, mAttCc1, mAttCc2, mAttCc3;
    char mPad9[0x24];
    sFStateMgr_c<daPlBase_c, sStateMethodUsr_FI_c> mDemoStateMgr;
    char mPad10[0x10];
    sFStateMgr_c<daPlBase_c, sStateMethodUsr_FI_c> mStateMgr;
    void *m_110c;
    u32 m_1110, m_1114, m_1118;
};
