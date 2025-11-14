#pragma once

#include <game/bases/d_a_en_carry.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_effect.hpp>
#include <constants/sound_list.h>

class daEnBomhei_c : public daEnCarry_c {
public:
    enum SpawnMode_e {
        SPAWN_MODE_NORMAL,
        SPAWN_MODE_WAKIDASHI,
        SPAWN_MODE_ICE_LUMP,
        SPAWN_MODE_CANNON,
    };

    daEnBomhei_c() : mTimer(-1), m_5f0(-1), m_620(3) {}
    virtual ~daEnBomhei_c() {}

    virtual int create();
    virtual int execute();
    virtual int preDraw();
    virtual int draw();
    virtual int doDelete();
    virtual void deleteReady();
    virtual void finalUpdate();
    virtual bool ActorDrawCullCheck();
    virtual void setSpinLiftUpActor(dActor_c *carryingActor);
    virtual void Normal_VsEnHitCheck(dCc_c *cc1, dCc_c *cc2);
    virtual void Normal_VsPlHitCheck(dCc_c *cc1, dCc_c *cc2);
    virtual void Normal_VsYoshiHitCheck(dCc_c *cc1, dCc_c *cc2);
    virtual void block_hit_init();
    virtual bool hitCallback_Shell(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Fire(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Ice(dCc_c *cc1, dCc_c *cc2);
    virtual void returnState_Ice();
    virtual bool hitCallback_HipAttk(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_YoshiHipAttk(dCc_c *cc1, dCc_c *cc2);
    virtual bool hitCallback_Spin(dCc_c *cc1, dCc_c *cc2) { return hitCallback_HipAttk(cc1, cc2); }
    virtual bool createIceActor();
    virtual bool EtcDamageCheck(dCc_c *cc1, dCc_c *cc2);

    virtual void boyonBegin() {}
    virtual bool isSpinLiftUpEnable() { return isState(StateID_Sleep); }

    STATE_FUNC_DECLARE(daEnBomhei_c, Walk);
    STATE_FUNC_DECLARE(daEnBomhei_c, Sleep);
    STATE_VIRTUAL_FUNC_DECLARE(daEnBomhei_c, Carry);
    STATE_FUNC_DECLARE(daEnBomhei_c, Slide);
    STATE_FUNC_DECLARE(daEnBomhei_c, Kick);
    STATE_FUNC_DECLARE(daEnBomhei_c, KickBom);
    STATE_FUNC_DECLARE(daEnBomhei_c, Wakidashi);
    STATE_FUNC_DECLARE(daEnBomhei_c, Explode);
    STATE_FUNC_DECLARE(daEnBomhei_c, Turn);
    STATE_FUNC_DECLARE(daEnBomhei_c, CannonHop_Upper);
    STATE_FUNC_DECLARE(daEnBomhei_c, CannonHop_Under);
    STATE_FUNC_DECLARE(daEnBomhei_c, AfterIce);
    STATE_FUNC_DECLARE(daEnBomhei_c, InIceLump);
    STATE_VIRTUAL_FUNC_DECLARE(daEnBomhei_c, EatOut);

    virtual void vf28c();
    virtual void vf290();
    virtual void vf294();
    virtual void vf298();
    virtual void vf29c() {}

    void mdlSetup();

    void bcSet1();
    void bcSet2();
    void setDeathInfo_Carry(dActor_c *killedBy);
    void checkBombBreak();
    void someBgCheck();
    mVec2_c getSomePos();
    void calcIgnitionPos();
    void updateCarryCc();

    void breakEffect() { mEf::createEffect("Wm_en_bombheibreak", 0, &mIgnitionPos, nullptr, nullptr); }
    void explosionEffect() { mEf::createEffect("Wm_en_explosion", 0, &mIgnitionPos, nullptr, nullptr); }

    ACTOR_PARAM_CONFIG(wakidashiConfig, 0, 2);
    ACTOR_PARAM_CONFIG(unk2, 4, 2);
    ACTOR_PARAM_CONFIG(CannonHopDir, 6, 1);
    ACTOR_PARAM_CONFIG(zLayer, 24, 4);
    ACTOR_PARAM_CONFIG(spawnMode, 28, 2);

    dHeapAllocator_c mAllocator;
    nw4r::g3d::ResFile mFile;
    m3d::mdl_c mModel;
    m3d::anmChr_c mAnm;
    m3d::anmMatClr_c mAnmMat;
    int mTimer;
    int m_5f0;
    int m_5f4;
    int m_5f8;
    int m_5fc;
    u8 mPad[4];
    mVec3_c m_604;
    // mVec2_c mCcOffset;
    float mCcOffsetX;
    float mCcOffsetY;
    float mCcWidth;
    float mCcHeight;
    int m_620;
    int m_624;
    int m_628;
    mVec3_c mIgnitionPos;
    mEf::levelEffect_c mEffect;

    static const float smc_WALK_SPEED;
    static const float smc_SLIDE_SPEED_X;
    static const float smc_KICK_SPEED_X;
    static const float smc_KICK_SPEED_Y;
    static const int smc_5EC_VALUE;
    static const int smc_SOUND_EFFECTS[];
};
