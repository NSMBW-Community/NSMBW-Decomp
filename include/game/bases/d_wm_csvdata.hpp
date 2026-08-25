#pragma once

#include <types.h>
#include <game/mLib/m_3d.hpp>

#define POINT_NAME_MAX_LEN (5)
#define ROUTE_NAME_MAX_LEN (10)

class dCsvData_c {
public:
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
        int mChildPoints;
        int mPointNum;
        int m_14;
    };

    virtual ~dCsvData_c();

    void initialize(int a, int b);

    const char *GetPointName(int) const;
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

    bool isLineEnd(char *csv, int pos);

    static void destroyRouteStruct(Route_s *route);

    int mWorld;
    int m_08;
    int mPointCount;
    int mRouteCount;
    int m_14;
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
