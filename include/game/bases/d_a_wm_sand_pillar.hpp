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
    static const int NODE_COUNT = 3;

    /// @brief The global configuration for the actor.
    struct GlobalData_t {
        float mUnk4F4[NODE_COUNT];
        float mUnk500[NODE_COUNT];
        float mUnk4F8[NODE_COUNT];
        float mPosY[NODE_COUNT];
        float mScaleY[NODE_COUNT];
        float mUnk3C[NODE_COUNT];
        float mUnk48[NODE_COUNT];
        float mUnk54[NODE_COUNT];
        int mTimer2[NODE_COUNT];
        int mTimer1[NODE_COUNT];
        u32 mUnk78[NODE_COUNT];
    };

    daWmSandPillar_c(); ///< @copydoc dWmObjActor_c::dWmObjActor_c
    ~daWmSandPillar_c(); ///< @copydoc dWmObjActor_c::~dWmObjActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void finalUpdate();

    virtual void processCutsceneCommand(int cutsceneCommandId, bool isFirstFrame);

    void createModel(); ///< Initializes the resources for the actor.
    void calcModel(); ///< Updates the model's transformation matrix.
    void createEffect();

    void FUN_808e55e0();

    u32 mUnk188; ///< @unused
    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mModel; ///< The model.
    m3d::anmChr_c mChrAnim; ///< The model animations.
    nw4r::g3d::ResAnmTexSrt mResAnmTexSrt; ///< The texture animation resource.
    m3d::anmTexSrt_c mSrtAnim; ///< The texture animation.

    mVec3_c mUnk254;
    dEf::dLevelEffect_c mEffect1;
    dEf::dLevelEffect_c mEffect2;

    sFStateMgr_c<daWmSandPillar_c, sStateMethodUsr_FI_c> mStateMgr; ///< The state manager.

    u32 mUnk4EC;
    int mTimer1;
    float mUnk4F4;
    float mUnk4F8;
    int mTimer2;
    float mUnk500;
    u32 mUnk504;

    STATE_FUNC_DECLARE(daWmSandPillar_c, Ready);
    STATE_FUNC_DECLARE(daWmSandPillar_c, BottomWait);
    STATE_FUNC_DECLARE(daWmSandPillar_c, MoveReady);
    STATE_FUNC_DECLARE(daWmSandPillar_c, MoveUp);
    STATE_FUNC_DECLARE(daWmSandPillar_c, TopWait);
    STATE_FUNC_DECLARE(daWmSandPillar_c, MoveDown);
    STATE_FUNC_DECLARE(daWmSandPillar_c, BottomWaitForever);
    STATE_FUNC_DECLARE(daWmSandPillar_c, TopWaitForever);
    STATE_FUNC_DECLARE(daWmSandPillar_c, TopWaitFromTheStart);

    ACTOR_PARAM_CONFIG(Node, 0, 8); ///< The index into the @ref GlobalData_t "hardcoded node configuration".
};
