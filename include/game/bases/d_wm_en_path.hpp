#pragma once
#include <game/bases/d_wm_connect.hpp>
#include <game/mLib/m_vec.hpp>
#include <constants/game_constants.h>

/// @unofficial
struct dWmPathPoint_s {
    const char *mpName;
    int mIndex;
    int mPointIndex;
    dWmPathPoint_s *mpNext;
    dWmPathPoint_s *mpPrev;
};

class dWmEnPath_c {
public:
    dWmEnPath_c();
    virtual ~dWmEnPath_c();

    /// @unofficial
    bool init(const char **names, int count, dWmConnect_c *connect, bool cyclic, PATH_DIRECTION_e dir);

    void SetStartPoint(int index);
    int GetNextPointIdx();
    int GetPathPointNo(const char *name);
    dWmPathPoint_s *GetNextPointInfo(bool updateDirection);
    void UpdatePoint();
    bool isPointIn(int pointIndex);

    dWmPathPoint_s mPoints[WM_ENEMY_PATH_MAX_POINT_COUNT];
    dWmPathPoint_s *mpCurrentPoint;
    PATH_DIRECTION_e mDir1;
    PATH_DIRECTION_e mDir2;
    bool mAdvancePoint;
};
