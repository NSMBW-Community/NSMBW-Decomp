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
    struct AnimEl_t {
        mVec3_c mPosDelta;
        float mAnmRate;
        s16 mTimer;
        s16 mTimer2;
        s16 mTimerThreshold;
        s16 mDirection;
    };

    struct GlobalData_t {
        float mInitialScale;
        AnimEl_t mAnims[6];
    };

    enum PROC_STATE_e {
        PROC_STATE_DEFAULT,
        PROC_STATE_COUNT
    };

    enum STATE_e {
        STATE_0,
        STATE_1,
        STATE_2,
        STATE_3,
        STATE_4
    };

    typedef void (daWmDokanRoute_c::*ProcFunc)();

    daWmDokanRoute_c(); ///< @copydoc dWmObjActor_c::dWmObjActor_c
    ~daWmDokanRoute_c(); ///< @copydoc dWmObjActor_c::~dWmObjActor_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    virtual void setCutEndSpecific(int cutsceneCommandId, bool param2);

    void createModel(); ///< Initializes the resources for the actor.
    void calcModel(m3d::mdl_c &model); ///< Updates the model's transformation matrix.

    void FUN_808d01a0();
    void FUN_808d0270();
    void FUN_808d02c0();
    void FUN_808d02d0();
    void FUN_808d0360();
    void FUN_808d0520();
    void FUN_808d0660();
    void FUN_808d0740();
    float GetFrame();

    u32 mUnk188; ///< @unused
    dHeapAllocator_c mAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mModel; ///< The model.
    m3d::anmChr_c mChrAnim[1]; ///< The model animation.
    m3d::anmTexSrt_c mSrtAnim[1]; ///< The texture animation.
    u32 mUnk250; ///< @unused
    PROC_STATE_e mProcState; ///< @unused
    int mTimer; ///< The timer for the current state.
    STATE_e mState; ///< The current state.
    mVec3_c mPosCopy;

    static const u8 sIsNormalExit[6];
    static const char *sNodeNames[6];

    ACTOR_PARAM_CONFIG(Node, 0, 8);
};
