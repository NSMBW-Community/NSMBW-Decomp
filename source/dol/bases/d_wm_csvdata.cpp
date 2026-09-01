#include <game/bases/d_wm_csvdata.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <lib/nw4r/g3d.h>
#include <lib/MSL/stdlib.h>
#include <constants/sjis_constants.h>
/// @file

const int dCsvData_c::c_COURSE_ID = STAGE_1;
const int dCsvData_c::c_GHOST_ID = STAGE_GHOST_HOUSE;
const int dCsvData_c::c_TOWER_ID = STAGE_TOWER;
const int dCsvData_c::c_CASTLE_ID = STAGE_CASTLE;
const int dCsvData_c::c_KINOKO_ID = STAGE_KINOKO_HOUSE;
const int dCsvData_c::c_ENEMY_ID = STAGE_ENEMY;
const int dCsvData_c::c_CANON_ID = STAGE_CANNON;
const int dCsvData_c::c_TRSHIP_ID = STAGE_TRSHIP;
const int dCsvData_c::c_AIRSHIP_ID = STAGE_AIRSHIP;
const int dCsvData_c::c_START_ID = STAGE_START_KINOKO_HOUSE;
const int dCsvData_c::c_PEACH_ID = STAGE_PEACH_CASTLE;

const char *l_routeInfoArcName = "RouteInfo";

/// @brief Reads a single CSV field into a buffer.
/// @return The length of the field read.
int read(char *buffer, const char *csv, int bufferSize) {
    int len = 0;
    if (*csv == '"') {
        csv++;
        if (*csv == '"') {
            return 0;
        }
    }

    if (*csv == ',') {
        return 0;
    }

    while (true) {
        if (*csv == ',' || *csv == '"' || *csv == '\r' && *(csv + 1) == '\n') {
            buffer[len] = '\0';
            break;
        }
        buffer[len] = *csv;
        len++;
        csv++;
    }
    return len;
}

dCsvData_c::~dCsvData_c() {
    for (int i = 0; i < MAX_ROUTE_COUNT; i++) {
        if (mRoutes[i].mChildPoints != nullptr) {
            destroyRouteStruct(&mRoutes[i]);
        }
    }
}

void dCsvData_c::initialize(int world, int subworld) {
    mWorld = world;
    mSubworld = subworld;
    RouteInfoInit();
    ReadCsvData();
}

void dCsvData_c::RouteInfoInit() {
    mPointCount = 0;
    mSubrouteCount = 0;
    mRouteCount = 0;
    for (int i = 0; i < MAX_POINT_COUNT; i++) {
        for (int j = 0; j < MAX_POINT_NAME_LEN; j++) {
            mPoints[i].mName[j] = '\0';
        }
        for (int k = 0; k < MAX_OPEN_POINT_COUNT; k++) {
            for (int j = 0; j < MAX_POINT_NAME_LEN; j++) {
                mPoints[i].mOpenPointNameRegular[k][j] = '\0';
                mPoints[i].mOpenPointNameSecret[k][j] = '\0';
            }
        }
        // [Bug: This should be MAX_OPEN_ROUTE_COUNT (16)]
        for (int k = 0; k < 64; k++) {
            for (int j = 0; j < MAX_ROUTE_NAME_LEN; j++) {
                mPoints[i].mRouteNameRegular[k][j] = '\0';
                mPoints[i].mRouteNameSecret[k][j] = '\0';
            }
        }
        for (int k = 0; k < MAX_FLAG_DATA_COUNT; k++) {
            for (int j = 0; j < MAX_FLAG_DATA_LEN; j++) {
                mPoints[i].mFlagDataRegular[k][j] = '\0';
                mPoints[i].mFlagDataSecret[k][j] = '\0';
            }
        }
        mPoints[i].mFlagDataNumRegular = 0;
        mPoints[i].mFlagDataNumSecret = 0;
        mPoints[i].mFlags = 0;
        mPoints[i].mParam = 0;
        mPoints[i].mFlagsEnemy = 0;
        mPoints[i].m_1bc = 0;
    }
    for (int i = 0; i < MAX_SUBROUTE_COUNT; i++) {
        for (int j = 0; j < MAX_ROUTE_NAME_LEN; j++) {
            mSubRoutes[i].mName[j] = '\0';
        }
        mSubRoutes[i].mActionType = ACTION_TYPE_NONE;
        mSubRoutes[i].m_10 = 0;
        mSubRoutes[i].mFlags = 0;
    }
    for (int i = 0; i < MAX_ROUTE_COUNT; i++) {
        for (int j = 0; j < MAX_ROUTE_NAME_LEN; j++) {
            mRoutes[i].mName[j] = '\0';
        }
        mRoutes[i].mChildPoints = nullptr;
        mRoutes[i].mNumSubroutes = 99999;
        mRoutes[i].mChildPointNum = 0;
    }
}

