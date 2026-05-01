#pragma once

#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_chr.hpp>
#include <game/mLib/m_3d/smdl.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_wm_obj_actor.hpp>
#include <game/bases/d_wm_bgm_sync.hpp>

class daWmCloud_c : public dWmObjActor_c {
public:
    static const int NODE_COUNT = 20;

    /// @brief The global configuration for the actor.
    struct GlobalData_t {
        float mUnk;
        float mGroupNodeRadii[NODE_COUNT];
        s16 mBgmValue[2];
    };

    /// @brief The available animations for this actor.
    enum ANIM_e {
        CS_W7_Cloud,
        ANIM_COUNT
    };

    typedef void (daWmCloud_c::*ProcFunc)();

    daWmCloud_c(); ///< @copydoc dWmObjActor_c::dWmObjActor_c
    ~daWmCloud_c(); ///< @copydoc dWmObjActor_c::~dWmObjActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame);

    void createModel(); ///< Initializes the resources for the actor.
    void initState(); ///< Sets up the actor's initial state.
    void initNodeGroupIds();

    void calcModel(); ///< Updates the model's transformation matrix.
    void calcCulling();

    void init_exec(); ///< Process initialization function for the @ref dWmObjActor_c::PROC_TYPE_EXEC "exec" process type.
    void mode_exec(); ///< Process function for the @ref dWmObjActor_c::PROC_TYPE_EXEC "exec" process type.

    static void hideNode(nw4r::g3d::ResNode node) {
        if (node.IsValid()) {
            node.SetVisibility(false);
        }
    }

    static void showNode(nw4r::g3d::ResNode node) {
        if (node.IsValid()) {
            node.SetVisibility(true);
        }
    }

    u32 mUnk188; ///< @unused
    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::smdl_c mModel; ///< The model.
    m3d::anmChr_c mChrAnim[ANIM_COUNT]; ///< The model animations.
    u32 mUnk250; ///< @unused
    PROC_TYPE_e mCurrProc; ///< The current process type. See dWmObjActor_c::PROC_TYPE_e.
    int mGroupNodeIds[NODE_COUNT];
    mSphere_c mCurrNodeClipSphere;
    dWmBgmSync_c *mpBgmSync;

    static const char *sGroupNodeNames[NODE_COUNT];
};
