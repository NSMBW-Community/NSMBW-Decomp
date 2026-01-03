#pragma once

#include <game/bases/d_s_stage.hpp>
#include <game/mLib/m_vec.hpp>
#include <constants/game_constants.h>

/// @brief Checkpoint manager.
/// This is intended to be used as a singleton and kept for the entire game session.
/// It deals with managing the active checkpoint and the collected star coins up until the checkpoint
/// - clearing the data if another level is entered, for example.
class dCyuukan_c {
public:
    dCyuukan_c(); ///< Creates a new checkpoint manager.
    virtual ~dCyuukan_c() {} ///< Destroys the checkpoint manager.
    void clear(); ///< Clears the active checkpoint.
    void courseIN(); ///< Check whether we want to clear the active checkpoint. Call this function when entering a course.
    bool checkEntry(); ///< If the player that hit the checkpoint leaves the party, the checkpoint is no longer active.

    /// @brief Sets the checkpoint data after hitting it.
    /// @param checkpointIndex The index of the checkpoint that was hit.
    /// @param nextGoto The next goto of the checkpoint.
    /// @param player The player who hit the checkpoint.
    /// @param ambushType The ambush type of the level.
    void setCyuukanData(int checkpointIndex, u8 nextGoto, s8 player, ulong ambushType);

    /// @brief Checks whether the checkpoint is active for a given world and course.
    /// @param idx The index of the checkpoint.
    /// @param world The world to check.
    /// @param course The course to check.
    bool isCyuukanStart(int idx, u8 world, u8 course);

    /// @brief Gets the player who hit checkpoint @p idx .
    /// @param idx The index of the checkpoint.
    s8 getPlrNo(int idx) const;

    void setPos(nw4r::math::VEC3 pos) { mPlayerSetPos = pos; }

    int mActiveCheckpointIndex; ///< The active checkpoint index.
    mVec3_c mPlayerSetPos; ///< Where the player should be set after respawning.
    u32 mAmbushType; ///< The ambush type of the current level.
    u8 mWorldNo; ///< The world in which the active checkpoint is located.
    u8 mCourseNo; ///< The course in which the active checkpoint is located.
    u8 mFileNo; ///< The file in which the active checkpoint is located.
    u8 mNextGoto; ///< The next goto of the active checkpoint.
    bool mIsKinopioInChuukan; ///< Whether Toad was saved before hitting the midway point.
    PLAYER_TYPE_e mCoinCollection[STAR_COIN_COUNT]; ///< The collected star coins up until the active checkpoint.
    PLAYER_TYPE_e mHitPlayer[CHECKPOINT_COUNT]; ///< The player who has hit each checkpoint.
};
