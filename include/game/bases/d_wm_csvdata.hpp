#pragma once

#include <types.h>
#include <game/mLib/m_3d.hpp>

#define POINT_NAME_MAX_LEN (5)
#define ROUTE_NAME_MAX_LEN (10)
#define SUBWORLD_CHAR_OFFSET (10) // The position of 'a' in "Wx/pointWxa.csv"

class dCsvData_c {
public:
    enum PointFlag_e {

    };

    enum EnemyFlag_e {

    };

    struct Point_s {
        char mName[POINT_NAME_MAX_LEN];
        char mOpenPointName1[4][POINT_NAME_MAX_LEN];
        char mRouteName1[16][ROUTE_NAME_MAX_LEN];
        char mOpenPointName2[4][POINT_NAME_MAX_LEN];
        char mRouteName2[16][ROUTE_NAME_MAX_LEN];
        int mOpenPointNum1;
        int mOpenRouteNum1;
        int mOpenRouteNum2;
        int mOpenPointNum2;
        char mAnimName1[2][10];
        char mAnimName2[2][10];
        int mAnimNum1;
        int mAnimNum2;
        u32 mFlags;
        u8 mParam;
        u32 mFlagsEnemy;
        u8 m_1bc;
    };

    struct SubRoute_s {
        char mName[10];
        int mActionLabel;
        int m_10;
        u8 mFlags;
    };

    struct Route_s {
        char mName[10];
        char (*mChildPoints)[5];
        int mPointNum;
        int m_14;
    };

    virtual ~dCsvData_c();

    void initialize(int world, int subworld);

    void RouteInfoInit();
    void ReadCsvData();

    void ReadPointName(char *csv, int &pos);
    void ReadPointType(char *csv, int &pos);
    void ReadOpenPointName(char *csv, int &pos, bool);
    void ReadOpenRouteName(char *csv, int &pos, bool);
    void ReadFlagData(char *csv, int &pos, bool);
    void ReadAnimeRouteName(char *csv, int &pos);
    void ReadAction(char *csv, int &pos);
    void ReadRouteFlag(char *csv, int &pos);

    int GetActionLabel(const char *route);
    int GetIndexFromPointName(const char *pointName);

    void addKeyPoint(const m3d::mdl_c &model);
    void SetRouteInfo(const m3d::mdl_c &model);

    const char *GetPointName(int idx) const;
    const char *GetOpenPointName(bool first, int idx, int openPointIdx) const;
    int GetOpenPointNum(bool first, int idx) const;
    const char *GetOpenRouteName(bool first, int idx, int openRouteIdx) const;
    int GetOpenRouteNum(bool first, int idx) const;
    u32 GetPointFlags(int idx, PointFlag_e type) const; ///< @unofficial
    u32 GetEnemyPointFlags(int idx, EnemyFlag_e type) const; ///< @unofficial
    u32 GetPointParam(int idx);
    const char *GetRouteName(int idx);
    const char *GetChildPointName(int idx, int childIdx);
    const char *GetSubRouteName(int idx);
    int GetSubRouteIdx(const char *pointA, const char *pointB);
    int GetPointNum(int idx);
    u32 GetSubRouteFlag(int idx);
    int GetRouteAnimNum(bool first, int idx);
    const char *GetRouteAnimName(bool first, int idx, int animIdx);

    bool appendChildFromSubRoute(const char *, const char *, Route_s *, int, bool);
    bool fn_800f5d20(const char *, const char *, Route_s *, int, bool);
    void appendChildFromModel(const nw4r::g3d::ResNode &node, int);
    void adjustChildInfo(Route_s *route);

    bool isLineEnd(char *csv, int pos);

    static void initRouteStruct(Route_s *route, int childCount);
    static void destroyRouteStruct(Route_s *route);

    int mWorld;
    int mSubworld;
    int mPointCount;
    int mSubrouteCount;
    int mRouteCount;
    Point_s mPoints[192];
    SubRoute_s mSubRoutes[160];
    Route_s mRoutes[64];

public:
    static const int c_COURSE_ID;
    static const int c_GHOST_ID;
    static const int c_TOWER_ID;
    static const int c_CASTLE_ID;
    static const int c_KINOKO_ID;
    static const int c_ENEMY_ID;
    static const int c_CANON_ID;
    static const int c_TRSHIP_ID;
    static const int c_AIRSHIP_ID;
    static const int c_START_ID;
    static const int c_PEACH_ID;
};
