#pragma once
#include <types.h>
#include <game/mLib/m_vec.hpp>

class dActor_c;

///< @unofficial
enum CC_SHAPE_e {
    CC_SHAPE_BOX, ///< Rectangular collider
    CC_SHAPE_CIRCLE, ///< Circular / elliptical collider
    CC_SHAPE_DAIKEI_UD, ///< Trapezoid-shaped collider (left/right sides are parallel)
    CC_SHAPE_DAIKEI_LR, ///< Trapezoid-shaped collider (top/bottom sides are parallel)
};

///< @unofficial
enum CC_STATUS_FLAG_e {
    CC_STATUS_NONE = 0,
    CC_STATUS_NO_REVISION = BIT_FLAG(0), ///< Don't set the collision offset if a collision occurs
    /**
    * When another collider collides with this one,
    * don't update the result or execute the callback
    * on the other collider.
    */
    CC_STATUS_NO_PASS_INFO = BIT_FLAG(2),
};

///< @unofficial
enum CC_INFO_e {
    CC_NO_HIT = BIT_FLAG(1), ///< Disables all collisions with this collider
};

///< @unofficial
enum CC_KIND_e {
    CC_KIND_PLAYER,
    /**
    * This collider can attack, which means it will use mAttackCategory and
    * mAttackCategoryInteract to further check if the colliders will collide.
    */
    CC_KIND_PLAYER_ATTACK,
    CC_KIND_YOSHI,
    CC_KIND_ENEMY,
    CC_KIND_BALLOON,
    CC_KIND_ITEM,
    CC_KIND_TAMA,
    CC_KIND_KILLER,
    CC_KIND_GOAL_POLE
};

///< @unofficial
enum CC_ATTACK_e {
    CC_ATTACK_NONE,
    CC_ATTACK_FIREBALL,
    CC_ATTACK_ICEBALL,
    CC_ATTACK_STAR,
    CC_ATTACK_ICE_BREAK,
    CC_ATTACK_SLIP,
    CC_ATTACK_KOOPA_FIRE,
    CC_ATTACK_HIP_ATTACK,
    CC_ATTACK_WIRE_NET,
    CC_ATTACK_SHELL,
    CC_ATTACK_PENGUIN_SLIDE,
    CC_ATTACK_SPIN,
    CC_ATTACK_UNK12,
    CC_ATTACK_SPIN_FALL,
    CC_ATTACK_FIRE_2,
    CC_ATTACK_YOSHI_EAT,
    CC_ATTACK_YOSHI_MOUTH,
    CC_ATTACK_CANNON,
    CC_ATTACK_SPIN_LIFT_UP,
    CC_ATTACK_YOSHI_BULLET,
    CC_ATTACK_YOSHI_FIRE,
    CC_ATTACK_ICE_2,
    CC_ATTACK_SAND_PILLAR,
    CC_FLAG_ATTACK_ALL = 0xFFFFFFFF
};

class dCc_c;

/**
* @brief A structure that contains information about a collider.
* @unofficial
*/
struct sCcDatNewF {
    mVec2_POD_c mOffset; ///< The offset of the collider.

    ///< @brief The size of the collider.
    ///< Note: This is the distance from the center to the edge, so half the actual size.
    mVec2_POD_c mSize;

    u8 mKind; ///< The type of this collider. See CC_KIND_e.
    u8 mAttack; ///< The attack type of this collider. See CC_ATTACK_e.

    ///< @brief Which types this collider should be able to collide with.
    ///< This is a bitfield with the bits enumerated by CC_KIND_e.
    u32 mVsKind;

    ///< @brief Which attack types this collider should be able to receive.
    ///< This is a bitfield with the bits enumerated by CC_ATTACK_e.
    u32 mVsDamage;

    u16 mStatus; ///< Status flags for this collider. See CC_STATUS_FLAG_e.

    void (*mCallback)(dCc_c *self, dCc_c *target); ///< The callback to execute when a collision occurs.
};

