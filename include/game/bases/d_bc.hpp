#pragma once

#include <game/bases/d_rc.hpp>

/// @unofficial
struct dBcSensor_c {
    u32 mFlags;
    int x;
    int y;
};

class dBg_ctr_c;

class dBc_c {
public:
    enum SensorFlags {
        SENSOR_IS_POINT = BIT_FLAG(0),
        SENSOR_1 = BIT_FLAG(1),
        SENSOR_2 = BIT_FLAG(2),
        SENSOR_3 = BIT_FLAG(3),
        SENSOR_4 = BIT_FLAG(4),
        SENSOR_5 = BIT_FLAG(5),
        SENSOR_6 = BIT_FLAG(6),
        SENSOR_7 = BIT_FLAG(7),
        SENSOR_8 = BIT_FLAG(8),
        SENSOR_9 = BIT_FLAG(9),
        SENSOR_10 = BIT_FLAG(10),
        SENSOR_11 = BIT_FLAG(11),
        SENSOR_12 = BIT_FLAG(12),
        SENSOR_13 = BIT_FLAG(13),
        SENSOR_14 = BIT_FLAG(14),
        SENSOR_15 = BIT_FLAG(15),
        SENSOR_16 = BIT_FLAG(16),
        SENSOR_17 = BIT_FLAG(17),
        SENSOR_18 = BIT_FLAG(18),
        SENSOR_19 = BIT_FLAG(19),
        SENSOR_20 = BIT_FLAG(20),
        SENSOR_21 = BIT_FLAG(21),
        SENSOR_22 = BIT_FLAG(22),
        SENSOR_23 = BIT_FLAG(23),
        SENSOR_24 = BIT_FLAG(24),
        SENSOR_25 = BIT_FLAG(25),
        SENSOR_26 = BIT_FLAG(26),
        SENSOR_27 = BIT_FLAG(27),
        SENSOR_28 = BIT_FLAG(28),
        SENSOR_29 = BIT_FLAG(29),
        SENSOR_30 = BIT_FLAG(30),
        SENSOR_31_NO_COLLISIONS = BIT_FLAG(31),

        SENSOR_HEAD =
            SENSOR_IS_POINT | SENSOR_2 | SENSOR_4,
        SENSOR_WALL_L =
            SENSOR_1 | SENSOR_3 | SENSOR_5,
        SENSOR_FOOT =
            SENSOR_13 | SENSOR_14 | SENSOR_15 | SENSOR_16 | SENSOR_14 |
            SENSOR_15 | SENSOR_16 | SENSOR_17 | SENSOR_18 | SENSOR_19 |
            SENSOR_20 | SENSOR_21,
        SENSOR_WALL_R =
            SENSOR_26 | SENSOR_27 | SENSOR_28 | SENSOR_29
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
    void checkLink();
    bool checkRide();
    bool checkHead(ulong);
    int getSakaAngle(u8);
    int getSakaMoveAngle(u8);
    SensorFlags checkWall(float *);
    SensorFlags checkWallEnm(float *);
    SensorFlags checkFoot();
    SensorFlags checkFootEnm();

    bool hasSensorFoot() { return mpSensorFoot != nullptr; }
    bool hasSensorHead() { return mpSensorHead != nullptr; }
    bool hasSensorWall() { return mpSensorWall != nullptr; }
    u32 getFootAttr();

    dActor_c *mpOwner;
    dBcSensor_c *mpSensorFoot;
    dBcSensor_c *mpSensorHead;
    dBcSensor_c *mpSensorWall;
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
    char pad3[0x55];
    u8 mAmiLine;
    char pad4[0x2];
    u8 *mpLayer;
    u8 mLayer;

    static int checkWaterDepth(float, float, u8, u8, float *);
    static WATER_TYPE_e checkWater(float, float, u8, float *);
    static bool checkBg(float, float, u8, u8, unsigned long);
    static bool checkWireNet(float x, float y, unsigned char layer);
    static bool checkGround(const mVec3_c *, float *, u8, u8, signed char);
    static bool findGround(mVec3_c *, float *, u32, u32); ///< @unofficial
    static u32 findWall(mVec3_c *, mVec3_c *, float *, u8, u32, dActor_c *); ///< @unofficial
    void set(dActor_c *, const dBcSensor_c *, const dBcSensor_c *, const dBcSensor_c *); ///< @unofficial

    bool isHead()  { return mFlags & 0b00000000000000000000000000010101; }
    bool isWallL() { return mFlags & 0b00000000000000000000000000101010; }
    bool isFoot(); //  { return mFlags & 0b00000000000111111110000000000000; }
    bool isWallR() { return mFlags & 0b00111100000000000000000000000000; }

};
