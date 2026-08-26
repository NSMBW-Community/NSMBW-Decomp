#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_heap_allocator.hpp>

/// @brief The base class for various projectile actors.
/// @statetable
/// @ingroup bases
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

    daBullet_c(); ///< @copydoc dActor_c::dActor_c
    virtual ~daBullet_c() {} ///< @copydoc dActor_c::~dActor_c

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

    virtual void createMdl(); ///< Creates the model for the bullet.
    virtual void removeMdl(); ///< Removes the model for the bullet.
    virtual void initialize(); ///< Subclass-specific initialization logic for the bullet.

    virtual void setSpitOutMove(dActor_c *eatingActor); ///< Handle the bullet being spat out by Yoshi.
    virtual void setDeadMove(const mVec3_c &speed, short angle); ///< Sets the movement parameters for the bullet after it has been hit.
    virtual void cullingProc(); ///< Culls the bullet if needed.

    virtual bool hitProc_Star(dCc_c *other); ///< Handles the bullet being hit by a player or Yoshi with a star.
    virtual bool hitProc_Shell(dCc_c *other); ///< Handles the bullet being hit by a shell.
    virtual bool hitProc_YoshiBullet(dCc_c *other);
    virtual bool hitProc_Reflect(dCc_c *other);

    virtual void revengeHitSE();
    virtual void spitRoll();
    virtual void moveSE();
    virtual void deadRoll();

    void allocate(); ///< Creates the allocator and load the model.

    bool checkPlayerDamage(dCc_c *self, dCc_c *other); ///< Returns whether a player with a star is touching the bullet.
    bool checkYoshiDamage(dCc_c *self, dCc_c *other); ///< Returns whether a Yoshi with a star is touching the bullet.

    bool splashProc(); ///< Checks if the water has been hit by the bullet and creates a splash effect if so.
    void setDamage_Player(dActor_c *actor); ///< Triggers damage to a player that touched the bullet.

    static void collisionCallback(dCc_c *self, dCc_c *other); ///< Regular collision callback for the bullet.
    static void revengeCallback(dCc_c *self, dCc_c *other); ///< Collision callback for when the bullet was spat back out by Yoshi.

private:
    dHeapAllocator_c mAllocator; ///< The allocator.
    BOOL mHasSplashed; ///< Whether the bullet has splashed in water yet.
    int mHitMoveDirection; ///< The direction to travel after the bullet has been hit.
    HitType_e mHitType; ///< The type of hit that the bullet has received.
    mAng3_c mHitRollDelta; ///< The rotation delta to apply to the bullet each frame after it has been hit.

    static const float smc_DEAD_FALL_GRAVITY; ///< The gravity to apply to a falling bullet.
    static const float smc_DEAD_FALL_YMAXSPEED; ///< The maximum falling speed of a bullet.
    static const float smc_DIR_PRM[]; ///< The unit direction values for left and right movement of the bullet.
};
