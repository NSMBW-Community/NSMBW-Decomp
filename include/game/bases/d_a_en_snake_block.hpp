#pragma once
#include <game/bases/d_enemy.hpp>
#include <game/bases/d_bg_ctr.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_mat_clr.hpp>
#include <game/mLib/m_3d/mdl.hpp>

#define MAX_SNAKE_BLOCK_COUNT 20

/// @brief A snake block which travels along a rail.
/// @paramtable
/// @statetable
/// @ingroup bases
class daEnSnakeBlock_c : public dEn_c {
public:
    class dBlock_c {
    public:
        dBlock_c() : mpOwner(nullptr), m_1b4(0) {}
        virtual ~dBlock_c() { mBgCtr.release(); }

        void createMdl(mAllocator_c *allocator);
        void doDelete();

        void calcAnm();
        void setAnmClr(const char *name);

        void draw(const mVec3_c &offset);

        void initBgCtr(daEnSnakeBlock_c *parent, mVec3_c &blockPos, bool icy);
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

    class dCtrlBlock_c : public dBlock_c {
    public:
        /// @brief The possible speed modes of a snake block.
        enum SnakeSpeed_e {
            SNAKE_SPEED_SLOW,
            SNAKE_SPEED_MEDIUM,
            SNAKE_SPEED_FAST,
            SNAKE_SPEED_COUNT
        };

        dCtrlBlock_c() {}
        virtual ~dCtrlBlock_c() { mBgCtr.release(); }

        /// @brief Move this block in the direction of the current travel info.
        /// @param travelInfo Travel info array, used to determine the direction of movement.
        /// @return Whether the block is ready for the next move.
        bool moveBlock(s8 *travelInfo);
        /// @brief Start the move to the next position.
        /// @param travelInfo Travel info array, used to determine the direction of movement.
        /// @return Whether the block has reached the end of the travel info.
        bool nextTravelMove(s8 *travelInfo);

        SnakeSpeed_e mSnakeSpeed;
    };

    enum TravelDir_e {
        TRAVEL_DIR_NONE,
        TRAVEL_DIR_UP,
        TRAVEL_DIR_DOWN,
        TRAVEL_DIR_LEFT,
        TRAVEL_DIR_RIGHT,
        TRAVEL_DIR_COUNT
    };

    daEnSnakeBlock_c() : mpTravelInfo(nullptr), mTravelInfoIdx(0), mShakeTimer(0) {}
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

    dCtrlBlock_c mCtrlBlock[2]; ///< The head and tail blocks.
    dBlock_c mBlocks[MAX_SNAKE_BLOCK_COUNT]; ///< The blocks between the head and tail. Only the first mBlockCount of these are used.

    int mBlockCount; ///< The number of blocks between the head and tail.
    s8 *mpTravelInfo; ///< Values are of type TravelDir_e.
    int mTravelInfoIdx; ///< The current index into mpTravelInfo.
    short mShakeTimer; ///< Counts frames for the shake animation.

    sStateID_c *mpStopState; ///< The state to transition to when the snake block stops.
    int mSnakeType; ///< Used for custom snake types
    int mCollapseTimer;
    int mCreateAnmBlockIdx;
    int mCreateAnmBlockNum;
    u8 mPad[4];

    ACTOR_PARAM_CONFIG(BlockCount, 0, 4); ///< The number of blocks between the head and tail.
    ACTOR_PARAM_CONFIG(RailID, 4, 4); ///< The ID of the rail that the snake block travels along.
    ACTOR_PARAM_CONFIG(SnakeSpeed, 8, 2); ///< The snake's speed. See dCtrlBlock_c::SnakeSpeed_e.
    ACTOR_PARAM_CONFIG(RailStartIdx, 12, 8); ///< The index of the rail points that the snake block starts at.
    ACTOR_PARAM_CONFIG(SnakeType, 20, 1); ///< [Has no effect in-game, but might have been intended for ice / no ice variants]
    ACTOR_PARAM_CONFIG(StopMode, 28, 4); ///< Determines how the snake block behaves after terminating.

    static const float sc_FallAccel;
    static const float sc_FallMaxSpeed;
    static const float sc_SnakeSpeeds[3];
    static const float sc_SnakeSpeeds2[3];
    static const mVec2_c sc_TravelDirs[5];
    static const int scSnakeSoundID;
    static int g_SnakeNum; ///< Global counter for the active snake blocks.

    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Wait); ///< Waiting for a player to land on the block to start moving.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Move); ///< Moving along the rail.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Shake); ///< Shaking before collapsing.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Stop); ///< Stopped at the end of the rail.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse1); ///< Collapse mode 1: Blocks fall through last node.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse2); ///< Collapse mode 2: Blocks fall after another, with an 8-frame delay between each.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse3); ///< Collapse mode 3: Blocks fall all at once.
};
