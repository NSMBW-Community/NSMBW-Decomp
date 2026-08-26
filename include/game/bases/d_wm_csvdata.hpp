#pragma once

#include <types.h>
#include <game/mLib/m_3d.hpp>

#define MAX_POINT_COUNT (192)
#define MAX_SUBROUTE_COUNT (160)
#define MAX_ROUTE_COUNT (64)

#define MAX_POINT_NAME_LEN (5)
#define MAX_ROUTE_NAME_LEN (10)
#define MAX_FLAG_DATA_LEN (10)

#define MAX_OPEN_POINT_COUNT (4)
#define MAX_OPEN_ROUTE_COUNT (16)
#define MAX_FLAG_DATA_COUNT (2)

#define SUBWORLD_CHAR_OFFSET (10) // The position of 'a' in "Wx/pointWxa.csv"

/// @brief Loader for world map point / route data.
/// @ingroup bases
class dCsvData_c {
public:
    /// @unofficial
    enum PointFlag_e {
        POINT_URA = BIT_FLAG(0),
        POINT_STOP = BIT_FLAG(1),
        POINT_LINK1 = BIT_FLAG(2),
        POINT_LINK2 = BIT_FLAG(3),
        POINT_LINK3 = BIT_FLAG(4),
        POINT_LINK4 = BIT_FLAG(5),
        POINT_LINK5 = BIT_FLAG(6),
        POINT_SCROLL = BIT_FLAG(7),
        POINT_SCALE = BIT_FLAG(8),
        POINT_DOKAN = BIT_FLAG(10),
        POINT_SWITCH = BIT_FLAG(11),
        POINT_ANCHOR_X = BIT_FLAG(12),
        POINT_ANCHOR_Y = BIT_FLAG(13),
        POINT_TILT = BIT_FLAG(14),
        POINT_DEMO1 = BIT_FLAG(15),
        POINT_DEMO2 = BIT_FLAG(16),
        POINT_DEMO3 = BIT_FLAG(17),
        POINT_CROSSROAD = BIT_FLAG(18),
        POINT_FOCUS = BIT_FLAG(19),
        POINT_BOARD = BIT_FLAG(20),
        POINT_DEMOSTOP = BIT_FLAG(21),
        POINT_SCROLL_Y = BIT_FLAG(22),
        POINT_SAND = BIT_FLAG(23),
        POINT_ICE = BIT_FLAG(24),
        POINT_DEMO4 = BIT_FLAG(25),
        POINT_DEMO5 = BIT_FLAG(26),
        POINT_DEMO6 = BIT_FLAG(27),
        POINT_DEMO7 = BIT_FLAG(28),
        POINT_CAMSTOP = BIT_FLAG(29),
        POINT_NOSHIFT = BIT_FLAG(30),
        POINT_SCROLL_A = BIT_FLAG(31)
    };

    /// @unofficial
    enum EnemyFlag_e {
        ENEMY_KURIBO1 = BIT_FLAG(1),
        ENEMY_KURIBO2 = BIT_FLAG(2),
        ENEMY_PUKU1 = BIT_FLAG(3),
        ENEMY_PUKU2 = BIT_FLAG(4),
        ENEMY_PAK1 = BIT_FLAG(5),
        ENEMY_PAK2 = BIT_FLAG(6),
        ENEMY_PAK3 = BIT_FLAG(7),
        ENEMY_HBROS1 = BIT_FLAG(8),
        ENEMY_TRAP1 = BIT_FLAG(10),
        ENEMY_TRAP2 = BIT_FLAG(11)
    };

    /// @unofficial
    enum ActionType_e {
        ACTION_TYPE_NONE = -1,
        ACTION_TYPE_0, ///< @unused
        ACTION_TYPE_1, ///< @unused
        ACTION_TYPE_ROAD,
        ACTION_TYPE_LADDER,
        ACTION_TYPE_JUMP,
        ACTION_TYPE_5, ///< @unused
        ACTION_TYPE_VINE,
        ACTION_TYPE_SLOPE,
        ACTION_TYPE_ICE_SLOPE,
        ACTION_TYPE_SWITCH_BLOCK = 14,
        ACTION_TYPE_SAND,
        ACTION_TYPE_QUICKSAND,
        ACTION_TYPE_SNOW,
        ACTION_TYPE_ICE,
        ACTION_TYPE_CLOUD,
        ACTION_TYPE_WATER,
        ACTION_TYPE_LEFT_LADDER,
        ACTION_TYPE_RIGHT_LADDER,
        ACTION_TYPE_DIRT,
        ACTION_TYPE_ROCK_LADDER,
        ACTION_TYPE_ROPE_LADDER,
        ACTION_TYPE_WOOD
    };

