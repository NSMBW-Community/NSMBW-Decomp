#pragma once

#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_chr.hpp>
#include <game/mLib/m_3d/smdl.hpp>
#include <game/bases/d_wm_obj_actor.hpp>
#include <game/bases/d_wm_bgm_sync.hpp>

/**
* @brief The actor for the decorative clouds used in the World 7 map.
* @details The actor represents an animated cloud model composed of multiple bones.
*
* Each bone listed in #sGroupNodeNames is treated as an independent visual group, and is
* culled individually every frame. This allows the model to be partially rendered, improving performance.
* 
* The bone indices are resolved during initialization via #initGroupNodeIds. Then on every frame the actor:
* - Computes each bone’s world position.
* - Builds a bounding sphere, using the radius from GlobalData_t::mGroupNodeRadii.
* - Tests the sphere against the camera frustum.
* - Toggles visibility of the bone depending on the test result.
*
* Animation playback is synchronized with background music using #mpBgmSync, which adjusts animation rate dynamically.
* @ingroup bases
*/
class daWmCloud_c : public dWmObjActor_c {
public:
    static const int NODE_COUNT = 20;

    /// @brief The global configuration for the actor.
    struct GlobalData_t {
        float mUnk; ///< @unused
        float mGroupNodeRadii[NODE_COUNT]; ///< The culling radius for each bone.
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
    void initGroupNodeIds(); ///< Initializes the @ref mGroupNodeIds "bone index values" from the @ref sGroupNodeNames "model bone names".

    void calcModel(); ///< Updates the model's transformation matrix.
    void calcCulling(); ///< Performs per-node culling based on the camera frustum.

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

    /// @brief The bone indices, resolved from #sGroupNodeNames.
    /// @details A value of @p -1 indicates an unused or missing node.
    int mGroupNodeIds[NODE_COUNT];
    mSphere_c mCurrNodeClipSphere; ///< The sphere currently in use during per-node culling checks.
    dWmBgmSync_c *mpBgmSync; ///< The background music synchronization helper.

    /// @brief The bone names used for the culling mechanism.
    /// @details Entries may be null, indicating unused slots.
    /// @hideinitializer
    static const char *sGroupNodeNames[NODE_COUNT];
};