/**
 * @brief Collider ("Collision Check") class - handles collisions between actors.
 *
 * It also includes logic that handles collisions
 * within a horizontally looping stage (like 2-C).
 */
class dCc_c {
public:
    ///< @unofficial
    class U32Holder {
    public:
        U32Holder(u32 value) : mValue(value) {}

        u32 mValue;
    };

    ///< @unofficial
    struct GlobalData_t {
        u32 a;
        U32Holder b;
        U32Holder c;
        bool d, e;
    };

public:
    dCc_c(); ///< Constructs a new collider.
    virtual ~dCc_c(); ///< Destroys the collider.

    void clear(); ///< Clear the data related to previous collisions.

    void entry(); ///< Places this collider in the collider list.
    void release(); ///< Removes this collider from the collider list.

    /**
     * @brief Registers an owner actor to this collider and sets the collider data.
     * @param actor The actor to register.
     * @param collInfo The collider data to set.
     */
    void set(dActor_c *actor, sCcDatNewF *collInfo);

    /**
     * @brief Registers an owner actor to this collider and sets the collider data.
     * @param actor The actor to register.
     * @param collInfo The collider data to set.
     * @param amiLine The chainline fence layer to set.
     */
    void set(dActor_c *actor, sCcDatNewF *collInfo, u8 amiLine);

    /// Sets a friend actor for this collider.
    void setFriendActor(dActor_c *actor) { mFriendActor = actor; }

    dActor_c *getOwner() const { return mpOwner; } ///< Gets the owner actor of this collider.

    /**
     * @brief Gets the result of a hit check.
     * @param mask The mask to check.
     * @return The result of the hit check.
     */
    u16 isHit(u16 mask) const;

    /**
     * @brief Gets the result of an attack hit check.
     * @param mask The mask to check.
     * @return The result of the attack hit check.
     */
    u16 isHitAtDmg(u16 mask) const;

    float getTopPos(); ///< Gets the Y position of the top of the collider.
    float getUnderPos(); ///< Gets the Y position of the bottom of the collider.
    float getCenterPosY(); ///< Gets the Y position of the center of the collider.

    float getRightPos(); ///< Gets the X position of the right side of the collider.
    float getLeftPos(); ///< Gets the X position of the left side of the collider.
    float getCenterPosX(); ///< Gets the X position of the center of the collider.

    /// Gets the center of the collider as a vector.
    inline mVec2_c getCenterVec() {
        return mVec2_c(getCenterPosX(), getCenterPosY());
    }

    /**
     * @brief Checks if this collider is inside another collider.
     *
     * @param other The collider to check against.
     */
    bool isInside(dCc_c *other);

private:
    float getTrpOffset(int idx) {
        return mTrpOffsets[idx];
    }

public:
    /**
     * @brief Checks for collisions between two colliders.
     *
     * @param c1 The first collider.
     * @param c2 The second collider.
     * @param active Whether to update the result and execute the callback if a collision occurs.
     * @return Whether the first collider collided with the second collider.
     */
    static bool checkCollision(dCc_c *c1, dCc_c *c2, int active);

    static void execute(); ///< Check all colliders against each other for collisions.

    /**
     * @brief Clears the collider list.
     *
     * It also sets the hit check to the correct type (normal / looping stage).
     * Note that this does not clean up the colliders themselves!
     */
    static void reset();

    float getCollPosX() const { return mCollPos.x; }
    float getCollPosY() const { return mCollPos.y; }

private:
    /**
     * @brief A hit check function for rectangular colliders. Used in _hitCheckNormal and _hitCheckLoop.
     * @param c1 The first collider.
     * @param c2 The second collider.
     * @param pos1 The position of the first collider.
     * @param pos2 The position of the second collider.
     */
    static bool _hitCheckSquare(dCc_c *c1, dCc_c *c2, mVec2_c pos1, mVec2_c pos2);