    /// @brief Contains the data for a single point on the world map.
    struct Point_s {
        char mName[MAX_POINT_NAME_LEN]; ///< The name of the point.
        char mOpenPointNameRegular[MAX_OPEN_POINT_COUNT][MAX_POINT_NAME_LEN]; ///< The points to be opened from a regular exit.
        char mRouteNameRegular[MAX_OPEN_ROUTE_COUNT][MAX_ROUTE_NAME_LEN]; ///< The routes to be opened from a regular exit.
        char mOpenPointNameSecret[MAX_OPEN_POINT_COUNT][MAX_POINT_NAME_LEN]; ///< The points to be opened from a secret exit.
        char mRouteNameSecret[MAX_OPEN_ROUTE_COUNT][MAX_ROUTE_NAME_LEN]; ///< The routes to be opened from a secret exit.
        int mOpenPointNumRegular; ///< The number of points in mOpenPointNameRegular.
        int mOpenRouteNumRegular; ///< The number of routes in mRouteNameRegular.
        int mOpenRouteNumSecret; ///< The number of routes in mRouteNameSecret.
        int mOpenPointNumSecret; ///< The number of points in mOpenPointNameSecret.
        char mFlagDataRegular[MAX_FLAG_DATA_COUNT][MAX_FLAG_DATA_LEN]; ///< The point flag data for a regular exit.
        char mFlagDataSecret[MAX_FLAG_DATA_COUNT][MAX_FLAG_DATA_LEN]; ///< The point flag data for a secret exit.
        int mFlagDataNumRegular; ///< The number of entries in mFlagDataRegular.
        int mFlagDataNumSecret; ///< The number of entries in mFlagDataSecret.
        u32 mFlags; ///< Is a bitfield of PointFlag_e values.
        u8 mParam; ///< A parameter value for the point. [Seems to only be used for "anchor[0-9]" / "anchor[xy][0-9]" points].
        u32 mFlagsEnemy; ///< Is a bitfield of EnemyFlag_e values.
        u8 m_1bc;
    };

    struct SubRoute_s {
        char mName[MAX_ROUTE_NAME_LEN];
        ActionType_e mActionType;
        int m_10;
        u8 mFlags;
    };

    struct Route_s {
        char mName[MAX_ROUTE_NAME_LEN];
        char (*mChildPoints)[MAX_POINT_NAME_LEN];
        int mPointNum;
        int mLevel;
    };

    virtual ~dCsvData_c();

    /// @brief Initializes the class and loads the CSV data for the specified world and subworld.
    void initialize(int world, int subworld);

    void RouteInfoInit(); ///< Resets all currently loaded point and route data.
    void ReadCsvData(); ///< Loads the CSV point and route data for the specified world and subworld.

    void ReadPointName(char *csv, int &pos); ///< Reads a point name from a CSV entry.
    void ReadPointType(char *csv, int &pos); ///< Reads a point type from a CSV entry.
    void ReadOpenPointName(char *csv, int &pos, bool regularExit); ///< Reads an open point name from a CSV entry.
    void ReadOpenRouteName(char *csv, int &pos, bool regularExit); ///< Reads an open route name from a CSV entry.
    void ReadFlagData(char *csv, int &pos, bool regularExit); ///< Reads the point flag data from a CSV entry.

    void ReadAnimeRouteName(char *csv, int &pos); ///< Reads a route name from a CSV entry.
    void ReadAction(char *csv, int &pos); ///< Reads a route action type from a CSV entry.
    void ReadRouteFlag(char *csv, int &pos); ///< Reads the route flag data from a CSV entry.

    ActionType_e GetActionLabel(const char *route); ///< Gets the action type for a given route name.
    int GetIndexFromPointName(const char *pointName); ///< Gets the index of a point.

    void addKeyPoint(const m3d::mdl_c &model); ///< Adds a key point from a model.
    void SetRouteInfo(const m3d::mdl_c &model); ///< Creates a route from a model and adds it to the route list.

    const char *GetPointName(int idx) const;
    const char *GetOpenPointName(bool regularExit, int idx, int openPointIdx) const;
    int GetOpenPointNum(bool regularExit, int idx) const;
    const char *GetOpenRouteName(bool regularExit, int idx, int openRouteIdx) const;
    int GetOpenRouteNum(bool regularExit, int idx) const;
    u32 GetPointFlags(int idx, PointFlag_e type) const; ///< @unofficial
    u32 GetEnemyPointFlags(int idx, EnemyFlag_e type) const; ///< @unofficial
    u32 GetPointParam(int idx);
    const char *GetRouteName(int idx);
    const char *GetChildPointName(int idx, int childIdx);
    const char *GetSubRouteName(int idx);
    int GetSubRouteIdx(const char *pointA, const char *pointB);
    int GetPointNum(int idx);
    u32 GetSubRouteFlag(int idx);
    int GetRouteAnimNum(bool regularExit, int idx);
    const char *GetRouteAnimName(bool regularExit, int idx, int animIdx);

    bool appendChildFromSubRoute(const char *, const char *, Route_s *, int, bool);
    bool fn_800f5d20(const char *, const char *, Route_s *, int, bool);
    void appendChildFromModel(const nw4r::g3d::ResNode &node, int);
    void adjustChildInfo(Route_s *route);

    bool isLineEnd(char *csv, int pos);

    static void initRouteStruct(Route_s *route, int childCount);
    static void destroyRouteStruct(Route_s *route);

private:
    int mWorld; ///< The world number for which to load the point and route data.
    int mSubworld; ///< The subworld number for which to load the point and route data.
    int mPointCount; ///< The number of loaded points in mPoints.
    int mSubrouteCount; ///< The number of loaded subroutes in mSubRoutes.
    int mRouteCount; ///< The number of loaded routes in mRoutes.
    Point_s mPoints[MAX_POINT_COUNT]; ///< The loaded point data.
    SubRoute_s mSubRoutes[MAX_SUBROUTE_COUNT]; ///< The loaded subroute data.
    Route_s mRoutes[MAX_ROUTE_COUNT]; ///< The loaded route data.

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
