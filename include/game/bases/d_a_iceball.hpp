#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_circle_light_mask.hpp>
#include <game/bases/d_heap_allocator.hpp>

/// @brief An iceball, thrown by the player.
/// @paramtable
/// @statetable
/// @ingroup bases
class daIceBall_c : public dActorState_c {
public:
    struct GlobalData_t {
        float calcExtraBoost(float speedDiff) const {
            return speedDiff * mBoostExtraScale;
        }

        /// Up to this speed, the iceball's speed boost is calculated by multiplying
        /// the player's speed by mMoveBoostScale.
        float moveBoostThreshold;

        /// Any speed between 0 and moveBoostThreshold is multiplied by this scale
        /// to determine the iceball's speed boost on bounce.
        float mMoveBoostScale;

        /// If the player's speed on iceball bounce is over the boost threshold,
        /// the extra speed is multiplied by this scale and added to the base speed boost.
        float mBoostExtraScale;

        float mLightRadius; ///< The radius of the iceball's light mask.
    };

    daIceBall_c() {} ///< Creates a new iceball actor.
    virtual ~daIceBall_c() {} ///< Destroys the iceball actor.

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();
    virtual void deleteReady();
    virtual void setEatTongue(dActor_c *eatingActor);
    virtual void eatMove(dActor_c *eatingActor);

    STATE_FUNC_DECLARE(daIceBall_c, EatIn); ///< The iceball is being eaten.
    STATE_FUNC_DECLARE(daIceBall_c, EatNow); ///< The iceball has been eaten.
    STATE_FUNC_DECLARE(daIceBall_c, FireMove); ///< The iceball is moving downwards after being thrown.
    STATE_FUNC_DECLARE(daIceBall_c, Kill); ///< The iceball has hit something and should be deleted.
    STATE_FUNC_DECLARE(daIceBall_c, Move); ///< The iceball hit the ground and is doing a final bounce.

    /// @brief Checks if the iceball is close to the ground so that it can spawn a bit higher up.
    /// @param groundHeight Output parameter for the ground height.
    /// @return Whether the iceball is close to the ground.
    bool checkInitLine(float &groundHeight);

    /// @brief Checks whether the iceball should vanish on creation.
    /// This happens if the iceball is supposed to spawn inside a wall.
    /// @return Whether the iceball should vanish.
    bool checkInitVanish();

    bool bgCheck(); ///< Checks for collisions and returns whether a collision occurred.
    bool checkDeleteBg(); ///< Checks if the iceball should be deleted due to hitting something.
    void setDeleteEffect(); ///< Creates an effect for when the iceball gets destroyed.
    bool cullCheck(); ///< Checks if the iceball is within the camera's view.

    void lightProc(); ///< Updates the iceball's light mask.
    void chgZpos(); ///< Updates the iceball's Z position.

    /// @brief Checks if the iceball hit the surface of a liquid.
    /// If it did, it creates a splash effect and sound.
    /// @return Whether the iceball should be deleted (in case of collision with lava or poison).
    bool waterlineCheck();

    void waterSplash(float height); ///< Creates a water splash effect and sound at a specified height.
    void yoganSplash(float height); ///< Creates a lava splash effect and sound at a specified height.
    void poisonSplash(float height); ///< Creates a poison splash effect and sound at a specified height.

    static void ccCallback_Iceball(dCc_c *self, dCc_c *other); ///< Collision callback for the iceball.

    /// @brief Checks if a new iceball can be created for a player based on the limit mode.
    /// @param playerNo The player number.
    /// @param limitMode The limit mode for iceball creation.
    /// 0: 2 alive iceballs allowed, 1: 6 alive iceballs allowed. @see mAliveTimer
    /// @return Whether a new iceball can be created.
    static bool CheckIceballLimit(int playerNo, int limitMode);

    /// @brief Timer for the iceball's lifetime.
    /// This is used to limit how quickly the player can throw multiple iceballs.
    /// Once the cooldown reaches zero, the iceball is no longer considered "alive",
    /// even though it still exists.
    u32 mAliveTimer;
    u32 mHitEntity; ///< Whether the iceball has hit an entity.
    u8 mPad1[4];
    dBc_c::WATER_TYPE_e mLiquidType; ///< The type of liquid the iceball is currently in contact with.
    float mLiquidHeight; ///< The height of the liquid surface the iceball is in contact with.
    mVec3_c mStartPos; ///< The starting position of the iceball when it was created.
    dHeapAllocator_c mAllocator; ///< Heap allocator for the iceball.
    dCircleLightMask_c mLightMask; ///< Light mask for the iceball.
    u8 mPad2[4];

    ACTOR_PARAM_CONFIG(PlayerNo, 0, 2); ///< The player who threw the iceball.
    ACTOR_PARAM_CONFIG(Direction, 4, 1); ///< The direction the iceball was thrown.
    ACTOR_PARAM_CONFIG(Layer, 8, 2); ///< The layer the iceball is on.
    ACTOR_PARAM_CONFIG(AmiLine, 12, 2); ///< The chainlink fence layer for the iceball.
    ACTOR_PARAM_CONFIG(LimitMode, 16, 2); ///< The limit mode for the iceball. @see CheckIceballLimit()

    static int sm_IceBallCount[4]; ///< The number of iceballs that currently exist for each player.
    static int sm_IceBallAliveCount[4]; ///< The number of "alive" iceballs for each player. @see #mAliveTimer

    static const int smc_MAX_ICEBALL_COUNT = 6; ///< @unofficial
    static const int smc_MAX_ALIVE_ICEBALL_COUNT = 2; ///< @unofficial
};
