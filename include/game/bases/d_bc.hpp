#pragma once

#include <game/bases/d_rc.hpp>

enum SensorFlags {
    SENSOR_IS_POINT = 0,
    SENSOR_IS_LINE = 1,
    SENSOR_TYPE_MASK = BIT_FLAG(0),
};

class sBcSensorPoint;
class sBcSensorLine;
class sBcSensorIf_c {
public:
    sBcSensorPoint *asPointSensor() { return (sBcSensorPoint *) this; }
    sBcSensorLine *asLineSensor() { return (sBcSensorLine *) this; }

    long mFlags;
};

/// @unofficial
struct sBcSensorPoint {
    operator sBcSensorIf_c *() const { return (sBcSensorIf_c *) this; }

    u32 mFlags;
    int mX, mY;
};

/// @unofficial
struct sBcSensorLine {
    operator sBcSensorIf_c *() const { return (sBcSensorIf_c *) this; }

    u32 mFlags;
    long mLineA, mLineB;
    long mOffset;
};

/// @unofficial
class sBcPlayerPointData {
public:
    sBcSensorLine mFoot;
    sBcSensorLine mHead;
    sBcSensorLine mWall;
    sBcSensorLine mVine;
};

class dBg_ctr_c {
public:
    dActor_c *mpActor;
    u8 mPad1[0x9c];
    mVec2_c m_a0;
    mVec2_c m_ac;
    u8 mPad2[0xc];
    short *mRotation;
    short m_c0;
    short m_c2;
    u8 mPad4[0x4];
    int m_c8;
    u32 mFlags;
    int m_d0;
    u8 mpPad5[0xc];
    int m_e0;

    void addDokanMoveDiff(mVec3_c *);
};

class dBc_c {
public:
    /// @unofficial
    enum Flag_e {
        FLAG_NONE = 0,
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
        FLAG_30 = BIT_FLAG(30),
        FLAG_31 = BIT_FLAG(31),

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

    /// @unofficial
    enum WATER_TYPE_e {
        WATER_CHECK_NONE,
        WATER_CHECK_WATER,
        WATER_CHECK_WATER_BUBBLE,
        WATER_CHECK_YOGAN,
        WATER_CHECK_POISON
    };

    /// @unofficial
    enum SakaType_e {
        SAKA_NONE = 10
    };

    typedef void (dBc_c::*AttrFunc)();

    dBc_c();
    virtual ~dBc_c();

    void init();
    void set(dActor_c *, sBcSensorIf_c *, sBcSensorIf_c *, sBcSensorIf_c *); ///< @unofficial
    void clear();
    void clearDamage();

    static bool isBGCoin(ulong);
    bool getBGCoin(ulong flags, ulong p2, ulong p3, mVec3_c *pos, int p5);

    bool _checkBreakUnit(ulong p1, u8 p2, float x, float y);
    static bool checkBreakUnit(ulong p1, u8 p2, float x, float y, u8 layer, s8 plrNo, u8 actorKind);
    bool setHatenaBlockHit(ulong p1, u8 p2, float x, float y);
    u32 setBreakBlockHit(ulong p1, u8 p2, float x, float y);

    u8 getActorKind();

    bool setBgDamageFlag(int, float, float);
    void checkLink();
    bool checkRide();
    s16 getSakaAngle(u8);
    s16 getSakaMoveAngle(u8);

    u32 checkFoot();
    u32 checkFootEnm();
    u32 checkFootPoint(sBcSensorPoint *, ulong); ///< @unofficial
    u32 checkFootLine(sBcSensorLine *, ulong); ///< @unofficial
    u32 _checkFoot(float x, float y, ulong sensorFlags);
    u32 _checkFootBlock(float x, float y, ulong sensorFlags);
    int _checkFootSakaType(float x, float y, ulong *type, ulong *kind);
    u32 _checkFootSaka(ulong sensorFlags);
    u32 _checkSakaFly(ulong subkind, ulong sensorFlags);

    u32 checkHead(unsigned long);
    u32 checkHeadPoint(sBcSensorPoint *, ulong); ///< @unofficial
    u32 checkHeadLine(sBcSensorLine *, ulong); ///< @unofficial
    u32 _checkHead(float x, float y, ulong sensorFlags);
    u32 _checkHeadBlock(float x, float y, ulong sensorFlags);
    void _checkHeadRev(ulong, float y, ulong sensorFlags);
    u32 _checkHeadSaka(ulong sensorFlags);
    u32 _checkHeadAttr(ulong);

    u32 checkWall(float *);
    u32 checkWallEnm(float *);
    u32 checkWallCommon(int, int);
    u32 checkWallPoint(sBcSensorPoint *, int dir, ulong); ///< @unofficial
    u32 checkWallLine(sBcSensorLine *, int dir, ulong); ///< @unofficial
    u32 _checkCancelWall(float x, float y);
    u32 _checkWall(float x, float y, ulong sensorFlags, int dir);

    u16 getWallAttr(int);
    u16 getFootAttr();

    u32 checkPoleCollision(sBcSensorLine *); ///< @unofficial
    u32 checkVineCollision(sBcSensorLine *); ///< @unofficial

    bool hasSensorFoot() { return mpSensorFoot != nullptr; }
    bool hasSensorHead() { return mpSensorHead != nullptr; }
    bool hasSensorWall() { return mpSensorWall != nullptr; }

