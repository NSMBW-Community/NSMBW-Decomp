#pragma once
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_chr.hpp>
#include <game/mLib/m_3d/anm_tex_srt.hpp>
#include <game/mLib/m_3d/mdl.hpp>
#include <game/bases/d_wm_obj_actor.hpp>

/**
* @brief The actor for World Map pipes used in course unlock paths.
* @details All instances of this actor are hardcoded and selected via the #Node parameter,
* obtained via the bone name. Each index corresponds to a specific, predefined location on the World Map.
* @ingroup bases
* @paramtable
*/
class daWmDokanRoute_c : public dWmObjActor_c {
public:
    static const int NODE_COUNT = 6;

    /// @brief The global configuration for the actor.
    struct GlobalData_t {
        float mScaleMultiplier; ///< The scale multiplier to be applied to all pipes.
        struct {
            mVec3_c mPosOffset; ///< The offset from the pipe's bone position.
            float mAnmRate; ///< The animation speed multiplier.
            s16 mInitialDelay; ///< The initial delay before the pipe begins moving, in frames.
            s16 mAnimDuration; ///< The total duration of the animation, in frames. This also controls the pipe's length.
            s16 mDecelDuration; ///< The duration of the animation's deceleration phase, in frames.
            s16 mDirection; ///< The direction of the pipe. Value is a DIR_e.
        } mNodeConfigs[NODE_COUNT]; ///< The configuration for each hardcoded pipe instance.
    };

    /// @brief The available animations for this actor.
    enum ANIM_e {
        cobDokanRoute,
        ANIM_COUNT
    };

    ///< The pipe's facing direction.
    enum DIR_e {
        EAST,
        WEST,
        NORTH, ///< @unused
        SOUTH
    };

    /// @brief The possible level exit types associated with the pipe.
    enum EXIT_TYPE_e {
        EXIT_TYPE_SECRET, ///< The pipe is part of a secret exit unlock animation.
        EXIT_TYPE_NORMAL, ///< The pipe is part of a regular exit unlock animation.
    };

    /// @brief The possible animation states.
    enum STATE_e {
        STATE_IDLE, ///< No animation plays.
        STATE_WAIT_DELAY, ///< Initial delay before the pipe begins moving.
        STATE_ANIM_START, ///< The sound effect is played and the pipe begins moving.
        STATE_ANIM_PLAY, ///< The movement animation is playing.
        STATE_ANIM_END ///< The movement animation is completed.
    };

    typedef void (daWmDokanRoute_c::*ProcFunc)();

    daWmDokanRoute_c(); ///< @copydoc dWmObjActor_c::dWmObjActor_c
    ~daWmDokanRoute_c(); ///< @copydoc dWmObjActor_c::~dWmObjActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame);

    void createModel(); ///< Initializes the resources for the actor.
    void initState(); ///< Sets up the actor's initial state.
    void initPosRot(); ///< Initializes the pipe position and rotation (and the associated texture animation).
    void initAnim(); ///< Initializes the pipe's model animation, based on the associated level's completion status.

    void calcModel(m3d::mdl_c &model); ///< Updates the model's transformation matrix.
    void calcAnim(); ///< Updates the currently playing animation.

    void init_exec(); ///< Process initialization function for the @ref dWmObjActor_c::PROC_TYPE_EXEC "exec" process type.
    void mode_exec(); ///< Process function for the @ref dWmObjActor_c::PROC_TYPE_EXEC "exec" process type.

    void onCourseUnlockInit(); ///< Initializes the course unlock cutscene animation.
    void onCourseUnlock(); ///< Updates the course unlock cutscene animation.

    /// @brief Computes the animation frame for a fully completed pipe animation.
    /// @details This is used to initialize the pipe in its "already activated" state when
    /// the associated course has been previously cleared.
    /// @return The animation frame corresponding to the end of the animation.
    float getEndFrame();

    u8 GetNodeNum() const { return ACTOR_PARAM(Node); }

    u32 mUnk188; ///< @unused
    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mModel; ///< The model.
    m3d::anmChr_c mChrAnim[ANIM_COUNT]; ///< The model animation.
    m3d::anmTexSrt_c mSrtAnim[ANIM_COUNT]; ///< The texture animation.
    u32 mUnk250; ///< @unused
    PROC_TYPE_e mCurrProc; ///< The current process type. See dWmObjActor_c::PROC_TYPE_e.
    int mStateTimer; ///< The timer for the current state.
    STATE_e mState; ///< The current cutscene state. See STATE_e.
    mVec3_c mInitialPos; ///< The initial position of the pipe.

    static const u8 sExitTypes[NODE_COUNT]; ///< The exit type associated with each node. Values are EXIT_TYPE_e. @hideinitializer
    static const char *sPointNames[NODE_COUNT]; ///< The point name associated with each node. @hideinitializer

    ACTOR_PARAM_CONFIG(Node, 0, 8); ///< The index into the @ref GlobalData_t::mNodeConfigs "hardcoded node configuration table".
};
