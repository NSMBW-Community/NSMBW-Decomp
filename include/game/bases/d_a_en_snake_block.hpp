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
    int m_cc;
    u32 mFlags;
    int m_d4;
    u8 mpPad5[0x8];
    bool m_e0;
    u8 m_e1;
    u8 m_e2;

    dBg_ctr_c();
    ~dBg_ctr_c();

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
        dBlock_c() : mpOwner(nullptr), m_1b4(0) {}
        virtual ~dBlock_c() { mBgCtr.release(); }

        void createMdl(dHeapAllocator_c *alloc);
        void doDelete();

        void calcAnm();
        void setAnmClr(const char *name);

        void draw(const mVec3_c &pos);

        void initBgCtr(daEnSnakeBlock_c *parent, mVec3_c &blockPos, int unused);
        void calcBgCtr();

        void setFallCollapse();
        void calcCollapse1(s8 *travelInfo);

        static void callBackF(dActor_c *self, dActor_c *other);
        static void callBackH(dActor_c *self, dActor_c *other);
        static void callBackW(dActor_c *self, dActor_c *other, u8);

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

        short m_1b4;
    };

    // @unofficial
    class dCtrlBlock_c : public dBlock_c {
    public:
        dCtrlBlock_c() {}
        virtual ~dCtrlBlock_c() { mBgCtr.release(); }

        bool calcPos(s8 *travelInfo);
        bool calcTravelPos(s8 *travelInfo);

        int mSnakeSpeedIdx;
    };

    daEnSnakeBlock_c() : mpTravelInfo(nullptr), mTravelInfoIdx(0), mShakeTime(0) {}
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

    dCtrlBlock_c *getHeadBlock() { return &mCtrlBlock[0]; }
    dCtrlBlock_c *getTailBlock() { return &mCtrlBlock[1]; }

    dHeapAllocator_c mAllocator;

    dCtrlBlock_c mCtrlBlock[2];
    dBlock_c mBlocks[20];

    int mBlockNum;
    s8 *mpTravelInfo;
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

    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Wait);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Move);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Shake);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Stop);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse1);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse2);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse3);
};