    bool checkRoofPlayer(const mVec3_c *, float *);
    u32 getSakaDir();
    int checkDokanLR(mVec3_c *, u8, int *, float, float);
    int checkDokanDown(mVec3_c *, int *);
    int checkDokanUp(mVec3_c *, int *);
    void setRideOnObjBg(dBg_ctr_c *, const mVec3_c &);
    bool checkWallPlayer(const mVec3_c *, const mVec3_c *, float *);
    s8 getPlrNo(float y);
    u32 checkBgPlr(dActor_c *);
    u16 getHeadAttr();
    short getHeadSakaMoveAngle(u8 direction);
    void clearBgcSaveAll();
    bool setWakuCoinAppear(ulong p1, ulong p2, ulong p3, mVec3_c *pos);
    bool _checkFootAttr(ulong);
    bool _checkFootAttr2(ulong);

    bool getSakaUpDown(u8 direction);
    bool getSakaUpDown(u8 direction, ulong subkind);
    short getSakaAngleBySpeed(float);
    int getSakaType();

    void _attrNormal();
    void _attrNuma();
    void _attrLeft();
    void _attrRight();

    static bool _isDifferentPos(float x, float y);
    static int checkWaterDepth(float, float, u8, u8, float *);
    static WATER_TYPE_e checkWater(float, float, u8, float *);
    static bool checkManyPoint(float *, float);
    static u32 checkBg(float, float, u8, u8, unsigned long);
    static u32 checkWireNet(float x, float y, unsigned char layer);
    static u32 checkGround(const mVec3_c *, float *, u8, u8, s8);
    static u32 checkGround(const mVec3_c *, float *, int *, u8, u8, s8);
    static bool checkGroundAngle(const mVec3_c *, float *, s16 *, u8, u8, s8, int *, int);
    static u32 checkGroundHalf(const mVec3_c *, float *, u8, u8);
    static u32 checkTenjou(const mVec3_c *, float *, u8, u8);
    static u32 checkWall(const mVec3_c *, const mVec3_c *, float *, u8, u8, dActor_c **);
    static int checkHitMameBlock(float x, float y, u8 layer);
    static u32 getUnitType(float x, float y, u8 layer);
    static u32 getUnitKind(float x, float y, u8 layer);
    static u32 _getUnitType(u16 x, u16 y, u8 layer);
    static u32 _getUnitKind(u16 x, u16 y, u8 layer);
    static void getAirWaterHitPos(mVec2_c *);
    static void getAirWaterHitAngle(short *);

    u32 getFlags() const { return mFlags; }

    u32 isWallR() { return mFlags & FLAG_WALL_R; }
    u32 isWallL()  { return mFlags & FLAG_WALL_L; }
    u32 isWall() { return mFlags & (FLAG_WALL_R | FLAG_WALL_L); }
    u32 isWall(u8 dir) { return mFlags & (FLAG_WALL_R << dir); }
    u32 isFoot(); // { return mFlags & FLAG_FOOT; }
    u32 isHead() { return mFlags & FLAG_HEAD; }
    u32 isCollision() { return mFlags & (FLAG_WALL_L | FLAG_WALL_R | FLAG_FOOT | FLAG_HEAD); }

    static s16 toTile(float v) { return (s16) v & 0xfff0; }
    static u16 toTileU(float v) { return (u16) v & 0xfff0; }
    static int toTileInt(float v) { return (int) v & 0xfff0; }
    static int toTileInt2(float v) { return (int) v & ~0xf; }
    static int roundTile(int v) { return v & 0xfff0; }
    static mVec3_c toTile(const mVec3_c &p) { return mVec3_c(toTile(p.x), toTile(p.y), 0.0f); }

    dActor_c *mpOwner;
    sBcSensorIf_c *mpSensorFoot;
    sBcSensorIf_c *mpSensorHead;
    sBcSensorIf_c *mpSensorWall;
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
    dBc_c *mPrevTrigBelowSensor;
    dBc_c *mPrevTrigAboveSensor;
    dBc_c *mPrevTrigAdjSensor;
    dBc_c *mPrevTrigAdjSensorForDirection[2];
    dBg_ctr_c *mLinkW[2];
    u32 mFlags;
    u32 mPrevFlags;
    u32 mLastUnitType;
    u32 mLastUnitKind;
    s8 mOwningPlrNo;
    s8 mRidePlrNo;
    u16 mLastX;
    u16 mLastY;

    u32 mFootAttr;
    u8 mFootUnitType;
    u8 mFootPrevUnitType;

    u32 mHeadAttr;
    u8 mHeadUnitType;
    u8 mHeadPrevUnitType;

    u32 mWallAttr[2];
    u8 mWallUnitType[2];
    u8 mWallPrevUnitType[2];

    u8 mPlayerFlags;

    u8 mSlopeType;
    s16 mSlopeAngle;
    u8 mFlippedSlopeType;
    s16 mFlippedSlopeAngle;

    int m_c4;
    mAng mAdjacentSlopeAngle;
    int mFenceType;
    dBg_ctr_c *mpFenceCollision;
    float mFenceY;
    float mIceSpeedParam;
    float mIceSpeed;
    u8 m_e0;
    s8 m_e1;
    u8 m_e2;
    u8 mDirection;
    bool mGrounded;
    u8 mAmiLine;
    char mPad9[0x2];
    u8 *mpLayer;
    u8 mLayer;

    static u16 gUnitX;
    static u16 gUnitY;
    static Flag_e (dBc_c::*checkObjFoot)(ulong flags, float);
    static Flag_e (dBc_c::*checkObjHead)(ulong flags, float);
    static Flag_e (dBc_c::*checkObjWall)(ulong flags, int dir, float);
};
