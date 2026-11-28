#pragma once
#include <game/bases/d_effect.hpp>
#include <game/sLib/s_State.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_chr.hpp>
#include <game/mLib/m_3d/anm_tex_srt.hpp>
#include <game/mLib/m_3d/mdl.hpp>
#include <game/bases/d_wm_obj_actor.hpp>

/// @brief The sand pillar world map actor.
/// @ingroup bases
/// @paramtable
/// @statetable
class daWmSandPillar_c : public dWmObjActor_c {
public:
    enum NodeType_e {
        NODE_0,
        NODE_1,
        NODE_2,
        NODE_COUNT
    };

    /// @brief The global configuration for the actor.
    struct GlobalData_t {
        float mStepInitialScale[NODE_COUNT]; ///< The initial scale step when moving.
        float mStepAccel[NODE_COUNT]; ///< The rate at which the scale step approaches mStepMaxScale.
        float mStepMaxScale[NODE_COUNT]; ///< The maximum scale step.
        float mPosY[NODE_COUNT]; ///< The base Y coordinate for the actor.
        float mBottomHeight[NODE_COUNT]; ///< The height of the pillar when resting at the bottom.
        float mTopHeight[NODE_COUNT]; ///< The height of the pillar when extended at the top.
        float mPulseBottomHeight[NODE_COUNT]; ///< The height of the pillar when resting during a pulse.
        float mPulseTopHeight[NODE_COUNT]; ///< The height of the pillar when extended during a pulse.
        int mPulseCount[NODE_COUNT]; ///< The number of pulses before rising all the way to mTopScaleY.
        int mBottomWaitTime[NODE_COUNT]; ///< The wait time at the bottom, in frames.
        int mTopWaitTime[NODE_COUNT]; ///< The wait time at the top, in frames.
    };

    daWmSandPillar_c(); ///< @copydoc dWmObjActor_c::dWmObjActor_c
    ~daWmSandPillar_c(); ///< @copydoc dWmObjActor_c::~dWmObjActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();

    virtual void finalUpdate();

    virtual void processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame);

    void createModel(); ///< Initializes the resources for the actor.
    void createEffect(); ///< Creates the sand effects.

    void calcModel(); ///< Updates the model's transformation matrix.
    void calcScaleStep(); ///< Updates the movement scale step.

    int getNodeIdx() { return ACTOR_PARAM(Node); }

    u32 mUnk188; ///< @unused
    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mModel; ///< The model.
    m3d::anmChr_c mChrAnim; ///< The model animation.
    nw4r::g3d::ResAnmTexSrt mResAnmTexSrt; ///< The texture animation resource.
    m3d::anmTexSrt_c mSrtAnim; ///< The texture animation.

    mVec3_c mInitialPos; ///< The actor's initial position.
    dEf::dLevelEffect_c mEffect1;
    dEf::dLevelEffect_c mEffect2;

    sFStateMgr_c<daWmSandPillar_c, sStateMethodUsr_FI_c> mStateMgr; ///< The state manager.

    BOOL mIsPulsing; ///< Whether the pillar is doing a regular pulse.
    int mWaitTimer; ///< The countdown timer for the Wait states.
    float mScaleStep; ///< The current scale step.
    float mTargetScaleStep; ///< The target scale step.
    int mRemainingPulses; ///< The remaining pulse cycles before the pillar moves all the way to the top.
    float mStepAccel; ///< The rate at which the scale step approaches mTargetScaleStep.
    BOOL mHasCourseCond; ///< Whether the actor is linked to a specific course.

    STATE_FUNC_DECLARE(daWmSandPillar_c, Ready); ///< Initial state.
    STATE_FUNC_DECLARE(daWmSandPillar_c, BottomWait); ///< Waiting at the bottom before pulsing.
    STATE_FUNC_DECLARE(daWmSandPillar_c, MoveReady); ///< Pulsing up and down before rising to full height.
    STATE_FUNC_DECLARE(daWmSandPillar_c, MoveUp); ///< Rising to the top scale.
    STATE_FUNC_DECLARE(daWmSandPillar_c, TopWait); ///< Waiting at the top before retracting.
    STATE_FUNC_DECLARE(daWmSandPillar_c, MoveDown); ///< Retracting back to the bottom scale.
    STATE_FUNC_DECLARE(daWmSandPillar_c, BottomWaitForever); ///< Waiting indefinitely at the bottom.
    STATE_FUNC_DECLARE(daWmSandPillar_c, TopWaitForever); ///< Waiting indefinitely at the top.
    STATE_FUNC_DECLARE(daWmSandPillar_c, TopWaitFromTheStart); ///< Initializing directly at the top before waiting indefinitely.

    ACTOR_PARAM_CONFIG(Node, 0, 8); ///< The index into the @ref GlobalData_t "hardcoded node configuration".
};
