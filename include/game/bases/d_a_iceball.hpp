#pragma once

#include <game/bases/d_actor_state.hpp>
#include <game/bases/d_circle_light_mask.hpp>
#include <game/bases/d_heap_allocator.hpp>

class daIceBall_c : public dActorState_c {
public:
    daIceBall_c() {}
    virtual ~daIceBall_c() {}

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();
    virtual void deleteReady();
    virtual void setEatTongue(dActor_c *);
    virtual void eatMove(dActor_c *);

    STATE_FUNC_DECLARE(daIceBall_c, EatIn);
    STATE_FUNC_DECLARE(daIceBall_c, EatNow);
    STATE_FUNC_DECLARE(daIceBall_c, FireMove);
    STATE_FUNC_DECLARE(daIceBall_c, Kill);
    STATE_FUNC_DECLARE(daIceBall_c, Move);

    bool checkInitLine(float &);
    bool checkInitVanish();
    void lightProc();
    void setDeleteEffect();
    bool bgCheck();
    void chgZpos();
    bool cullCheck();
    bool checkDeleteBg();
    bool waterlineCheck();
    void waterSplash(float);
    void yoganSplash(float);
    void poisonSplash(float);

    static void ccCallback_Iceball(dCc_c *, dCc_c *);
    static bool CheckIceballLimit(int, int);

    u32 m_3d0;
    u32 m_3d4;
    u8 m_unk_3d8[4];
    dBc_c::WATER_TYPE_e mLiquidType;
    float mLiquidHeight;
    mVec3_c mStartPos;
    dHeapAllocator_c mAllocator;
    dCircleLightMask_c mLightMask;
    u8 m_unk_428[8];

    static int sm_IceBallCount[4];
    static int sm_IceBallAliveCount[4];
};
