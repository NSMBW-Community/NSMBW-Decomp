#pragma once
#include <game/bases/d_enemy.hpp>
#include <game/bases/d_bg_ctr.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_mat_clr.hpp>
#include <game/mLib/m_3d/mdl.hpp>

class daEnSnakeBlock_c : public dEn_c {
public:
    // @unofficial
    class dBlock_c {
    public:
        dBlock_c() : mpOwner(nullptr), m_1b4(0) {}
        virtual ~dBlock_c() { mBgCtr.release(); }

        void createMdl(mAllocator_c *allocator);
        void doDelete();

        void calcAnm();
        void setAnmClr(const char *name);

        void draw(const mVec3_c &offset);

        void initBgCtr(daEnSnakeBlock_c *parent, mVec3_c &blockPos, int unused);
        void calcBgCtr();

        void setFallCollapse();
        void calcCollapse1(s8 *travelInfo);

        mVec3_c getPos() const { return mPos; }
        void setPos(mVec3_c pos) { mPos = pos; }
        void releaseBgCtr() {
            if (mBgCtr.m_dc) {
                mBgCtr.release();
            }
        }
        void updatePos() {
            mPos += mSpeed;
        }

        static dBg_ctr_c::CallbackF callBackF;
        static dBg_ctr_c::CallbackH callBackH;
        static dBg_ctr_c::CallbackW callBackW;

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

    enum TravelDir_e {
        TRAVEL_DIR_NONE,
        TRAVEL_DIR_UP,
        TRAVEL_DIR_DOWN,
        TRAVEL_DIR_LEFT,
        TRAVEL_DIR_RIGHT
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

    dCtrlBlock_c &getHeadBlock() { return mCtrlBlock[0]; }
    dCtrlBlock_c &getTailBlock() { return mCtrlBlock[1]; }

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

    ACTOR_PARAM_CONFIG(Param_00, 0, 4);
    ACTOR_PARAM_CONFIG(RailIdx, 4, 4);
    ACTOR_PARAM_CONFIG(Param_08, 8, 2);
    ACTOR_PARAM_CONFIG(Param_0c, 12, 8);
    ACTOR_PARAM_CONFIG(RailStartIdx, 12, 8);
    ACTOR_PARAM_CONFIG(Param_14, 20, 1);
    ACTOR_PARAM_CONFIG(StopState, 28, 4);

    static const float sc_FallAccel;
    static const float sc_FallMaxSpeed;
    static const float sc_snakeSpeeds[3];
    static const float sc_snakeSpeeds2[3];
    static mVec2_c sc_ctrlPosMods[5];
    static int sc_glbSnakeNum;
    static const int scSnakeSoundID;

    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Wait);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Move);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Shake);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Stop);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse1);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse2);
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse3);
};