void dCsvData_c::ReadCsvData() {
    dResMng_c *resMng = dResMng_c::m_instance;

    char fileName[0x20];
    size_t fileSize;
    nw4r::g3d::ResFile res;
    char *csv;

    // Load the point CSV file
    snprintf(fileName, sizeof(fileName), "W%X/pointW%X.csv", mWorld + 1, mWorld + 1);
    res = resMng->getResSilently(l_routeInfoArcName, fileName, &fileSize);
    if (!res.IsValid()) {
        snprintf(fileName, sizeof(fileName), "W%X/pointW%Xa.csv", mWorld + 1, mWorld + 1);
        fileName[SUBWORLD_CHAR_OFFSET] += mSubworld;
        res = resMng->getRes(l_routeInfoArcName, fileName, &fileSize);
    }
    csv = (char *) res.ptr();

    int pos = 0;
    do {
        // Skip ahead to the first comma. Each file in the line starts with a line number,
        // which we don't need to process.
        while (csv[pos] != ',') {
            pos++;
        }
        pos++;

        ReadPointName(csv, pos);
        if (isLineEnd(csv, pos)) goto end;
        pos++;
        if (isLineEnd(csv, pos)) goto end;

        ReadPointType(csv, pos);
        if (isLineEnd(csv, pos)) goto end;
        if (isLineEnd(csv, pos)) goto end;

        // Regular exit data

        ReadOpenPointName(csv, pos, true);
        if (isLineEnd(csv, pos)) goto end;
        pos++;
        if (isLineEnd(csv, pos)) goto end;

        ReadOpenRouteName(csv, pos, true);
        if (isLineEnd(csv, pos)) goto end;
        pos++;
        if (isLineEnd(csv, pos)) goto end;

        ReadFlagData(csv, pos, true);
        if (isLineEnd(csv, pos)) goto end;
        if (isLineEnd(csv, pos)) goto end;

        // Secret exit data

        ReadOpenPointName(csv, pos, false);
        if (isLineEnd(csv, pos)) goto end;
        pos++;
        if (isLineEnd(csv, pos)) goto end;

        ReadOpenRouteName(csv, pos, false);
        if (isLineEnd(csv, pos)) goto end;
        pos++;
        if (isLineEnd(csv, pos)) goto end;

        ReadFlagData(csv, pos, false);

    end:
        mPointCount++;
        pos += 2;
    } while (pos != fileSize);

    // Now load the route CSV file
    snprintf(fileName, sizeof(fileName), "W%X/routeW%X.csv", mWorld + 1, mWorld + 1);
    res = resMng->getResSilently(l_routeInfoArcName, fileName, &fileSize);
    if (!res.IsValid()) {
        snprintf(fileName, sizeof(fileName), "W%X/routeW%Xa.csv", mWorld + 1, mWorld + 1);
        fileName[SUBWORLD_CHAR_OFFSET] += mSubworld;
        res = resMng->getRes(l_routeInfoArcName, fileName, &fileSize);
    }
    csv = (char *) res.ptr();

    pos = 0;
    do {
        ReadAnimeRouteName(csv, pos);
        pos++;
        ReadAction(csv, pos);
        pos++;
        ReadRouteFlag(csv, pos);
        if (mSubRoutes[mSubrouteCount].mName[0] != '0') {
            mSubrouteCount++;
        }
        pos += 2;
    } while (pos != fileSize);
}

void dCsvData_c::ReadPointName(char *csv, int &pos) {
    pos += read(mPoints[mPointCount].mName, &csv[pos], sizeof(mPoints[mPointCount].mName));
}

