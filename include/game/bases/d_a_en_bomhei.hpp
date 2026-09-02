#pragma once

#include <game/bases/d_a_en_carry.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d.hpp>
#include <game/mLib/m_effect.hpp>
#include <constants/sound_list.h>

/// @brief A bob-omb enemy.
/// @statetable
/// @paramtable
/// @ingroup bases
class daEnBomhei_c : public daEnCarry_c {
public:
    /// @brief The different modes for spawning a bob-omb.
    enum SpawnMode_e {
        SPAWN_MODE_NORMAL, ///< Start walking immediately.
        SPAWN_MODE_WAKIDASHI, ///< Walk out of a pipe.
        SPAWN_MODE_ICE_LUMP, ///< Stuck in a large ice block.
        SPAWN_MODE_CANNON_HOP ///< Shoot out of a cannon.
    };

    daEnBomhei_c() : mExplodeWaitTimer(-1), mExplodeTimer(-1), mFireCoinCount(smc_FireCountCount) {}
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
    virtual void Normal_VsEnHitCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsPlHitCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsYoshiHitCheck(dCc_c *self, dCc_c *other);
    virtual void block_hit_init();
    virtual bool hitCallback_Shell(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Fire(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Ice(dCc_c *self, dCc_c *other);
    virtual void returnState_Ice();
    virtual bool hitCallback_HipAttk(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_YoshiHipAttk(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Spin(dCc_c *self, dCc_c *other) { return hitCallback_HipAttk(self, other); }
    virtual bool createIceActor();
    virtual bool EtcDamageCheck(dCc_c *self, dCc_c *other);

    virtual void boyonBegin() {}
    virtual bool isSpinLiftUpEnable() { return isState(StateID_Sleep); }

    STATE_FUNC_DECLARE(daEnBomhei_c, Walk); ///< Walking on the ground.
    STATE_FUNC_DECLARE(daEnBomhei_c, Sleep); ///< Sleeping, waiting to explode.
    STATE_VIRTUAL_FUNC_DECLARE(daEnBomhei_c, Carry); ///< Being carried by a player.
    STATE_FUNC_DECLARE(daEnBomhei_c, Slide); ///< Sliding along the ground after a ground pound.
    STATE_FUNC_DECLARE(daEnBomhei_c, Kick); ///< Being kicked by the player.
    STATE_FUNC_DECLARE(daEnBomhei_c, KickBom);
    STATE_FUNC_DECLARE(daEnBomhei_c, Wakidashi); ///< Moving out of a pipe.
    STATE_FUNC_DECLARE(daEnBomhei_c, Explode); ///< Exploding after the timer runs out.
    STATE_FUNC_DECLARE(daEnBomhei_c, Turn); ///< Turning around
    STATE_FUNC_DECLARE(daEnBomhei_c, CannonHop_Upper); ///< Being shot upwards out of a cannon.
    STATE_FUNC_DECLARE(daEnBomhei_c, CannonHop_Under); ///< Being shot downwards out of a cannon.
    STATE_FUNC_DECLARE(daEnBomhei_c, AfterIce);
    STATE_FUNC_DECLARE(daEnBomhei_c, InIceLump); ///< Stuck in a large ice block.
    STATE_VIRTUAL_FUNC_DECLARE(daEnBomhei_c, EatOut);

    virtual void vf28c();
    virtual void modelUpdate();
    virtual void postMdlSetup();
    virtual void initialStateSet();
    virtual void vf29c() {}

    void mdlSetup();

    void bcSet1();
    void bcSet2();
    void setDeathInfo_Carry(dActor_c *killedBy);
    void checkBombBreak();
    void explodeBgUnit();
    mVec2_c getExplodeTilePos();
    void calcIgnitionPos();
    void updateCarryCc();

    void breakEffect() { mEf::createEffect("Wm_en_bombheibreak", 0, &mIgnitionPos, nullptr, nullptr); }
    void explosionEffect() { mEf::createEffect("Wm_en_explosion", 0, &mIgnitionPos, nullptr, nullptr); }

    dHeapAllocator_c mAllocator;
    nw4r::g3d::ResFile mFile;
    m3d::mdl_c mModel;
    m3d::anmChr_c mAnm;
    m3d::anmMatClr_c mAnmMat;
    int mExplodeWaitTimer; ///< Timer counting down until the bob-omb explodes.
    int mExplodeTimer; ///< Timer counting up while exploding.
    int mWakidashiTimer; ///< Timer counting down until the bob-omb will beging walking normally.
    int mCannonHopDir;
    int mCannonHopTimer;
    u8 mPad[4];
    mVec3_c mLiftPos;
    float mCcOffsetX;
    float mCcOffsetY;
    float mCcWidth;
    float mCcHeight;
    int mFireCoinCount; ///< How many more coins can be spawned by hitting the bob-omb with a fireball.
    int mCarryingPlayer;
    int mUnkTimer;
    mVec3_c mIgnitionPos;
    mEf::levelEffect_c mEffect;

    ACTOR_PARAM_CONFIG(WakidashiSpawnDir, 0, 2); ///< The direction to move out of the pipe.
    ACTOR_PARAM_CONFIG(CannonHopSpawnDir, 4, 2); ///< The direction to shoot out of the cannon.
    ACTOR_PARAM_CONFIG(CannonHopType, 6, 1); ///< The trajectory to shoot out of the cannon. 0 is a flat arc, 1 is a high arc.
    ACTOR_PARAM_CONFIG(ZLayer, 24, 4); ///< The Z layer to place the bob-omb on.
    ACTOR_PARAM_CONFIG(SpawnMode, 28, 2); ///< See SpawnMode_e.

    static const float smc_WalkSpeed;
    static const float smc_SlideSpeedX;
    static const float smc_KickSpeedX;
    static const float smc_KickSpeedY;
    static const int smc_ExplodeWaitFrames;
    static const int smc_SoundEffectIDs[];

    static const int smc_FireCountCount = 3;
};
