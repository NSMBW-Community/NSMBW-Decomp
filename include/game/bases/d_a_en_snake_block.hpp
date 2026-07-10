#pragma once
#include <game/bases/d_enemy.hpp>
#include <game/bases/d_bg_ctr.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <game/mLib/m_3d/anm_mat_clr.hpp>
#include <game/mLib/m_3d/mdl.hpp>

#define MAX_SNAKE_BLOCK_COUNT 20

/// @brief A Snake Block which travels along a rail.
/// @paramtable
/// @statetable
/// @ingroup bases
class daEnSnakeBlock_c : public dEn_c {
public:

    /// @brief A single block segment within the Snake Block actor.
    /// @details Manages its own 3D model, animations, collision, and physics.
    class dBlock_c {
    public:
        /// @brief Creates a block.
        dBlock_c() : mpOwner(nullptr), m_1b4(0) {}

        /// @brief Destroys the block.
        virtual ~dBlock_c() { mBgCtr.release(); }

        /// @brief Initializes the 3D model, textures, and color animations.
        /// @param allocator The allocator to use for the model and animation data.
        void createMdl(mAllocator_c *allocator);

        /// @brief Initializes the block's collision.
        /// @param parent The parent Snake Block actor.
        /// @param blockPos The block position.
        /// @param icy Whether the surface must be icy.
        void initCollision(daEnSnakeBlock_c *parent, mVec3_c &blockPos, bool icy);

        /// @brief Sets the color animation for the block.
        /// @param name The animation name.
        void setAnmClr(const char *name);

        /// @brief Renders the block model.
        /// @param offset The offset from the block position, used for the shaking animation.
        void draw(const mVec3_c &offset);

        /// @brief Updates the block's position when in the @ref StateID_Collapse1 "Collapse1 state".
        void calcCollapse1(s8 *moveSequence);
        void calcCollision(); ///< Updates the block's collision based on its offset from the parent.
        void calcFallSpeed(); ///< Updates the block's falling speed.
        void calcAnm(); ///< Advances the animations for the block.

        mVec3_c getPos() const { return mPos; }
        void setPos(mVec3_c pos) { mPos = pos; }
        void posMove() { mPos += mSpeed; } ///< Moves the block by its @ref mSpeed "speed".

        void deleteBlock(); ///< Releases the model and associated animation resources.

        /// @brief Releases the block's collision.
        void releaseBgCtr() {
            if (mBgCtr.m_dc) {
                mBgCtr.release();
            }
        }

        static dBg_ctr_c::CallbackF callBackF; ///< The floor collision callback.
        static dBg_ctr_c::CallbackH callBackH; ///< The ceiling collision callback.
        static dBg_ctr_c::CallbackW callBackW; ///< The side collision callback.

        nw4r::g3d::ResFile mResFile;        ///< The resource file.
        m3d::mdl_c mModel;                  ///< The block model.
        m3d::anmMatClr_c mAnmClr;           ///< The color animation.
        nw4r::g3d::ResAnmTexSrt mResTexSrt; ///< The texture animation resource.
        m3d::anmTexSrt_c mAnmTexSrt;        ///< The texture animation.

        mVec3_c mPos;       ///< The block's position (center of block).
        mVec3_c mSpeed;     ///< The block' speed.
        mVec3_c mLastPos;   ///< The block's position in the previous movement.

        dBg_ctr_c mBgCtr;           ///< The collision.
        int mMoveSequenceIdx;       ///< The block's current step in the movement sequence.
        daEnSnakeBlock_c *mpOwner;  ///< The parent Snake Block actor.

        s16 m_1b4; ///< @unused
    };

    /// @brief Specialized block representing the head or tail of the Snake Block.
    /// @details Contains logic for advancing the block along the precalculated path.
    class dCtrlBlock_c : public dBlock_c {
    public:
        /// @brief The possible speed modes of a Snake Block.
        enum SnakeSpeed_e {
            SNAKE_SPEED_SLOW,
            SNAKE_SPEED_MEDIUM,
            SNAKE_SPEED_FAST,
            SNAKE_SPEED_COUNT
        };

        dCtrlBlock_c() {} ///< @copydoc dBlock_c::dBlock_c
        virtual ~dCtrlBlock_c() { mBgCtr.release(); } ///< @copydoc dBlock_c::~dBlock_c

        /// @brief Moves the block in the direction specified by the current sequence step.
        /// @param moveSequence The sequence of movement directions.
        /// @return Whether the block is ready for the next move.
        bool moveBlock(s8 *moveSequence);

        /// @brief Snaps the block to the grid and advances the sequence index to the next step.
        /// @param moveSequence The sequence of movement directions.
        /// @return Whether the block has reached the end of the movement sequence.
        bool nextTravelMove(s8 *moveSequence);

        SnakeSpeed_e mSnakeSpeed; ///< The speed mode for this block.
    };

    enum MoveDir_e {
        MOVE_DIR_NONE,
        MOVE_DIR_UP,
        MOVE_DIR_DOWN,
        MOVE_DIR_LEFT,
        MOVE_DIR_RIGHT,
        MOVE_DIR_COUNT
    };

