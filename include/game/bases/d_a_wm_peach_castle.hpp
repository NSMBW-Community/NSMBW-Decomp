#pragma once
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_chr.hpp>
#include <game/mLib/m_3d/anm_chr_blend.hpp>
#include <game/mLib/m_3d/mdl.hpp>
#include <game/bases/d_wm_obj_actor.hpp>

/**
* @brief TODO
* @ingroup bases
* @paramtable
*/
class daWmPeachCastle_c : public dWmObjActor_c {
public:
    /// @brief The available animations for this actor.
    enum ANIM_e {
        cobPeachCastleFlag,
        cobPeachCastleOpen,
        cobPeachCastleClose,
        ANIM_COUNT
    };

    daWmPeachCastle_c(); ///< @copydoc dWmObjActor_c::dWmObjActor_c
    ~daWmPeachCastle_c(); ///< @copydoc dWmObjActor_c::~dWmObjActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame);

    void FUN_808e4730();
    void FUN_808e47b0();

    void createModel(); ///< Initializes the resources for the actor.
    void calcModel(); ///< Updates the model's transformation matrix.

    void FUN_808e4b50();
    void FUN_808e4bd0();
    void FUN_808e4c60();
    void FUN_808e4cf0();

    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mModel; ///< The model.
    m3d::anmChr_c mChrAnim[ANIM_COUNT]; ///< The model animations.
    m3d::anmChrBlend_c mChrBlendAnim;
    int _2B8;
};
