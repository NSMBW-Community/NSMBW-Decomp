#pragma once
#include <game/bases/d_wm_connect.hpp>
#include <game/mLib/m_vec.hpp>

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
    bool init(const char **names, int count, dWmConnect_c *connect, bool cyclic, int dir);

    void SetStartPoint(int index);
    int GetNextPointIdx();
    int GetPathPointNo(const char *name);
    dWmPathPoint_s *GetNextPointInfo(bool updateDirection);
    void UpdatePoint();
    bool isPointIn(int pointIndex);

    dWmPathPoint_s mPoints[64];
    dWmPathPoint_s *mpCurrentPoint;
    int mDir1;
    int mDir2;
    bool mAdvancePoint;
};