    /// @brief Creates a Snake Block.
    daEnSnakeBlock_c() : mpMoveSequence(nullptr), mMoveSequenceIdx(0), mShakeTimer(0) {}

    /// @brief Destroys the Snake Block.
    virtual ~daEnSnakeBlock_c() {}

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();
    virtual void deleteReady();

    /// @brief Rasterizes the rail path into the @ref mpMoveSequence "sequence of movements".
    /// @details Diagonal steps are converted automatically to a staircase pattern.
    void initMoveSequence();
    void initBlocks(); ///< Initializes various components.
    void initBlockCollision(); ///< Initializes the collision for the head, body, and tail segments.
    void createMdl(); ///< Creates the model and animations for each block.
    void setStopState(); ///< Sets the state to transition to after the Snake Block reaches the end of the rail.

    void calcAnm(); ///< Advances the animations for all blocks in the Snake Block.
    void calcCollision(); ///< Updates each block's collision.
    void calcBlockPos(); ///< Updates each body block's position.
    void calcActorPos(); ///< Updates the Snake Block's position to match the head block's.

    /// @brief Checks whether the tail block is below the screen edge.
    /// @details Used by the collapse states to check if the actor can be deleted.
    bool chkCollapseDelete();

    /// @brief Checks whether the Snake Block is far past the right edge of the screen.
    /// @details Used to delete the actor if it goes off-screen.
    bool chkOffScreen();

    void deleteBlocks(); ///< Releases the resources for each block.

    dCtrlBlock_c &getHeadBlock() { return mCtrlBlock[0]; }
    dCtrlBlock_c &getTailBlock() { return mCtrlBlock[1]; }

    dHeapAllocator_c mAllocator; ///< The allocator.
    dCtrlBlock_c mCtrlBlock[2]; ///< The head and tail blocks.
    dBlock_c mBlocks[MAX_SNAKE_BLOCK_COUNT]; ///< The blocks between the head and tail. Only the first #mBlockCount of these are used.
    int mBlockCount; ///< The number of blocks between the head and tail.
    
    /// @brief The precalculated sequence of movements each block has to perform to reach the end of the rail.
    /// @details Each value is a MoveDir_e.
    s8 *mpMoveSequence;
    int mMoveSequenceIdx; ///< The current index into #mpMoveSequence.
    s16 mShakeTimer; ///< Counts down frames for the shake animation at the end of the rail.
    sStateID_c *mpStopState; ///< The state to transition to when the Snake Block stops.

    /// @brief The snake type.
    /// @decompnote{Has no effect in-game, but might have been intended for ice / no ice variants.}
    int mSnakeType;
    int mCollapseTimer; ///< The timer used by the @ref StateID_Collapse2 "Collapse2 state" to delay the fall of each block.
    int mGradientActiveBlocks; ///< The number of blocks currently displaying the gradient animation.
    int mGradientMaxBlocks; ///< The maximum number of blocks the gradient animation can cover.
    u8 mPad[4]; ///< @unused

    ACTOR_PARAM_CONFIG(BlockCount, 0, 4); ///< The number of blocks between the head and tail.
    ACTOR_PARAM_CONFIG(RailID, 4, 4); ///< The ID of the rail that the Snake Block travels along.
    ACTOR_PARAM_CONFIG(SnakeSpeed, 8, 2); ///< The snake's speed. See dCtrlBlock_c::SnakeSpeed_e.
    ACTOR_PARAM_CONFIG(RailStartIdx, 12, 8); ///< The rail node index where the snake spawns.
    ACTOR_PARAM_CONFIG(SnakeType, 20, 1); ///< The snake type. See #mSnakeType.
    ACTOR_PARAM_CONFIG(StopMode, 28, 4); ///< Determines how the Snake Block behaves after terminating.

    static const float sc_FallAccel; ///< The gravity acceleration applied during a collapse.
    static const float sc_FallMaxSpeed; ///< The terminal velocity applied during a collapse.
    static const float sc_SnakeSpeeds[dCtrlBlock_c::SNAKE_SPEED_COUNT]; ///< The speed values corresponding to SnakeSpeed_e. @hideinitializer
    static const float sc_SnakeSpeeds2[dCtrlBlock_c::SNAKE_SPEED_COUNT]; ///< Alternate array of speed values. @unused @hideinitializer
    static const mVec2_c sc_TravelDirs[MOVE_DIR_COUNT]; ///< The unit vector for each movement direction. @hideinitializer
    static const int scSnakeSoundID; ///< The sound effect ID played while the snake moves.
    static int g_SnakeNum; ///< Global counter for the active Snake Blocks.

    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Wait); ///< Waiting for a player to land on the Snake Block to start moving.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Move); ///< Moving along the rail.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Shake); ///< Shaking before collapsing.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Stop); ///< Stopped at the end of the rail.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse1); ///< Collapse mode 1: Blocks travel all the way to the final rail node and then fall.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse2); ///< Collapse mode 2: Blocks fall one after another, with a delay between each.
    STATE_FUNC_DECLARE(daEnSnakeBlock_c, Collapse3); ///< Collapse mode 3: Blocks fall all at once.
};