    /// Check two rectangular colliders against each other for collisions without stage looping.
    static bool _hitCheckNormal(dCc_c *c1, dCc_c *c2);
    /// Check two rectangular colliders against each other for collisions with stage looping.
    static bool _hitCheckLoop(dCc_c *c1, dCc_c *c2);
    /// Check two circle colliders against each other for collisions.
    static bool _hitCheckCircle(dCc_c *c1, dCc_c *c2);
    /// Check a rectangular and a circle collider against each other for collisions.
    static bool _hitCheckBoxCircle(dCc_c *c1, dCc_c *c2);

    static int _lineCheckUD(mVec2_c p1, mVec2_c p2, float p3, float p4);
    /// Check a rectangular collider against a trapezoid-shaped collider for collisions.
    static bool _hitCheckDaikeiUD_R(dCc_c *ccBox, dCc_c *ccTrp);
    /// Check a trapezoid-shaped collider against a rectangular collider for collisions.
    static bool _hitCheckDaikeiUD(dCc_c *ccTrp, dCc_c *ccBox);

    static int _lineCheckLR(mVec2_c p1, mVec2_c p2, float p3, float p4);
    /// Check a rectangular collider against a trapezoid-shaped collider for collisions.
    static bool _hitCheckDaikeiLR_R(dCc_c *ccBox, dCc_c *ccTrp);
    /// Check a trapezoid-shaped collider against a rectangular collider for collisions.
    static bool _hitCheckDaikeiLR(dCc_c *ccTrp, dCc_c *ccBox);

public:
    float getXOffset(int idx) { return mCollOffsetX[idx]; }
    float getYOffset(int idx) { return mCollOffsetY[idx]; }

    dActor_c *mpOwner; ///< The actor this collider belongs to.
    dActor_c *mFriendActor; ///< A second actor that this collider will not collide with.

    u32 unk2; ///< [Unused (?)].

    dCc_c *mpNext; ///< The next collider in the list.
    dCc_c *mpPrev; ///< The previous collider in the list.

    u32 unk3; ///< [Unused (?)].

    sCcDatNewF mCcData; ///< The collision data of this collider.

    /**
     * @brief The X or Y offset of the four corners of a trapezoid-shaped collider.
     *
     * Relative to the center of the collider.
     * If mShape is CC_SHAPE_DAIKEI_UD, this is the Y offset.
     * If mShape is CC_SHAPE_DAIKEI_LR, this is the X offset.
     */
    float mTrpOffsets[4];

    /**
     * @brief The X offset for a collision.
     *
     * One entry per category. Each entry describes by how much the collider must be
     * offset in the X direction in order to not collide with the other collider.
     */
    float mCollOffsetX[8];
    /**
     * @brief The Y offset for a collision.
     *
     * One entry per category. Each entry describes by how much the collider must be
     * offset in the Y direction in order to not collide with the other collider.
     */
    float mCollOffsetY[8];

    mVec2_c mCollPos; ///< The position where the last collision occurred.

    u16 mCollidedWith; ///< The categories of the previously collided with colliders.
    u16 mAttSent; ///< The attack types sent by this collider in the previous collisions.
    u16 mAttReceived; ///< The attack types received by this collider in the previous collisions.

    u8 mShape; ///< The shape of the collider. See @ref CC_SHAPE_e .

    /**
     * @brief The non-collide mask for this collider.
     *
     * If the same bit is set in a second actor's non-collide mask,
     * the two actors will not collide.
     */
    u8 mAmiLine;

    /**
     * @brief The layer this collider is on.
     *
     * Colliders can only collide with other colliders on the same layer.
     */
    u8 mLayer;

    u8 mInfo; ///< Info flags for this collider. See CC_INFO_e.

private:
    bool mIsLinked; ///< Whether this collider has been placed in the collider list.

private:
    typedef bool (*hitCheck)(dCc_c *, dCc_c *);

    /**
     * @brief The hit check function for each combination of collider shapes.
     *
     * The first index is the shape of the first collider and the second index
     * is the shape of the second collider.
     */
    static hitCheck _hitCheck[4][4];

    static dCc_c *mEntryN; ///< The first collider in the list.
    static dCc_c *mEntryB; ///< The last collider in the list.
};
