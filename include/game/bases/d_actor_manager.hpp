#pragma once

#include <game/bases/d_actor.hpp>
#include <game/bases/d_cd.hpp>
#include <game/mLib/m_vec.hpp>
#include <constants/game_constants.h>

class dActorMng_c {
public:
    void createUpCoin(const mVec3_c &pos, u8 dir, u8 count, u8 layer);

    static dActorMng_c *m_instance;
};

/// @unofficial
enum ACTOR_CREATE_INFO_FLAG_e {
    ACTOR_CREATE_ALWAYS_SPAWN = BIT_FLAG(1),
    ACTOR_CREATE_MAPOBJ = BIT_FLAG(3),
    ACTOR_CREATE_GROUPED = BIT_FLAG(4)
};

/// @unofficial
struct sActorCreateInfo {
    u16 mProfileName;
    int mSpawnOffsetX;
    int mSpawnOffsetY;
    int mSpawnRangeOffsetX;
    int mSpawnRangeOffsetY;
    int mSpawnRangeWidth;
    int mSpawnRangeHeight;
    u16 mSpawnMarginTop;
    u16 mSpawnMarginBottom;
    u16 mSpawnMarginLeft;
    u16 mSpawnMarginRight;
    u16 mFlags; /// See ACTOR_CREATE_INFO_FLAG_e.
};


/// @unofficial
namespace dActorCreate {
    extern sActorCreateInfo sActorCreates[];

    inline sActorCreateInfo *getActorCreateInfo(int idx) { return &sActorCreates[idx]; }
}

/// @unofficial
struct sSomeStruct {
    int m_00, m_04;
    int mTileLeft, mTileRight, mTileTop, mTileBottom;
    int m_18, m_1c;
};

class dActorCreateMng_c {
public:
    void ActorCreateInfoClear();
    bool ScroolAreaInCheck(int a, int b, int c, int d, int e, int f);
    bool ScroolAreaInLoopCheck(int a, int b, int c, int d, int e, int f);
    void setMapActorCreate();
    void setMapActorCreate_next();
    void MapActorInital_set();
    void MapActorInital_next();
    /// @unofficial
    bool GlobalActorCheck(sActorCreateData *data);
    /// @unofficial
    void processGroupId(sActorCreateData *data, u8 file, u32 idx);
    bool GroupIdCheck(ulong param);
    /// @unofficial
    void create(sActorCreateData *data, u16 *deleteNum, u8 *spawnFlags, int count, bool b);
    void MapActorScroolCreateCheck();
    /// @unofficial
    void MapActorScrollCreate(sSomeStruct *s, int);

    void incZposCount();
    float addZposCount();
    void incZposCount_layer2();
    float addZposCount_layer2();
    void incMapObjZposCount();
    float addMapObjZposCount();
    void incMapObjZposCount_layer2();
    float addMapObjZposCount_layer2();

    /// @unofficial
    dActor_c *construct(sActorCreateData *data, sActorCreateInfo *info, u8 *spawnFlags, u16 *deleteNum, u8 areaNo);

    int mZPosCount; ///< The Z position index for the next actor.
    int mZPosCountLayer2; ///< The Z position index for the next actor on layer 2.
    int mMapObjZPosCount;
    int mMapObjZPosCountLayer2;
    u16 mDeleteNums[MAX_ACTOR_COUNT]; ///< Number that can be written to by the actor when deleting.
    u8 mSpawnFlags[MAX_ACTOR_COUNT]; ///< Flags indicating actor spawn status. See dActor_c::ACTOR_SPAWN_FLAG_e.
    u16 m_bc8;
    bool m_bca;
    u8 m_bcb;
    int mPosX, mPosY, mTileX, mTileY;

    static dActorCreateMng_c *m_instance;
};
