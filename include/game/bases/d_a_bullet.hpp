#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_heap_allocator.hpp>

class daBullet_c : public dActorState_c {
public:
    /// @unofficial
    enum HitType_e {
        HIT_NONE,
        HIT_REFLECT,
        HIT_STAR,
        HIT_SHELL,
        HIT_YOSHI_BULLET
    };

    daBullet_c();
    virtual ~daBullet_c() {}

    virtual int create();
    virtual int preExecute();
    virtual int execute();
    virtual int draw();
    virtual void deleteReady();
    virtual int doDelete();

    virtual void setEatTongue(dActor_c *eatingActor);
    virtual bool setEatSpitOut(dActor_c *eatingActor);
    virtual void waterSplashEffect(const mVec3_c &pos, float size);

    STATE_VIRTUAL_FUNC_DECLARE(daBullet_c, EatIn);
    STATE_VIRTUAL_FUNC_DECLARE(daBullet_c, EatNow);
    STATE_VIRTUAL_FUNC_DECLARE(daBullet_c, SpiteMove);
    STATE_VIRTUAL_FUNC_DECLARE(daBullet_c, Reflect);
    STATE_VIRTUAL_FUNC_DECLARE(daBullet_c, HitReflect);
    STATE_VIRTUAL_FUNC_DECLARE(daBullet_c, HitStar);
    STATE_VIRTUAL_FUNC_DECLARE(daBullet_c, HitShell);
    STATE_VIRTUAL_FUNC_DECLARE(daBullet_c, HitYoshiBullet);

    virtual void createMdl();
    virtual void removeMdl();
    virtual void initialize();

    virtual void setSpitOutMove(dActor_c *eatingActor);
    virtual void setDeadMove(const mVec3_c &speed, short angle);
    virtual void cullingProc();

    virtual bool hitProc_Star(dCc_c *other);
    virtual bool hitProc_Shell(dCc_c *other);
    virtual bool hitProc_YoshiBullet(dCc_c *other);
    virtual bool hitProc_Reflect(dCc_c *other);

    virtual void revengeHitSE();
    virtual void spitRoll();
    virtual void moveSE();
    virtual void deadRoll();

    void allocate();

    bool checkPlayerDamage(dCc_c *self, dCc_c *other);
    bool checkYoshiDamage(dCc_c *self, dCc_c *other);

    bool splashProc();
    void setDamage_Player(dActor_c *actor);

    static void collisionCallback(dCc_c *self, dCc_c *other);
    static void revengeCallback(dCc_c *self, dCc_c *other);

private:
    dHeapAllocator_c mAllocator;
    BOOL mHasSplashed;
    int mDeadMoveDirection;
    int mHitType;
    mAng3_c mDeadRollDelta;

    static const float smc_DEAD_FALL_GRAVITY;
    static const float smc_DEAD_FALL_YMAXSPEED;
    static const float smc_DIR_PRM[];
};