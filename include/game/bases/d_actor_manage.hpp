#pragma once

#include <types.h>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_cd.hpp>

struct sActorCreateInfo {
    u16 mProfileName;
    u16 m_02;
    int m_04;
    int m_08;
    int m_0c;
    int m_10;
    int m_14;
    int m_18;
    u16 m_1c;
    u16 m_1e;
    u16 m_20;
    u16 m_22;
    u16 m_24; /// &8 = mapobj
};

namespace dActorCreate {
    extern sActorCreateInfo sActorCreates[];

    inline sActorCreateInfo *getActorCreateInfo(int idx) { return &sActorCreates[idx]; }
}

class dActorGroupIdMng_c {
public:
    void operate(sActorCreateData *data, int groupID, u8 area, int idx);

    static dActorGroupIdMng_c *m_instance;
};

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
    bool someCheck(sActorCreateData *data);
    /// @unofficial
    void operate(sActorCreateData *data, u8 area, u32 idx);
    bool GroupIdCheck(ulong param);
    /// @unofficial
    void create(sActorCreateData *data, u16 *data1, u8 *data2, int count, bool b);
    void MapActorScroolCreateCheck();
    /// @unofficial
    void dealWithSomeStruct(sSomeStruct *s, int);

    void incZposCount();
    float addZposCount();
    void incZposCount_layer2();
    float addZposCount_layer2();
    void incMapObjZposCount();
    float addMapObjZposCount();
    void incMapObjZposCount_layer2();
    float addMapObjZposCount_layer2();

    /// @unofficial
    dActor_c *construct(sActorCreateData *data, sActorCreateInfo *info, u8 *data2, u16 *data1, u8 area);

    int mZPosCount;
    int mZPosCountLayer2;
    int mMapObjZPosCount;
    int mMapObjZPosCountLayer2;
    u16 m_10[1000];
    u8 m_7e0[1000];
    u16 m_bc8;
    bool m_bca;
    u8 m_bcb;
    int mPosX, mPosY, mTileX, mTileY;

    static dActorCreateMng_c *m_instance;
};
