#pragma once
#include <game/bases/d_cd.hpp>

/**
 * @brief Manages movement along rails.
 */
class dRail_c {
public:
    virtual ~dRail_c() {} ///< Destroys the rail handler.
    virtual void calcSpeed(); ///< Calculates and applies the new speed based on the current and next node.

    /**
     * @brief Initializes the rail with the specified ID and parameters.
     * @param id The ID of the rail to use.
     * @param pos Pointer to the position vector of the object on the rail.
     * @param speed Pointer to the speed vector of the object on the rail.
     * @param speedF Pointer to the forward speed of the object on the rail.
     * @param startIndex The starting node index on the rail.
     * @param reverse Whether the object should travel the rail in reverse.
     * @return Whether the rail was successfully initialized.
     */
    bool set(u8 id, mVec3_c *pos, mVec3_c *speed, float *speedF, u16 startIndex, u8 reverse);

    /// @brief Advances the node index to the next target point.
    /// @return Whether the end of the rail has been reached.
    bool calcNextPoint();
    void calcTargetPos(); ///< Calculates the target position based on the next node.
    bool execute(); ///< Executes the rail movement logic. Returns whether a target node has been reached.
    bool checkArrive(); ///< Checks whether the rail has arrived at the next target node.

    static sRailInfoData *getRailInfoP(u8 id); ///< Gets the rail data for the specified ID.

    /// @unofficial
    enum RAIL_FLAGS_e {
        RAIL_FLAG_LOOP = BIT_FLAG(1),
        RAIL_FLAG_DELAYING = BIT_FLAG(2),
        RAIL_FLAG_DISABLED = BIT_FLAG(3)
    };

    /// @unofficial
    enum NODE_FLAGS_e {
        NODE_FLAG_1 = BIT_FLAG(0)
    };

    sRailNodeData *mpNodes; ///< The array of rail nodes.
    mVec2_c mTargetPos; ///< The position of the next node.
    mVec3_c *mpPos; ///< Reference to the position of the object on the rail.
    mVec3_c *mpSpeed; ///< Reference to the speed vector of the object on the rail.
    float *mpSpeedF; ///< Reference to the forward speed of the object on the rail.
    u16 mCount; ///< The number of nodes in the rail.
    short mIdxCurr; ///< The current node index.
    short mIdxNext; ///< The next node index.
    u16 mDelayTimer; ///< By how many frames to delay before starting movement from a node.
    u16 mAngle; ///< The current angle of movement.
    u16 mFlags; ///< The rail flags.
    u16 mNodeFlags; ///< The current node flags.
    u8 mReverse; ///< Whether to go through the rail in reverse.
    int mAngleCalcTimer; ///< Cooldown timer for angle recalculation.
    bool mBounce; ///< Whether to reverse direction at the ends of the rail (only applicable if not looping).
};
