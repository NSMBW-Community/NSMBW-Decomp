#pragma once
#include <game/bases/d_enemy.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_mat_clr.hpp>
#include <game/mLib/m_3d/mdl.hpp>

class dBg_ctr_c {
public:
    dActor_c *mpActor;
    u8 mPad1[0x9c];
    mVec2_c m_a0;
    mVec2_c m_ac;
    u8 mPad2[0xc];
    short *m_bc;
    u8 mPad4[0x8];
    int m_c8;
    u32 mFlags;
    int m_d0;
    u8 mpPad5[0x8];
    bool m_dc;
    u8 mpPad6[3];
    int m_e0;

    void release();
    void entry();
    void set(dActor_c *, float, float, float, float, void *, void *, void *, int, int, void *);
    void calc();
    void setOfs(float, float, float, float, void *);
};

class daEnSnakeBlock_c : public dEn_c {
public:
    // @unofficial
    class dBlock_c {
    public:
        dBlock_c();
        virtual ~dBlock_c() {}

        void createMdl(dHeapAllocator_c *alloc);
        void doDelete();

        void calcAnm();
        void setAnmClr(const char *name);

        void draw(mVec3_c *pos);

        void initBgCtr(daEnSnakeBlock_c *parent, mVec3_c *blockPos, int unused);
        void calcBgCtr();

        void setFallCollapse();
        void calcCollapse1(u8 *travelInfo);

        static void callBackF(dActor_c *, dActor_c *);
        static void callBackH(dActor_c *, dActor_c *);
        static void callBackW(dActor_c *, dActor_c *, u8);

        nw4r::g3d::ResFile mResFile;
        m3d::mdl_c mModel;
        m3d::anmMatClr_c mAnmClr;
        nw4r::g3d::ResAnmTexSrt mResTexSrt;
        m3d::anmTexSrt_c mAnmTexSrt;

        mVec3_c mPos;
        mVec3_c mSpeed;
        mVec3_c mLastPos;

        dBg_ctr_c mBgCtr;
        int mTravelInfoIdx;
        daEnSnakeBlock_c *mpOwner;

        short _1B4;
    };

    // @unofficial
    class dCtrlBlock_c : public dBlock_c {
    public:
        dCtrlBlock_c();
        virtual ~dCtrlBlock_c() {}

        bool calcPos(u8 *travelInfo);
        bool calcTravelPos(u8 *travelInfo);

        int mSnakeSpeedIdx;
    };

    daEnSnakeBlock_c();
    virtual ~daEnSnakeBlock_c() {}

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();
    virtual void deleteReady();

    void initBlock();
    void initBlockPath();

    void calcAnm();
    void createMdl();
    void deleteBlock();

    void setStopState();

    void calcBgCtr();
    void initTravelInfo();

    void setBlockPos();
    void setActorPos();

    bool chkCollapseDelete(); ///< Returns if the Tail is below the screen edge (meaning the collapse has ended, and the actor can be deleted)
    bool chkOffScreen(); ///< Returns if the Snake Block is far past the right edge of the screen (used to delete it)

    dHeapAllocator_c mAllocator;

    dCtrlBlock_c mHead;
    dCtrlBlock_c mTail;
    dBlock_c mBlocks[20];

    int mBlockNum;
    u8 *mpTravelInfo;
    int mTravelInfoIdx;
    short mShakeTime;

    sStateID_c *mpStopState;
    int mSnakeType; ///< Used for custom snake types
    int mCollapse2Idx;
    int mCreateAnmBlockIdx;
    int mCreateAnmBlockNum;
    u8 _pad[4];

    static float sc_snakeSpeeds[3];
    static sStateID_c *sc_stopStates[4];
    static mVec2_c sc_ctrlPosMods[5];
    static int sc_glbSnakeNum;
    static mVec2_c sc_collapseSpeeds[5];

    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Wait);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Move);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Shake);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Stop);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse1);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse2);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse3);
};
