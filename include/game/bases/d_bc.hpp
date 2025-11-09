#pragma once

#include <game/bases/d_rc.hpp>

enum SensorFlags {
    SENSOR_IS_POINT = 0,
    SENSOR_IS_LINE = 1
};

struct sBcSensorBase {
    u32 mFlags;
};

/// @unofficial
struct sBcSensorPoint {
    operator const sBcSensorBase *() const { return &mBase; }

    sBcSensorBase mBase;
    int mX, mY;
};

/// @unofficial
struct sBcSensorLine {
    operator const sBcSensorBase *() const { return &mBase; }

    sBcSensorBase mBase;
    int mLineA, mLineB;
    int mDistanceFromCenter;
};

class dBg_ctr_c;

class dBc_c {
public:
    enum Flag_e {
        FLAG_0 = BIT_FLAG(0),
        FLAG_1 = BIT_FLAG(1),
        FLAG_2 = BIT_FLAG(2),
        FLAG_3 = BIT_FLAG(3),
        FLAG_4 = BIT_FLAG(4),
        FLAG_5 = BIT_FLAG(5),
        FLAG_6 = BIT_FLAG(6),
        FLAG_7 = BIT_FLAG(7),
        FLAG_8 = BIT_FLAG(8),
        FLAG_9 = BIT_FLAG(9),
        FLAG_10 = BIT_FLAG(10),
        FLAG_11 = BIT_FLAG(11),
        FLAG_12 = BIT_FLAG(12),
        FLAG_13 = BIT_FLAG(13),
        FLAG_14 = BIT_FLAG(14),
        FLAG_15 = BIT_FLAG(15),
        FLAG_16 = BIT_FLAG(16),
        FLAG_17 = BIT_FLAG(17),
        FLAG_18 = BIT_FLAG(18),
        FLAG_19 = BIT_FLAG(19),
        FLAG_20 = BIT_FLAG(20),
        FLAG_21 = BIT_FLAG(21),
        FLAG_22 = BIT_FLAG(22),
        FLAG_23 = BIT_FLAG(23),
        FLAG_24 = BIT_FLAG(24),
        FLAG_25 = BIT_FLAG(25),
        FLAG_26 = BIT_FLAG(26),
        FLAG_27 = BIT_FLAG(27),
        FLAG_28 = BIT_FLAG(28),
        FLAG_29 = BIT_FLAG(29),

        FLAG_WALL_R =
            FLAG_0 | FLAG_2 | FLAG_4,
        FLAG_WALL_L =
            FLAG_1 | FLAG_3 | FLAG_5,
        FLAG_FOOT =
            FLAG_13 | FLAG_14 | FLAG_15 | FLAG_16 | FLAG_14 |
            FLAG_15 | FLAG_16 | FLAG_17 | FLAG_18 | FLAG_19 |
            FLAG_20,
        FLAG_HEAD =
            FLAG_26 | FLAG_27 | FLAG_28 | FLAG_29
    };

    enum WATER_TYPE_e {
        WATER_CHECK_NONE,
        WATER_CHECK_WATER,
        WATER_CHECK_WATER_BUBBLE,
        WATER_CHECK_YOGAN,
        WATER_CHECK_POISON
    };

    dBc_c();
    virtual ~dBc_c();

    void set(dActor_c *owner, const sBcSensorBase *foot, const sBcSensorBase *head, const sBcSensorBase *wall); ///< @unofficial

    void checkLink();
    bool checkRide();
    bool checkHead(ulong);
    s16 getSakaAngle(u8);
    int getSakaMoveAngle(u8);
    Flag_e checkWall(float *);
    Flag_e checkWallEnm(float *);
    Flag_e checkFoot();
    Flag_e checkFootEnm();
    u16 getFootAttr();

    bool hasSensorFoot() { return mpSensorFoot != nullptr; }
    bool hasSensorHead() { return mpSensorHead != nullptr; }
    bool hasSensorWall() { return mpSensorWall != nullptr; }

    void checkBombBreak(mVec2_c, mVec2_c);

    dActor_c *mpOwner;
    sBcSensorBase *mpSensorFoot;
    sBcSensorBase *mpSensorHead;
    sBcSensorBase *mpSensorWall;
    mVec3_c *mpOwnerPos;
    mVec3_c *mpOwnerLastPos;
    mVec3_c *mpOwnerSpeed;
    mVec3_c mPushForce;
    mVec2_c mOwnerPosDelta;
    mVec2_c m_34;
    mVec2_c m_3c;
    float m_44;
    float m_48;
    float m_4c;
    dRc_c *mpRc;
    dActor_c *mFriendActor;
    dBg_ctr_c *mCollidedBelow;
    dBg_ctr_c *mCollidedAbove;
    dBg_ctr_c *mCollidedAdj;
    dBg_ctr_c *mCollidedAdjForDirection[2];
    dBc_c *mPrevTrigBelowSensor;
    dBc_c *mPrevTrigAboveSensor;
    dBc_c *mPrevTrigAdjSensor;
    dBc_c *mPrevTrigAdjSensorForDirection[2];
    dBg_ctr_c *mLinkW[2];
    u32 mFlags;
    u32 mPrevFlags;
    char mPad3[0x8];
    s8 mOwningPlayerNo;
    char mPad4[0x4c];
    u8 mAmiLine;
    char pad4[0x2];
    u8 *mpLayer;

    u8 mLayer;

    static int getUnitType(float, float, u8);
    static int checkWaterDepth(float, float, u8, u8, float *);
    static WATER_TYPE_e checkWater(float, float, u8, float *);

    static u32 checkBg(float, float, u8, u8, unsigned long);
    static u32 checkWireNet(float x, float y, unsigned char layer);
    static u32 checkGround(const mVec3_c *, float *, u8, u8, signed char);
    static u32 checkTenjou(const mVec3_c *, float *, u8, u8);
    static u32 checkWall(const mVec3_c *, const mVec3_c *, float *, u8, u8, dActor_c **);

    u32 isWallR() { return mFlags & FLAG_WALL_R; }
    u32 isWallL()  { return mFlags & FLAG_WALL_L; }
    u32 isFoot(); // { return mFlags & FLAG_FOOT; }
    u32 isHead() { return mFlags & FLAG_HEAD; }
    u32 isCollision() { return mFlags & (FLAG_WALL_L | FLAG_WALL_R | FLAG_FOOT | FLAG_HEAD); }
};
