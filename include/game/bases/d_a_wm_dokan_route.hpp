#pragma once
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_chr.hpp>
#include <game/mLib/m_3d/anm_tex_srt.hpp>
#include <game/mLib/m_3d/mdl.hpp>
#include <game/bases/d_wm_obj_actor.hpp>

/// @brief The actor for moving pipes found on the World Map.
/// @ingroup bases
/// @paramtable
class daWmDokanRoute_c : public dWmObjActor_c {
public:
    struct GlobalData_t {
        float mInitialScale;
        struct {
            mVec3_c mPosDelta; ///< The delta from the pipe's bone position.
            float mAnmRate; ///< The animation speed multiplier.
            s16 mTimer;
            s16 mTimer2;
            s16 mTimerThreshold;
            s16 mDirection; ///< The direction of the pipe.
        } mAnims[6];
    };

    enum ANIM_e {
        cobDokanRoute,
        ANIM_COUNT
    };

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
    void calcModel(m3d::mdl_c &model); ///< Updates the model's transformation matrix.
    void calcAnim(); ///< Updates the currently playing animation.

    void init_exec(); ///< Process initialization function for the @ref dWmObjActor_c::PROC_TYPE_EXEC "exec" process type.
    void mode_exec(); ///< Process function for the @ref dWmObjActor_c::PROC_TYPE_EXEC "exec" process type.

    void onCourseUnlockInit();
    void onCourseUnlock();

    void FUN_808d0270();
    void FUN_808d0360();
    void FUN_808d0520();
    float getFrame();

    u8 GetNodeNum() const { return ACTOR_PARAM(Node); }

    u32 mUnk188; ///< @unused
    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mModel; ///< The model.
    m3d::anmChr_c mChrAnim[ANIM_COUNT]; ///< The model animation.
    m3d::anmTexSrt_c mSrtAnim[ANIM_COUNT]; ///< The texture animation.
    u32 mUnk250; ///< @unused
    PROC_TYPE_e mCurrProc; ///< The current process type.
    int mStateTimer; ///< The timer for the current state.
    STATE_e mState; ///< The current cutscene state.
    mVec3_c mPosCopy;

    static const u8 sIsNormalExit[6];
    static const char *sPointNames[6];

    ACTOR_PARAM_CONFIG(Node, 0, 8);
};
