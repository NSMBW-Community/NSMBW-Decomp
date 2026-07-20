#pragma once
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_chr.hpp>
#include <game/mLib/m_3d/anm_chr_blend.hpp>
#include <game/mLib/m_3d/mdl.hpp>
#include <game/bases/d_wm_obj_actor.hpp>

/**
* @brief The actor for Peach's Castle found on the World Map.
* @ingroup bases
* @paramtable
*/
class daWmPeachCastle_c : public dWmObjActor_c {
public:
    /// @brief The available animations for this actor.
    enum ANIM_e {
        cobPeachCastleFlag, ///< The flag animation.
        cobPeachCastleOpen, ///< The door open animation.
        cobPeachCastleClose, ///< The door close animation.
        ANIM_COUNT
    };

    daWmPeachCastle_c(); ///< @copydoc dWmObjActor_c::dWmObjActor_c
    ~daWmPeachCastle_c(); ///< @copydoc dWmObjActor_c::~dWmObjActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame);

    void initState(); ///< Sets up the actor's initial state.
    void initUnk(); ///< Initializes #mUnk2B8.

    void createModel(); ///< Initializes the resources for the actor.
    void calcModel(); ///< Updates the model's transformation matrix.

    void initFlagAnim(); ///< Initializes the flag animation.
    void initDoorOpenAnim(); ///< Initializes the door open animation.
    void initDoorCloseAnim(); ///< Initializes the door close animation.
    void ensureCourseClear(); ///< Ensures the course associated to the actor is cleared.

    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mModel; ///< The model.
    m3d::anmChr_c mChrAnim[ANIM_COUNT]; ///< The model animations.
    m3d::anmChrBlend_c mChrBlendAnim; ///< The blended animation.
    int mUnk2B8; ///< @unused

    static const char *smc_doorNodeNames[]; ///< The node names for the door open/close animations. @hideinitializer
};
