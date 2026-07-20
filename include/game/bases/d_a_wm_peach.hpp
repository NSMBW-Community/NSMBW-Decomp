#pragma once
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_chr.hpp>
#include <game/mLib/m_3d/mdl.hpp>
#include <game/bases/d_wm_demo_actor.hpp>

/// @brief The worldmap Peach actor, used during the World 8-Castle unlock cutscene.
/// @ingroup bases
class daWmPeach_c : public dWmDemoActor_c {
public:
    enum PROC_TYPE_e {
        PROC_TYPE_EXEC,
        PROC_COUNT
    };

    /// @brief The available animations for this actor.
    enum ANIM_e {
        fly_wmap_B1,
        fly_wmap_B2,
        ANIM_COUNT
    };

    typedef void (daWmPeach_c::*ProcFunc)();

    daWmPeach_c(); ///< @copydoc dWmDemoActor_c::dWmDemoActor_c
    ~daWmPeach_c(); ///< @copydoc dWmDemoActor_c::~dWmDemoActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame);

    void createModel(); ///< Initializes the resources for the actor.
    void calcModel(); ///< Updates the model's transformation matrix.

    /// @brief Sets an animation on the actor.
    /// @param animIdx The animation to play. Value is an ANIM_e.
    /// @param blendFrame The animation blending duration, in frames.
    /// @param rate The animation playback speed.
    /// @param frame The animation frame to start playing from.
    void setAnim(int animIdx, float blendFrame, float rate, float frame);

    void initState(); ///< Sets up the actor's initial state.
    void init_exec(); ///< Process initialization function for the @ref PROC_TYPE_EXEC "exec" process type.
    void mode_exec(); ///< Process function for the @ref PROC_TYPE_EXEC "exec" process type.

    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mModel; ///< The model.
    m3d::anmChr_c mChrAnim[ANIM_COUNT]; ///< The model animations.
    u32 mUnk250; ///< @unused
    PROC_TYPE_e mCurrProc; ///< The current process type. See PROC_TYPE_e.

    static const char *smc_resAnmNames[ANIM_COUNT]; ///< The animation names.
};
