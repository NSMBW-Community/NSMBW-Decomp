#pragma once

#include <types.h>
#include <game/mLib/m_3d.hpp>
/// @file

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
        char mOpenPointNameRegular[MAX_OPEN_POINT_COUNT][MAX_POINT_NAME_LEN]; ///< The points to be opened by the regular exit.
        char mRouteNameRegular[MAX_OPEN_ROUTE_COUNT][MAX_ROUTE_NAME_LEN]; ///< The routes to be opened by the regular exit.
        char mOpenPointNameSecret[MAX_OPEN_POINT_COUNT][MAX_POINT_NAME_LEN]; ///< The points to be opened by the secret exit.
        char mRouteNameSecret[MAX_OPEN_ROUTE_COUNT][MAX_ROUTE_NAME_LEN]; ///< The routes to be opened by the secret exit.
        int mOpenPointNumRegular; ///< The number of points in mOpenPointNameRegular.
        int mOpenRouteNumRegular; ///< The number of routes in mRouteNameRegular.
        int mOpenRouteNumSecret; ///< The number of routes in mRouteNameSecret.
        int mOpenPointNumSecret; ///< The number of points in mOpenPointNameSecret.
        char mFlagDataRegular[MAX_FLAG_DATA_COUNT][MAX_FLAG_DATA_LEN]; ///< The point flag data for the regular exit.
        char mFlagDataSecret[MAX_FLAG_DATA_COUNT][MAX_FLAG_DATA_LEN]; ///< The point flag data for the secret exit.
        int mFlagDataNumRegular; ///< The number of entries in mFlagDataRegular.
        int mFlagDataNumSecret; ///< The number of entries in mFlagDataSecret.
        u32 mFlags; ///< A bitfield of PointFlag_e values.
        u8 mParam; ///< A parameter value for the point. @decompnote{Seems to only be used for "anchor[0-9]" / "anchor[xy][0-9]" points}.
        u32 mFlagsEnemy; ///< A bitfield of EnemyFlag_e values.
        u8 m_1bc;
    };

    /// @brief Contains the data for a point-to-point route on the world map.
    struct SubRoute_s {
        char mName[MAX_ROUTE_NAME_LEN]; ///< The name of the route.
        ActionType_e mActionType; ///< The action type of the route.
        int m_10;
        u8 mFlags;
    };

    /// @brief Contains the points of a route on the world map.
    /// @details The difference between this and SubRoute_s is that a route can span multiple subroutes.
    /// E.g. R00010003, consisting of subroutes R00010002 and R00020003 and the points 0001, 0002, and 0003.
    struct Route_s {
        char mName[MAX_ROUTE_NAME_LEN]; ///< The name of the route.
        char (*mChildPoints)[MAX_POINT_NAME_LEN]; ///< The points that make up the route.
        int mChildPointNum; ///< The number of occupied entries in mChildPoints.
        int mNumSubroutes; ///< The number of subroutes this route is made up of.
    };

    virtual ~dCsvData_c(); ///< Destroys the class.

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

    const char *GetPointName(int idx) const; ///< Returns the name of a point at the specified index.
    const char *GetOpenPointName(bool regularExit, int idx, int openPointIdx) const; ///< Returns the name of a point that is opened from a regular or secret exit from this point.
    int GetOpenPointNum(bool regularExit, int idx) const; ///< Returns the number of points that are opened from a regular or secret exit from this point.
    const char *GetOpenRouteName(bool regularExit, int idx, int openRouteIdx) const; ///< Returns the name of a route that is opened from a regular or secret exit from this point.
    int GetOpenRouteNum(bool regularExit, int idx) const; ///< Returns the number of routes that are opened from a regular or secret exit from this point.
    u32 GetPointFlags(int idx, PointFlag_e flags) const; ///< Returns the flags ANDed with @p flags. @unofficial
    u32 GetEnemyPointFlags(int idx, EnemyFlag_e flags) const; ///< Returns the enemy flags ANDed with @p flags. @unofficial
    u32 GetPointParam(int idx); ///< Returns the parameter value for this point.

    const char *GetRouteName(int idx); ///< Returns the name of a route at the specified index.
    const char *GetChildPointName(int idx, int childIdx); ///< Returns the name of a point that is part of a route at the specified index.
    const char *GetSubRouteName(int idx); ///< Returns the name of a subroute at the specified index.
    int GetSubRouteIdx(const char *pointA, const char *pointB); ///< Returns the index of a route connecting two points.
    int GetPointNum(int idx); ///< Returns the number of points that are part of a route at the specified index.
    u32 GetSubRouteFlag(int idx); ///< Returns the flags for a route at the specified index.

    int GetRouteAnimNum(bool regularExit, int idx); ///< Returns the number of flag data entries for a point.
    const char *GetRouteAnimName(bool regularExit, int idx, int animIdx); ///< Returns the value of a flag data entry for a point.

    /// @brief Searches the subroute graph for a path between two points and writes the length of the path to @p route.
    /// @param startPointName The name of the starting point.
    /// @param endPointName The name of the ending point.
    /// @param route The route to write the path length to.
    /// @param maxLevel The maximum depth of the search.
    /// @param resetLevel Whether to reset the search depth counter. Should be true for the initial call and false for recursive calls.
    /// @return True if a path was found, false otherwise.
    /// @unofficial
    bool findSubRoutesForRoute(const char *startPointName, const char *endPointName, Route_s *route, int maxLevel, bool resetLevel = true);

    /// @brief Searches the subroute graph for a path between two points and appends the child points to @p route.
    /// @details This function assumes the length of the path has already been determined with findSubRoutesForRoute.
    /// @param startPointName The name of the starting point.
    /// @param endPointName The name of the ending point.
    /// @param route The route to write the points to.
    /// @param maxLevel The maximum depth of the search.
    /// @param resetLevel Whether to reset the search depth counter. Should be true for the initial call and false for recursive calls.
    /// @return True if everything was successful, false otherwise.
    bool appendChildFromSubRoute(const char *startPointName, const char *endPointName, Route_s *route, int maxLevel, bool resetLevel = true);

    void appendChildFromModel(const nw4r::g3d::ResNode &node, int); ///< Appends the child points of a route from a model to @p route.
    void adjustChildInfo(Route_s *route); ///< Reverses the order of the child points of a route.

    bool isLineEnd(char *csv, int pos); ///< Returns whether the the CSV has a line ending at the specified position.

    /// @brief Initializes a route with the specified number of child points.
    /// @param route The route to initialize.
    /// @param childCount The number of child points to allocate for the route.
    static void initRouteStruct(Route_s *route, int childCount);

    /// @brief Destroys a route and frees its child point data.
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