void dCsvData_c::ReadPointType(char *csv, int &pos) {
    GetPointName(mPointCount);
    if (csv[pos] == '"' && csv[pos + 1] == '"') {
        pos += 3;
        return;
    }
    if (csv[pos] == ',') {
        pos++;
        return;
    }

    bool isStr = false;
    if (csv[pos] == '"') {
        isStr = true;
        pos++;
    }

    char pointName[32];
    while (true) {
        int i = 0;
        while (true) {
            if (csv[pos] == ',' || csv[pos] == '"' || isLineEnd(csv, pos)) {
                pointName[i] = '\0';
                break;
            }
            pointName[i] = csv[pos];
            i++;
            pos++;
        }

        // Normal points
        if (strcmp(pointName, "ura") == 0) {
            mPoints[mPointCount].mFlags |= POINT_URA;
        } else if (strcmp(pointName, "stop") == 0) {
            mPoints[mPointCount].mFlags |= POINT_STOP;
        } else if (strcmp(pointName, "link1") == 0) {
            mPoints[mPointCount].mFlags |= POINT_LINK1;
        } else if (strcmp(pointName, "link2") == 0) {
            mPoints[mPointCount].mFlags |= POINT_LINK2;
        } else if (strcmp(pointName, "link3") == 0) {
            mPoints[mPointCount].mFlags |= POINT_LINK3;
        } else if (strcmp(pointName, "link4") == 0) {
            mPoints[mPointCount].mFlags |= POINT_LINK4;
        } else if (strcmp(pointName, "link5") == 0) {
            mPoints[mPointCount].mFlags |= POINT_LINK5;
        } else if (strcmp(pointName, "scroll") == 0) {
            mPoints[mPointCount].mFlags |= POINT_SCROLL;
        } else if (strcmp(pointName, "scale") == 0) {
            mPoints[mPointCount].mFlags |= POINT_SCALE;
        } else if (strcmp(pointName, "dokan") == 0) {
            mPoints[mPointCount].mFlags |= POINT_DOKAN;
        } else if (strcmp(pointName, "switch") == 0) {
            mPoints[mPointCount].mFlags |= POINT_SWITCH;
        } else if (strcmp(pointName, "crossroad") == 0) {
            mPoints[mPointCount].mFlags |= POINT_CROSSROAD;
        } else if (strcmp(pointName, "focus") == 0) {
            mPoints[mPointCount].mFlags |= POINT_FOCUS;
        } else if (strncmp(pointName, "anchor", 6) == 0) {
            int len = strlen(pointName);
            if (len == 6) {
                mPoints[mPointCount].mFlags |= POINT_ANCHOR_X;
                mPoints[mPointCount].mFlags |= POINT_ANCHOR_Y;
            } else if (len == 7 || len == 8) {
                if (pointName[6] == 'x') {
                    mPoints[mPointCount].mFlags |= POINT_ANCHOR_X;
                } else if (pointName[6] == 'y') {
                    mPoints[mPointCount].mFlags |= POINT_ANCHOR_Y;
                } else if ('1' <= pointName[6] && pointName[6] <= '9') {
                    mPoints[mPointCount].mParam = atoi(&pointName[6]);
                }
                if ('1' <= pointName[7] && pointName[7] <= '9') {
                    mPoints[mPointCount].mParam = atoi(&pointName[7]);
                }
            }
        } else if (strcmp(pointName, "tilt") == 0) {
            mPoints[mPointCount].mFlags |= POINT_TILT;
        } else if (strcmp(pointName, "demo1") == 0) {
            mPoints[mPointCount].mFlags |= POINT_DEMO1;
        } else if (strcmp(pointName, "demo2") == 0) {
            mPoints[mPointCount].mFlags |= POINT_DEMO2;
        } else if (strcmp(pointName, "demo3") == 0) {
            mPoints[mPointCount].mFlags |= POINT_DEMO3;
        } else if (strcmp(pointName, "demo4") == 0) {
            mPoints[mPointCount].mFlags |= POINT_DEMO4;
        } else if (strcmp(pointName, "demo5") == 0) {
            mPoints[mPointCount].mFlags |= POINT_DEMO5;
        } else if (strcmp(pointName, "demo6") == 0) {
            mPoints[mPointCount].mFlags |= POINT_DEMO6;
        } else if (strcmp(pointName, "demo7") == 0) {
            mPoints[mPointCount].mFlags |= POINT_DEMO7;
        } else if (strcmp(pointName, "camstop") == 0) {
            mPoints[mPointCount].mFlags |= POINT_CAMSTOP;
        } else if (strcmp(pointName, "noshift") == 0) {
            mPoints[mPointCount].mFlags |= POINT_NOSHIFT;
        } else if (strncmp(pointName, "board", 5) == 0) {
            mPoints[mPointCount].mFlags |= POINT_BOARD;
        } else if (strcmp(pointName, "demostop") == 0) {
            mPoints[mPointCount].mFlags |= POINT_DEMOSTOP;
        } else if (strcmp(pointName, "scrollY") == 0) {
            mPoints[mPointCount].mFlags |= POINT_SCROLL_Y;
        } else if (strcmp(pointName, "sand") == 0) {
            mPoints[mPointCount].mFlags |= POINT_SAND;
        } else if (strcmp(pointName, "ice") == 0) {
            mPoints[mPointCount].mFlags |= POINT_ICE;
        } else if (strcmp(pointName, "scrollA") == 0) {
            mPoints[mPointCount].mFlags |= POINT_SCROLL_A;
        }

        // Enemy points
        if (strcmp(pointName, "kuribo1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_KURIBO1;
        } else if (strcmp(pointName, "kuribo2") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_KURIBO2;
        } else if (strcmp(pointName, "Puku1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_PUKU1;
        } else if (strcmp(pointName, "Puku2") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_PUKU2;
        } else if (strcmp(pointName, "Pak1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_PAK1;
        } else if (strcmp(pointName, "Pak2") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_PAK2;
        } else if (strcmp(pointName, "Pak3") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_PAK3;
        } else if (strcmp(pointName, "Hbros1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_HBROS1;
        } else if (strcmp(pointName, "trap1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_TRAP1;
        } else if (strcmp(pointName, "trap2") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= ENEMY_TRAP2;
        }

        if (csv[pos] == '"') {
            pos++;
            break;
        }
        if (!isStr && csv[pos] == ',' || isLineEnd(csv, pos)) {
            break;
        }
        pos++;
    }
    if (!isLineEnd(csv, pos)) {
        pos++;
    }
}

void dCsvData_c::ReadOpenPointName(char *csv, int &pos, bool regularExit) {
    int count = 0;
    if (csv[pos] == '"' && csv[pos + 1] == '"') {
        pos += 2;
    } else if (csv[pos] == '"') {
        int pointIdx = 0;
        pos++;
        do {
            for (int i = 0; i < MAX_POINT_NAME_LEN - 1; i++) {
                if (csv[pos] == ',') {
                    continue;
                }
                if (regularExit) {
                    mPoints[mPointCount].mOpenPointNameRegular[pointIdx][i] = csv[pos];
                } else {
                    mPoints[mPointCount].mOpenPointNameSecret[pointIdx][i] = csv[pos];
                }
                pos++;
            }
            pointIdx++;
            count++;
        } while (csv[pos++] != '"');
    } else if (csv[pos] != ',') {
        int pointIdx = 0;
        do {
            for (int i = 0; i < MAX_POINT_NAME_LEN - 1; i++) {
                if (csv[pos] == ',') {
                    continue;
                }
                if (regularExit) {
                    mPoints[mPointCount].mOpenPointNameRegular[pointIdx][i] = csv[pos];
                } else {
                    mPoints[mPointCount].mOpenPointNameSecret[pointIdx][i] = csv[pos];
                }
                pos++;
            }
            pointIdx++;
            count++;
            while (csv[pos] != ',') {
                if (isLineEnd(csv, pos)) {
                    break;
                }
                pos++;
            }
        } while (csv[pos] != ',' && !isLineEnd(csv, pos));
    }
    if (regularExit) {
        mPoints[mPointCount].mOpenPointNumRegular = count;
    } else {
        mPoints[mPointCount].mOpenPointNumSecret = count;
    }
}

void dCsvData_c::ReadOpenRouteName(char *csv, int &pos, bool regularExit) {
    int count = 0;
    if (csv[pos] == '"' && csv[pos + 1] == '"') {
        pos += 2;
    } else if (csv[pos] == '"') {
        int routeIdx = 0;
        pos++;
        do {
            for (int i = 0; i < MAX_ROUTE_NAME_LEN - 1; i++) {
                if (csv[pos] == ',' || csv[pos] == '"') {
                    continue;
                }
                if (csv[pos] != ' ') {
                    if (regularExit) {
                        mPoints[mPointCount].mRouteNameRegular[routeIdx][i] = csv[pos];
                    } else {
                        mPoints[mPointCount].mRouteNameSecret[routeIdx][i] = csv[pos];
                    }
                }
                pos++;
            }
            routeIdx++;
            count++;
        } while (csv[pos++] != '"');
    } else if (csv[pos] != ',') {
        int routeIdx = 0;
        do {
            for (int i = 0; i < MAX_ROUTE_NAME_LEN - 1; i++) {
                if (csv[pos] == ',' || csv[pos] == '"') {
                    continue;
                }
                if (csv[pos] != ' ') {
                    if (regularExit) {
                        mPoints[mPointCount].mRouteNameRegular[routeIdx][i] = csv[pos];
                    } else {
                        mPoints[mPointCount].mRouteNameSecret[routeIdx][i] = csv[pos];
                    }
                }
                pos++;
            }
            routeIdx++;
            count++;
            while (csv[pos] != ',') {
                if (isLineEnd(csv, pos)) {
                    break;
                }
                pos++;
            }
        } while (csv[pos] != ',' && !isLineEnd(csv, pos));
    }
    if (regularExit) {
        mPoints[mPointCount].mOpenRouteNumRegular = count;
    } else {
        mPoints[mPointCount].mOpenRouteNumSecret = count;
    }
}

void dCsvData_c::ReadFlagData(char *csv, int &pos, bool regularExit) {
    int flagIndex, i;
    bool isStr = false;
    if (csv[pos] == '"') {
        isStr = true;
        pos++;
    }
    flagIndex = 0;
    while (true) {
        i = 0;
        while (true) {
            if (csv[pos] == ',' || csv[pos] == '"' || csv[pos] == '\r' && csv[pos + 1] == '\n') {
                if (regularExit) {
                    mPoints[mPointCount].mFlagDataRegular[flagIndex][i] = '\0';
                } else {
                    mPoints[mPointCount].mFlagDataSecret[flagIndex][i] = '\0';
                }
                if (i > 0) {
                    if (regularExit) {
                        mPoints[mPointCount].mFlagDataNumRegular++;
                    } else {
                        mPoints[mPointCount].mFlagDataNumSecret++;
                    }
                }
                break;
            }
            if (regularExit) {
                mPoints[mPointCount].mFlagDataRegular[flagIndex][i] = csv[pos];
            } else {
                mPoints[mPointCount].mFlagDataSecret[flagIndex][i] = csv[pos];
            }
            i++;
            pos++;
        }
        if (csv[pos] == '\"') {
            pos++;
            break;
        }
        if (!isStr && csv[pos] == ',' || csv[pos] == '\r' && csv[pos + 1] == '\n') {
            if (i == 0 && csv[pos] == ',') {
                pos++;
            }
            break;
        }
        flagIndex++;
        pos++;
    }
}

void dCsvData_c::ReadAnimeRouteName(char *csv, int &pos) {
    SubRoute_s *firstFreeRoute = &mSubRoutes[mSubrouteCount];

    char buf[MAX_ROUTE_NAME_LEN];
    int len = read(buf, &csv[pos], sizeof(buf));

    bool found = false;
    for (int i = 0; i < mSubrouteCount - 1; i++) {
        if (strncmp(mSubRoutes[i].mName, buf, sizeof(buf)) == 0) {
            found = true;
        }
    }
    if (!found) {
        // Add if doesn't exist yet
        strncpy(firstFreeRoute->mName, buf, sizeof(firstFreeRoute->mName));
    }

    pos += len;
}

void dCsvData_c::ReadAction(char *csv, int &pos) {
    char buf[20];
    pos += read(buf, &csv[pos], sizeof(buf));

    // [Must have some static local here based on symbol names...]
    static int unusedLocal[] = { 0, 0, 0, 0 };

    static const char *l_actionName[] = {
        CSV_ACTION_ROAD,
        CSV_ACTION_SAND,
        CSV_ACTION_WOOD,
        CSV_ACTION_JUMP,
        CSV_ACTION_LADDER,
        CSV_ACTION_VINE,
        CSV_ACTION_SLOPE,
        CSV_ACTION_ICE_SLOPE,
        CSV_ACTION_SWITCH_BLOCK,
        CSV_ACTION_QUICKSAND,
        CSV_ACTION_SNOW,
        CSV_ACTION_ICE,
        CSV_ACTION_CLOUD,
        CSV_ACTION_WATER,
        CSV_ACTION_RIGHT_LADDER,
        CSV_ACTION_LEFT_LADDER,
        CSV_ACTION_ROCK_LADDER,
        CSV_ACTION_ROPE_LADDER,
        CSV_ACTION_DIRT,
        nullptr
    };

    static const ActionType_e l_actionType[] = {
        ACTION_TYPE_ROAD,
        ACTION_TYPE_SAND,
        ACTION_TYPE_WOOD,
        ACTION_TYPE_JUMP,
        ACTION_TYPE_LADDER,
        ACTION_TYPE_VINE,
        ACTION_TYPE_SLOPE,
        ACTION_TYPE_ICE_SLOPE,
        ACTION_TYPE_SWITCH_BLOCK,
        ACTION_TYPE_QUICKSAND,
        ACTION_TYPE_SNOW,
        ACTION_TYPE_ICE,
        ACTION_TYPE_CLOUD,
        ACTION_TYPE_WATER,
        ACTION_TYPE_RIGHT_LADDER,
        ACTION_TYPE_LEFT_LADDER,
        ACTION_TYPE_ROCK_LADDER,
        ACTION_TYPE_ROPE_LADDER,
        ACTION_TYPE_DIRT,
    };

    if (mSubRoutes[mSubrouteCount].mName[0] != '0') {
        int i = 0;
        while (l_actionName[i] != nullptr) {
            if (strcmp(l_actionName[i], buf) == 0) {
                mSubRoutes[mSubrouteCount].mActionType = l_actionType[i];
                break;
            }
            i++;
        }
    }
}

void dCsvData_c::ReadRouteFlag(char *csv, int &pos) {
    if (csv[pos] == '\r' && csv[pos + 1] == '\n') {
        return;
    }

    bool isStr = false;
    if (csv[pos] == '"') {
        isStr = true;
        pos++;
    }
    while (true) {
        int i = 0;
        char buf[40];
        while (true) {
            if (csv[pos] == ',' || csv[pos] == '"' || csv[pos] == '\r' && csv[pos + 1] == '\n') {
                buf[i] = '\0';
                break;
            }
            buf[i] = csv[pos];
            i++;
            pos++;
        }
        if (mSubRoutes[mSubrouteCount].mName[0] != '0') {
            if (strcmp(buf, "A") == 0) {
                mSubRoutes[mSubrouteCount].mFlags |= BIT_FLAG(0);
            } else if (strcmp(buf, "B") == 0) {
                mSubRoutes[mSubrouteCount].mFlags |= BIT_FLAG(1);
            } else if (strcmp(buf, "C") == 0) {
                mSubRoutes[mSubrouteCount].mFlags |= BIT_FLAG(2);
            }
        }
        if (csv[pos] == '\"') {
            pos++;
            break;
        }
        if (!isStr && csv[pos] == ',' || csv[pos] == '\r' && csv[pos + 1] == '\n') {
            break;
        }
        pos++;
    }
}

dCsvData_c::ActionType_e dCsvData_c::GetActionLabel(const char *route) {
    for (int i = 0; i < mSubrouteCount; i++) {
        if (strcmp(mSubRoutes[i].mName, route) == 0) {
            return mSubRoutes[i].mActionType;
        }
    }
    return ACTION_TYPE_NONE;
}

int dCsvData_c::GetIndexFromPointName(const char *pointName) {
    for (int i = 0; i < mPointCount; i++) {
        if (strcmp(mPoints[i].mName, pointName) == 0) {
            return i;
        }
    }
    return -1;
}

void dCsvData_c::addKeyPoint(const m3d::mdl_c &model) {
    nw4r::g3d::ResMdl res = model.getResMdl();
    for (int i = 0; i < res.GetResNodeNumEntries(); i++) {
        nw4r::g3d::ResNode node = res.GetResNode(i);
        const char *name = node.GetName();
        if (name[0] == 'K') {
            strncpy(mPoints[mPointCount].mName, name, sizeof(mPoints[mPointCount].mName));
            mPointCount++;
        }
    }
}

void dCsvData_c::SetRouteInfo(const m3d::mdl_c &model) {
    nw4r::g3d::ResMdl res = model.getResMdl();
    for (int i = 0; i < res.GetResNodeNumEntries(); i++) {
        nw4r::g3d::ResNode node = res.GetResNode(i);
        const char *name = node.GetName();
        if (strlen(name) != 9) {
            continue;
        }
        if (name[0] == 'R' && name[4] >= '0' && name[4] <= '9' && name[8] >= '0' && name[8] <= '9') {
            strncpy(mRoutes[mRouteCount].mName, name, sizeof(mRoutes[mRouteCount].mName));

            char startPoint[MAX_POINT_NAME_LEN];
            char endPoint[MAX_POINT_NAME_LEN];
            dWmLib::GetStartPointNameFromRouteName(mRoutes[mRouteCount].mName, startPoint);
            dWmLib::GetEndPointNameFromRouteName(mRoutes[mRouteCount].mName, endPoint);

            initRouteStruct(&mRoutes[mRouteCount], 20);
            findSubRoutesForRoute(startPoint, endPoint, &mRoutes[mRouteCount], 20);
            appendChildFromSubRoute(startPoint, endPoint, &mRoutes[mRouteCount], 20);

            mRouteCount++;

            adjustChildInfo(&mRoutes[mRouteCount - 1]);
            appendChildFromModel(node, mRouteCount - 1);
        }
    }
}

const char *dCsvData_c::GetPointName(int idx) const {
    return mPoints[idx].mName;
}

const char *dCsvData_c::GetOpenPointName(bool regularExit, int idx, int openPointIdx) const {
    if (regularExit) {
        return mPoints[idx].mOpenPointNameRegular[openPointIdx];
    } else {
        return mPoints[idx].mOpenPointNameSecret[openPointIdx];
    }
}

int dCsvData_c::GetOpenPointNum(bool regularExit, int idx) const {
    if (regularExit) {
        return mPoints[idx].mOpenPointNumRegular;
    } else {
        return mPoints[idx].mOpenPointNumSecret;
    }
}

const char *dCsvData_c::GetOpenRouteName(bool regularExit, int idx, int openRouteIdx) const {
    if (regularExit) {
        return mPoints[idx].mRouteNameRegular[openRouteIdx];
    } else {
        return mPoints[idx].mRouteNameSecret[openRouteIdx];
    }
}

int dCsvData_c::GetOpenRouteNum(bool regularExit, int idx) const {
    if (regularExit) {
        return mPoints[idx].mOpenRouteNumRegular;
    } else {
        return mPoints[idx].mOpenRouteNumSecret;
    }
}

u32 dCsvData_c::GetPointFlags(int idx, PointFlag_e flags) const {
    return mPoints[idx].mFlags & flags;
}

u32 dCsvData_c::GetEnemyPointFlags(int idx, EnemyFlag_e flags) const {
    return mPoints[idx].mFlagsEnemy & flags;
}

u32 dCsvData_c::GetPointParam(int idx) {
    return mPoints[idx].mParam;
}

const char *dCsvData_c::GetRouteName(int idx) {
    return mRoutes[idx].mName;
}

const char *dCsvData_c::GetChildPointName(int idx, int childIdx) {
    return mRoutes[idx].mChildPoints[childIdx];
}

const char *dCsvData_c::GetSubRouteName(int idx) {
    return mSubRoutes[idx].mName;
}

int dCsvData_c::GetSubRouteIdx(const char *pointA, const char *pointB) {
    int i = 0;
    while (i < mSubrouteCount) {
        const char *subRoutePointA = &GetSubRouteName(i)[1];
        const char *subRoutePointB = &GetSubRouteName(i)[5];
        if (
            strncmp(subRoutePointA, pointA, 4) == 0 && strncmp(subRoutePointB, pointB, 4) == 0 ||
            strncmp(subRoutePointB, pointA, 4) == 0 && strncmp(subRoutePointA, pointB, 4) == 0
        ) {
            return i;
        }
        i++;
    }
    return -1;
}

int dCsvData_c::GetPointNum(int idx) {
    return mRoutes[idx].mChildPointNum;
}

u32 dCsvData_c::GetSubRouteFlag(int idx) {
    return mSubRoutes[idx].mFlags;
}

int dCsvData_c::GetRouteAnimNum(bool regularExit, int idx) {
    if (regularExit) {
        return mPoints[idx].mFlagDataNumRegular;
    } else {
        return mPoints[idx].mFlagDataNumSecret;
    }
}

const char *dCsvData_c::GetRouteAnimName(bool regularExit, int idx, int animIdx) {
    if (regularExit) {
        return mPoints[idx].mFlagDataRegular[animIdx];
    } else {
        return mPoints[idx].mFlagDataSecret[animIdx];
    }
}

void dCsvData_c::initRouteStruct(Route_s *route, int childCount) {
    route->mChildPoints = new char[childCount][MAX_POINT_NAME_LEN];
    for (int i = 0; i < childCount; i++) {
        memset(route->mChildPoints[i], 0, MAX_POINT_NAME_LEN);
    }
    route->mChildPointNum = 0;
    route->mNumSubroutes = 99999;
}

void dCsvData_c::destroyRouteStruct(Route_s *route) {
    delete[] route->mChildPoints;
}

bool dCsvData_c::appendChildFromSubRoute(const char *startPointName, const char *endPointName, Route_s *route, int maxLevel, bool resetLevel) {
    static int s_Level = 0;
    bool success = false;

    if (resetLevel) {
        s_Level = 0;
    }

    s_Level++;
    if (s_Level >= maxLevel) {
        s_Level--;
        return false;
    }

    for (int i = 0; i < mSubrouteCount; i++) {
        char routeStartPoint[MAX_POINT_NAME_LEN];
        char routeEndPoint[MAX_POINT_NAME_LEN];

        dWmLib::GetStartPointNameFromRouteName(mSubRoutes[i].mName, routeStartPoint);
        if (strncmp(routeStartPoint, startPointName, ARRAY_MAX_STRLEN(routeStartPoint)) == 0) {
            dWmLib::GetEndPointNameFromRouteName(mSubRoutes[i].mName, routeEndPoint);
            if (strncmp(routeEndPoint, endPointName, ARRAY_MAX_STRLEN(routeEndPoint)) == 0) {
                if (route->mNumSubroutes == s_Level) {
                    success = true;
                }
                break;
            }

            if (appendChildFromSubRoute(routeEndPoint, endPointName, route, maxLevel, false)) {
                strncpy(route->mChildPoints[route->mChildPointNum], routeEndPoint, ARRAY_MAX_STRLEN(route->mChildPoints[route->mChildPointNum]));
                route->mChildPoints[route->mChildPointNum][5] = '0'; // [Not sure what this is... this also writes out-of-bounds!]
                route->mChildPointNum++;
                success = true;
                break;
            }
        }
    }
    s_Level--;

    return success;
}

bool dCsvData_c::findSubRoutesForRoute(const char *startPointName, const char *endPointName, Route_s *route, int maxLevel, bool resetLevel) {
    static int s_Level = 0;

    if (resetLevel) {
        s_Level = 0;
    }

    bool success = false;
    s_Level++;
    if (s_Level >= maxLevel) {
        s_Level--;
        return false;
    }

    for (int i = 0; i < mSubrouteCount; i++) {
        char routeStartPoint[MAX_POINT_NAME_LEN];
        char routeEndPoint[MAX_POINT_NAME_LEN];

        dWmLib::GetStartPointNameFromRouteName(mSubRoutes[i].mName, routeStartPoint);
        if (strncmp(routeStartPoint, startPointName, ARRAY_MAX_STRLEN(routeStartPoint)) == 0) {
            dWmLib::GetEndPointNameFromRouteName(mSubRoutes[i].mName, routeEndPoint);
            if (strncmp(routeEndPoint, endPointName, ARRAY_MAX_STRLEN(routeEndPoint)) == 0) {
                if (route->mNumSubroutes > s_Level) {
                    route->mNumSubroutes = s_Level;
                }
                success = true;
                break;
            }

            findSubRoutesForRoute(routeEndPoint, endPointName, route, maxLevel, false);
        }
    }

    s_Level--;

    return success;
}

void dCsvData_c::appendChildFromModel(const nw4r::g3d::ResNode &node, int routeIdx) {
    // [Fake match...]
    nw4r::g3d::ResNodeData *nodeData = (nw4r::g3d::ResNodeData *) (node.ptr());
    nw4r::g3d::ResNode curr;

    curr = nw4r::g3d::ResNode(nodeData).GetChildNode();
    int count = 0;
    while (curr.IsValid()) {
        count++;
        curr = curr.GetChildNode();
    }

    if (count < mRoutes[routeIdx].mChildPointNum) {
        return;
    }

    curr = nw4r::g3d::ResNode(nodeData).GetChildNode();
    mRoutes[routeIdx].mChildPointNum = 0;
    while (curr.IsValid()) {
        strncpy(mRoutes[routeIdx].mChildPoints[mRoutes[routeIdx].mChildPointNum], curr.GetName(), MAX_POINT_NAME_LEN);
        mRoutes[routeIdx].mChildPointNum++;
        curr = curr.GetChildNode();
    }
}

void dCsvData_c::adjustChildInfo(Route_s *route) {
    static char s_Tmp[MAX_POINT_NAME_LEN + 1];

    int pointNum = route->mChildPointNum;
    if (pointNum > 1) {
        for (int i = 0; i < pointNum / 2; i++) {
            int otherIdx = pointNum - i - 1;
            strncpy(s_Tmp, route->mChildPoints[i], MAX_POINT_NAME_LEN);
            strncpy(route->mChildPoints[i], route->mChildPoints[otherIdx], MAX_POINT_NAME_LEN);
            strncpy(route->mChildPoints[otherIdx], s_Tmp, MAX_POINT_NAME_LEN);
        }
    }
}

bool dCsvData_c::isLineEnd(char *csv, int pos) {
    bool lineEnd = false;
    if (csv[pos] == '\r' && csv[pos + 1] == '\n') {
        lineEnd = true;
    }
    return lineEnd;
}
