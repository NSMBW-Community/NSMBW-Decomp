#pragma once

#include <game/bases/d_rc.hpp>

class dBcSensor_c {
public:
    u32 mFlags;
    int x;
    int y;
};

class dBg_ctr_c;

class dBc_c {
public:
    enum SensorFlags {
        SENSOR_TYPE_POINT = 0,
        SENSOR_TYPE_LINE = 1,

        SENSOR_1_TYPE = 0x00000001,
        SENSOR_2 = 0x00000002,
        SENSOR_4_IGNORE_SIMPLE_COLL = 0x00000004,
        SENSOR_8_CAN_ENTER_PIPE_MAYBE = 0x00000008,
        SENSOR_10 = 0x00000010,
        SENSOR_20 = 0x00000020,
        SENSOR_40 = 0x00000040,
        SENSOR_80 = 0x00000080,
        SENSOR_100 = 0x00000100,
        SENSOR_200 = 0x00000200,
        SENSOR_400 = 0x00000400,
        SENSOR_800 = 0x00000800,
        SENSOR_1000 = 0x00001000,
        SENSOR_2000 = 0x00002000,
        SENSOR_4000 = 0x00004000,
        SENSOR_8000 = 0x00008000,
        SENSOR_10000 = 0x00010000,
        SENSOR_20000 = 0x00020000,
        SENSOR_40000 = 0x00040000,
        SENSOR_80000 = 0x00080000,
        SENSOR_100000 = 0x00100000,
        SENSOR_200000 = 0x00200000,
        SENSOR_400000 = 0x00400000,
        SENSOR_800000 = 0x00800000,
        SENSOR_1000000 = 0x01000000,
        SENSOR_2000000 = 0x02000000,
        SENSOR_4000000 = 0x04000000,
        SENSOR_8000000 = 0x08000000,
        SENSOR_10000000 = 0x10000000,
        SENSOR_20000000 = 0x20000000,
        SENSOR_40000000 = 0x40000000,
        SENSOR_80000000_NO_COLLISIONS = 0x80000000,

        SENSOR_WALL = 0x00001fff,
        SENSOR_FEET = 0x03ffe000,
        SENSOR_HEAD = 0xfc000000,
    };

    dBc_c();
    virtual ~dBc_c();

    bool checkRide();
    void checkLink();
    bool checkHead(ulong);
    int getSakaMoveAngle(u8);
    int getSakaAngle(u8);
    SensorFlags checkFoot();
    SensorFlags checkWall(float *);
    SensorFlags checkWallEnm(float *);


    bool checkFootEnm();
    bool hasSensorFoot() { return mpSensorFoot != nullptr; }
    bool hasSensorHead() { return mpSensorHead != nullptr; }
    bool hasSensorWall() { return mpSensorWall != nullptr; }
    u32 getFootAttr();

    enum WaterCheckResult_e {
        R0 = 0,
        R1 = 1,
        R2 = 2,
        R3 = 3,
        R4 = 4
    };

    dActor_c *mpOwner;
    dBcSensor_c *mpSensorFoot;
    dBcSensor_c *mpSensorHead;
    dBcSensor_c *mpSensorWall;
    mVec3_c *mpOwnerPos;
    mVec3_c *mpOwnerLastPos;
    mVec3_c *mpOwnerSpeed;
    mVec3_c m_20;
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
    u8 mLineKind;
    char pad4[0x2];
    u8 *mpLayer;
    u8 mLayer;

    static int checkWaterDepth(float, float, u8, u8, float *);
    static WaterCheckResult_e checkWater(float, float, u8, float *);
    static void checkBg(float, float, u8, u8, unsigned long);
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
