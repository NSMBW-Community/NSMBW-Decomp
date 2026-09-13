#include <game/bases/d_wm_connect.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <string.h>
#include <math.h>

void dWmConnect_c::dPnt_c::SetInfo(const char *pointName, nw4r::math::VEC3 &pointPos, int index) {
    strncpy(name, pointName, 5);
    pos.x = pointPos.x;
    pos.y = pointPos.y;
    pos.z = pointPos.z;
    mPointIndex = index;
}

void dWmConnect_c::dPnt_c::init() {
    name[0] = 0;
    name[1] = 0;
    name[2] = 0;
    name[3] = 0;
    name[4] = 0;

    for (int i = 0; i < 6; i++) {
        mLink[i] = nullptr;
        mActionLabel[i] = dCsvData_c::ACTION_TYPE_NONE;
    }

    mCourseNo = -1;
    mPointIndex = -1;
}

void dWmConnect_c::SetInfo(dCsvData_c &csvData, const m3d::mdl_c &model) {
    nw4r::g3d::ResMdl resMdl = model.getResMdl();

    for (int i = 0; i < csvData.mPointCount; i++) {
        nw4r::math::VEC3 pos;
        int nodeId = m3d::getNodeID(resMdl, csvData.GetPointName(i));
        model.getNodeWorldMtxMultVecZero(nodeId, pos);

        const char *pointName = csvData.GetPointName(i);
        mPoints[i].SetInfo(pointName, pos, i);

        if (pointName[0] == 'K') {
            mPoints[i].mIsValid = 1;
        } else if (dWmLib::GetPointTypeFromPointName(pointName) == dWmLib::POINT_TYPE_START_NODE
                || dWmLib::GetCourseNoFromPointName(pointName) >= 0
                || csvData.GetPointFlags(i, dCsvData_c::POINT_STOP)
                || csvData.GetPointFlags(i, dCsvData_c::POINT_DOKAN)
                || csvData.GetEnemyPointFlags(i, dCsvData_c::ENEMY_TRAP1)
                || csvData.GetEnemyPointFlags(i, dCsvData_c::ENEMY_TRAP2)
                || csvData.GetPointFlags(i, dCsvData_c::POINT_SWITCH)) {
            mPoints[i].mIsValid = 0;
        } else {
            mPoints[i].mIsValid = 1;
        }
    }
}

void dWmConnect_c::initialize(dCsvData_c &csvData, const m3d::mdl_c &model) {
    mWorld = csvData.mWorld;

    for (int i = 0; i < MAX_POINT_COUNT; i++) {
        mPoints[i].init();
    }

    SetInfo(csvData, model);

    for (int i = 0; i < MAX_POINT_COUNT; i++) {
        SetCourseNo(i);
    }

    calcConnect(csvData);
    initAnimation(csvData);
}

void dWmConnect_c::calcConnect(dCsvData_c &csvData) {
    for (int i = 0; i < csvData.mSubrouteCount; i++) {
        char startPointName[8];
        char endPointName[8];
        dWmLib::GetStartPointNameFromRouteName(csvData.GetSubRouteName(i), startPointName);
        dWmLib::GetEndPointNameFromRouteName(csvData.GetSubRouteName(i), endPointName);

        if (startPointName[0]) {
            __connect(GetPointNo(startPointName), GetPointNo(endPointName));
        }
    }
}

/// @unofficial
static inline dWmConnect_c::dPnt_c *getPnt(dWmConnect_c::dPnt_c *points, int index) {
    return &points[index];
}

/// @unofficial
static inline char *getName(dWmConnect_c::dPnt_c *point) {
    return point->name;
}

int dWmConnect_c::GetPointNo(const char *name) {
    dPnt_c *p = mPoints;
    for (int i = 0; i < MAX_POINT_COUNT; i++, p++) {
        if (strcmp(p->name, name) == 0) {
            return i;
        }
    }

    return -1;
}

