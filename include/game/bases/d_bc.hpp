#pragma once

#include <game/bases/d_rc.hpp>

class dBg_ctr_c {
public:
    dActor_c *mpActor;
    u8 mPad1[0x9c];
    mVec3_c m_a0;
    mVec3_c m_ac;
    u8 mPad2[0x4];
    short *m_bc;
    u8 mPad4[0x8];
    int m_c8;
    u32 mFlags;
    int m_d0;

    void addDokanMoveDiff(mVec3_c *);
};

class dBc_c {
public:
    dBc_c();
    virtual ~dBc_c();

    bool checkRide();
    void checkLink();

    char mPad1[0x4c];
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
    char mPad8[0x5];
    u8 m_e5;
    char mPad9[0x6];
    u8 mLayer;

    static int checkWater(float, float, u8, float *);
    static int checkWaterDepth(float, float, u8, u8, float *);
    static bool checkGround(const mVec3_c *, float *, u8, u8, s8);
    static bool checkGround(const mVec3_c *, float *, int *, u8, u8, s8);
    static bool checkGroundHalf(const mVec3_c *, float *, u8, u8);
    static bool checkTenjou(const mVec3_c &, float &, u8, u8);
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

    bool getSakaUpDown(u8 direction);
    short getSakaAngle(u8 direction);
    short getSakaAngleBySpeed(float);
    int getSakaType();

    bool isHead()  { return mFlags & 0b00000000000000000000000000010101; }
    bool isWallL() { return mFlags & 0b00000000000000000000000000101010; }
    bool isFoot(); //  { return mFlags & 0b00000000000111111110000000000000; }
    bool isWallR() { return mFlags & 0b00111100000000000000000000000000; }

};
