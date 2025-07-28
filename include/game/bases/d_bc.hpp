#pragma once

#include <game/bases/d_rc.hpp>

class dBcSensor_c {
public:
    u32 mFlags;
    int x;
    int y;

// public:
//     dBcSensor_c(u32 f, int x, int y) : mFlags(f), x(x), y(y) {}
};

// class dBcPointSensor_c : public dBcSensor_c {
//     int x;
//     int y;

// public:
//     dBcPointSensor_c(u32 f, int x, int y) : dBcSensor_c(f), x(x), y(y) {}
// };

class dBc_c {
public:
    enum WaterCheckResult_e {};

    dBc_c();
    virtual ~dBc_c();

    bool checkRide();
    void checkLink();

    char mPad1[0x1c];
    mVec3_c m_20;
    char mPad2[0x24];
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
    short mAdjacentSlopeAngle;
    char mPad7[0x12];
    float mIceSpeed;
    u8 m_e0;
    u8 m_e1;
    u8 m_e2;
    bool mMovingLeft;
    bool mGrounded;
    u8 mLineKind;
    char mPad9[0x6];
    u8 mLayer;

    static int checkWaterDepth(float, float, u8, u8, float *);
    static WaterCheckResult_e checkWater(float, float, u8, float *);
    static void checkBg(float, float, u8, u8, unsigned long);
    static bool checkWireNet(float x, float y, unsigned char layer);
    static bool checkGround(const mVec3_c *, float *, u8, u8, signed char);
    static bool findGround(mVec3_c *, float *, u32, u32); ///< @unofficial
    static u32 findWall(mVec3_c *, mVec3_c *, float *, u8, u32, dActor_c *); ///< @unofficial
    /// @unofficial
    void set(dActor_c *, const dBcSensor_c *, const dBcSensor_c *, const dBcSensor_c *);

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

    bool checkHead(ulong);
    bool checkWall(float *);
    bool checkFoot();
};
