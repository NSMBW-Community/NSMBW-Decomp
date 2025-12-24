#pragma once
#include <game/bases/d_heap_allocator.hpp>
#include <game/bases/d_actor.hpp>

/**
 * @brief An early version of the unused water bubble, without collision.
 * @details The bubble moves in the specified direction with a fixed acceleration, and is deleted automatically after #LIFE_TIME frames.
 * @paramtable
 * @ingroup bases
 * @unused
 */
class dAwa_c : public dActor_c {
public:
    typedef void (dAwa_c::*CalcSpeed)(); ///< The speed update function.

    dAwa_c() : mCalcSpeedFn(), mLifeTimer(LIFE_TIME) {} ///< @copydoc dActor_c::dActor_c

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();
    virtual void deleteReady();
    virtual ~dAwa_c() {} ///< @copydoc dActor_c::~dActor_c

    void createModel(); ///< Initializes the resources for the actor.
    void setAnim(); ///< Initializes the model animation.
    void calc(); ///< Updates the bubble's position and speed.

    void calcSpeedUp(); ///< Updates the bubble's speed (upwards direction).
    void calcSpeedDown(); ///< Updates the bubble's speed (downwards direction).
    void calcSpeedLeft(); ///< Updates the bubble's speed (leftwards direction).
    void calcSpeedRight(); ///< Updates the bubble's speed (rightwards direction).

    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mBubbleModel; ///< The bubble model.
    m3d::anmChr_c mBubbleAnim; ///< The bubble animation.
    CalcSpeed mCalcSpeedFn; ///< The speed update function.
    int mLifeTimer; ///< The amount of frames left before the bubble is deleted.

    static const int LIFE_TIME = 60; ///< The life time of the bubble.
    ACTOR_PARAM_CONFIG(Direction, 0, 2); ///< The direction the bubble moves in.
};
