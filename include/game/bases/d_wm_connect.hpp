#pragma once
#include <types.h>
#include <game/mLib/m_vec.hpp>
#include <game/bases/d_wm_csvdata.hpp>
#include <game/mLib/m_3d/mdl.hpp>

class dWmConnect_c {
public:
    /// @unofficial
    class dPnt_c {
    public:
        void SetInfo(const char *pointName, nw4r::math::VEC3 &pointPos, int index);
        void init();
        dPnt_c *GetLink(int dir);

        u32 m_00; ///< @unofficial Unknown. Never read/written by any decompiled code so far.
        char name[8]; ///< @unofficial Only the first MAX_POINT_NAME_LEN bytes are used.
        nw4r::math::VEC3 pos;
        dPnt_c *mLink[6];
        s32 mActionLabel[6]; ///< @unofficial dCsvData_c::ActionType_e per link direction.
        s32 mCourseNo;
        s32 mPointIndex;
        s32 mIsValid; ///< @unofficial Whether the point is usable for pathing (not a special/hidden node).
    };

    void SetInfo(dCsvData_c &csvData, const m3d::mdl_c &model);
    void initialize(dCsvData_c &csvData, const m3d::mdl_c &model);
    void calcConnect(dCsvData_c &csvData);
    int GetPointNo(const char *name);
    bool __connect(int a, int b); ///< @unofficial
    void initAnimation(dCsvData_c &csvData);
    void SetCourseNo(int index);
    int GetCourseNoFromName(const char *name);
    dPnt_c *GetConnect(const char *name, int dir);
    dPnt_c *GetPointFromIndex(int index);
    int GetDir(int indexA, int indexB);

    u32 m_00; ///< @unofficial Unknown. Never read/written by any decompiled code so far.
    dPnt_c mPoints[MAX_POINT_COUNT];
    s32 mWorld; ///< @unofficial Copied from dCsvData_c::mWorld during initialize().
};
