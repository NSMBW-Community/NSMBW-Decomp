#pragma once

#include <game/bases/d_rc.hpp>

/// @unofficial
class dBcSensor_c;

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
    u32 mFlags;
    int m_d0;
    u8 mpPad5[0xc];
    int m_e0;

    void addDokanMoveDiff(mVec3_c *);
};

class dBc_c {
public:
    dBc_c();
    virtual ~dBc_c();
    void checkLink();
    bool checkRide();
    bool checkHead(unsigned long);
    void checkWall(float *);
    u16 getWallAttr(int);
    u32 checkWallEnm(float *);

    bool checkFootEnm();
    bool hasSensorFoot() { return mpSensorFoot != nullptr; }
    bool hasSensorHead() { return mpSensorHead != nullptr; }
    bool hasSensorWall() { return mpSensorWall != nullptr; }

    enum WATER_TYPE_e {
        WATER_CHECK_NONE,
        WATER_CHECK_WATER,
        WATER_CHECK_WATER_BUBBLE,
        WATER_CHECK_YOGAN,
        WATER_CHECK_POISON
    };

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
    dActor_c *mpNoHitActor;
    dBg_ctr_c *mpCtrHead;
    dBg_ctr_c *mpCtrFoot;
    dBg_ctr_c *mpCtrWall;
    dBg_ctr_c *mpCtrWalls[2];
    char mPad3[0x1c];
    u32 mFlags;
    u32 mPrevFlags;
    u32 mLastUnitType;
    u32 mLastUnitKind;
    u8 mOwningPlrNo;
    char mPad5[0x23];
    u8 mPlayerFlags;
    char mPad6[0x4];
    int m_c4;
    mAng mAdjacentSlopeAngle;
    char mPad7[0x12];
    float mIceSpeed;
    bool m_e0;
    u8 m_e1;
    u8 m_e2;
    bool mMovingLeft;
    bool mGrounded;
    u8 mAmiLine;
    char mPad9[0x6];
    u8 mLayer;

    static WATER_TYPE_e checkWater(float, float, u8, float *);
    static int checkWaterDepth(float, float, u8, u8, float *);
    static bool checkBg(float, float, u8, u8, unsigned long);
    static bool checkGround(const mVec3_c *, float *, u8, u8, s8);
    static bool checkGround(const mVec3_c *, float *, int *, u8, u8, s8);
    static bool checkGroundAngle(const mVec3_c *, float *, s16 *, u8, u8, s8, int *, int);
    static bool checkGroundHalf(const mVec3_c *, float *, u8, u8);
    static bool checkTenjou(const mVec3_c *, float *, u8, u8);
    static bool checkWall(const mVec3_c *, const mVec3_c *, float *p_hit_x, u8 layer, u8, dActor_c **p_hit_actor);
    static void getAirWaterHitPos(mVec2_c *);
    static void getAirWaterHitAngle(short *);

    bool checkRoofPlayer(const mVec3_c *, float *);
    u16 getFootAttr();
    u32 getSakaDir();
    int checkDokanLR(mVec3_c *, u8, int *, float, float);
    int checkDokanDown(mVec3_c *, int *);
    int checkDokanUp(mVec3_c *, int *);
    void setRideOnObjBg(dBg_ctr_c *, const mVec3_c &);
    bool checkWallPlayer(const mVec3_c *, const mVec3_c *, float *);
    u32 checkBgPlr(dActor_c *);
    u32 getHeadAttr();
    short getHeadSakaMoveAngle(u8 direction);
    short getSakaMoveAngle(u8 direction);
    void clearBgcSaveAll();

    bool getSakaUpDown(u8 direction);
    short getSakaAngle(u8 direction);
    short getSakaAngleBySpeed(float);
    int getSakaType();

    bool isHead()  { return mFlags & 0b00000000000000000000000000010101; }
    bool isWallL() { return mFlags & 0b00000000000000000000000000101010; }
    bool isFoot(); //  { return mFlags & 0b00000000000111111110000000000000; }
    bool isWallR() { return mFlags & 0b00111100000000000000000000000000; }

};