bool dWmConnect_c::__connect(int a, int b) {
    dPnt_c *pointB = getPnt(mPoints, b);
    dPnt_c *pointA = getPnt(mPoints, a);

    float dx = pointA->pos.x - pointB->pos.x;
    float dy = pointA->pos.y - pointB->pos.y;
    float dz = pointA->pos.z - pointB->pos.z;

    bool xSmall = (float)std::fabs(dx) < 3.0f;
    bool zSmall = (float)std::fabs(dz) < 3.0f;

    if (xSmall && zSmall) {
        if (dy > 0.0f) {
            mPoints[a].mLink[1] = pointB;
            mPoints[b].mLink[0] = pointA;
        } else {
            mPoints[b].mLink[1] = pointA;
            mPoints[a].mLink[0] = pointB;
        }
    } else if (xSmall) {
        if (dz > 0.0f) {
            mPoints[a].mLink[3] = pointB;
            mPoints[b].mLink[2] = pointA;
        } else {
            mPoints[b].mLink[3] = pointA;
            mPoints[a].mLink[2] = pointB;
        }
    } else if (zSmall) {
        if (dx > 0.0f) {
            mPoints[a].mLink[4] = pointB;
            mPoints[b].mLink[5] = pointA;
        } else {
            mPoints[b].mLink[4] = pointA;
            mPoints[a].mLink[5] = pointB;
        }
    } else if ((float)std::fabs(dz) >= 3.0f) {
        if (dz > 0.0f) {
            mPoints[a].mLink[3] = pointB;
            mPoints[b].mLink[2] = pointA;
        } else {
            mPoints[b].mLink[3] = pointA;
            mPoints[a].mLink[2] = pointB;
        }
    } else {
        if (dx > 0.0f) {
            mPoints[a].mLink[4] = pointB;
            mPoints[b].mLink[5] = pointA;
        } else {
            mPoints[b].mLink[4] = pointA;
            mPoints[a].mLink[5] = pointB;
        }
    }

    return true;
}

void dWmConnect_c::initAnimation(dCsvData_c &csvData) {
    char *ownName;
    char *neighborName;
    for (int i = 0; i < MAX_POINT_COUNT; i++) {
        for (int dir = 0; dir < 6; dir++) {
            dPnt_c *link = getPnt(mPoints, i)->GetLink(dir);
            if (link != nullptr) {
                ownName = getName(getPnt(mPoints, i));
                neighborName = getName(link);
                char routeName[32];
                dWmLib::MakeRouteName(routeName, ownName, neighborName);
                int label = csvData.GetActionLabel(routeName);
                if (label == -1) {
                    dWmLib::MakeRouteName(routeName, neighborName, ownName);
                    label = csvData.GetActionLabel(routeName);
                }

                mPoints[i].mActionLabel[dir] = label;
            }
        }
    }
}

void dWmConnect_c::SetCourseNo(int index) {
    mPoints[index].mCourseNo = dWmLib::GetCourseNoFromPointName(mPoints[index].name);
}

int dWmConnect_c::GetCourseNoFromName(const char *name) {
    dPnt_c *p = mPoints;
    for (int i = 0; i < MAX_POINT_COUNT; i++, p++) {
        if (strcmp(p->name, name) == 0) {
            return mPoints[i].mCourseNo;
        }
    }

    return -1;
}

dWmConnect_c::dPnt_c *dWmConnect_c::GetConnect(const char *name, int dir) {
    dPnt_c *p = mPoints;
    for (int i = 0; i < MAX_POINT_COUNT; i++, p++) {
        if (strcmp(p->name, name) == 0) {
            return mPoints[i].GetLink(dir);
        }
    }

    return nullptr;
}

dWmConnect_c::dPnt_c *dWmConnect_c::GetPointFromIndex(int index) {
    for (int i = 0; i < MAX_POINT_COUNT; i++) {
        if (mPoints[i].mPointIndex == index) {
            return &mPoints[i];
        }
    }

    return nullptr;
}

int dWmConnect_c::GetDir(int indexA, int indexB) {
    dPnt_c *pointA = GetPointFromIndex(indexA);
    dPnt_c *pointB = GetPointFromIndex(indexB);

    for (int dir = 0; dir < 6; dir++) {
        dPnt_c *link = pointA->GetLink(dir);
        if (link != nullptr && link == pointB) {
            return dir;
        }
    }

    return 6;
}

dWmConnect_c::dPnt_c *dWmConnect_c::dPnt_c::GetLink(int dir) {
    return mLink[dir];
}
