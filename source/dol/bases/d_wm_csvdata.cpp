#include <game/bases/d_wm_csvdata.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <lib/nw4r/g3d.h>
#include <constants/sjis_constants.h>

extern "C" int atoi(const char *);

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

int read(char *buffer, const char *b, int c) {
    int i = 0;
    if (*b == '"') {
        b++;
        if (*b == '"') {
            return 0;
        }
    }
    if (*b == ',') {
        return 0;
    }
    while (true) {
        if (*b == ',' || *b == '"' || *b == '\r' && *(b + 1) == '\n') {
            buffer[i] = 0;
            break;
        }
        buffer[i] = *b;
        i++;
        b++;
    }
    return i;
}

dCsvData_c::~dCsvData_c() {
    for (int i = 0; i < (int) ARRAY_SIZE(mRoutes); i++) {
        if (mRoutes[i].mChildPoints != 0) {
            destroyRouteStruct(&mRoutes[i]);
        }
    }
}

void dCsvData_c::initialize(int a, int b) {
    mWorld = a;
    m_08 = b;
    RouteInfoInit();
    ReadCsvData();
}

void dCsvData_c::RouteInfoInit() {
    mPointCount = 0;
    mRouteCount = 0;
    m_14 = 0;
    for (int i = 0; i < (int) ARRAY_SIZE(mPoints); i++) {
        for (int j = 0; j < (int) ARRAY_SIZE(mPoints[i].mName); j++) {
            mPoints[i].mName[j] = '\0';
        }
        for (int k = 0; k < (int) ARRAY_SIZE(mPoints[i].mOpenPointName1); k++) {
            for (int j = 0; j < (int) ARRAY_SIZE(mPoints[i].mOpenPointName1[k]); j++) {
                mPoints[i].mOpenPointName1[k][j] = '\0';
                mPoints[i].mOpenPointName2[k][j] = '\0';
            }
        }
        // [Shouldn't this be 16?]
        for (int k = 0; k < 64; k++) {
            for (int j = 0; j < (int) ARRAY_SIZE(mPoints[i].mRouteName1[k]); j++) {
                mPoints[i].mRouteName1[k][j] = '\0';
                mPoints[i].mRouteName2[k][j] = '\0';
            }
        }
        for (int k = 0; k < (int) ARRAY_SIZE(mPoints[i].mAnimName1); k++) {
            for (int j = 0; j < (int) ARRAY_SIZE(mPoints[i].mAnimName1[k]); j++) {
                mPoints[i].mAnimName1[k][j] = '\0';
                mPoints[i].mAnimName2[k][j] = '\0';
            }
        }
        mPoints[i].mAnimNum1 = 0;
        mPoints[i].mAnimNum2 = 0;
        mPoints[i].mFlags = 0;
        mPoints[i].mParam = 0;
        mPoints[i].mFlagsEnemy = 0;
        mPoints[i].m_1bc = 0;
    }
    for (int i = 0; i < (int) ARRAY_SIZE(mSubRoutes); i++) {
        for (int j = 0; j < (int) ARRAY_SIZE(mSubRoutes[i].mName); j++) {
            mSubRoutes[i].mName[j] = '\0';
        }
        mSubRoutes[i].mActionLabel = -1;
        mSubRoutes[i].m_10 = 0;
        mSubRoutes[i].mFlags = 0;
    }
    for (int i = 0; i < (int) ARRAY_SIZE(mRoutes); i++) {
        for (int j = 0; j < (int) ARRAY_SIZE(mRoutes[i].mName); j++) {
            mRoutes[i].mName[j] = '\0';
        }
        mRoutes[i].mChildPoints = 0;
        mRoutes[i].m_14 = 99999;
        mRoutes[i].mPointNum = 0;
    }
}

void dCsvData_c::ReadCsvData() {
    dResMng_c *resMng = dResMng_c::m_instance;

    u8 unkLocal;
    char fileName[0x20];
    snprintf(fileName, sizeof(fileName), "W%X/pointW%X.csv", mWorld + 1, mWorld + 1);
    size_t fileSize;
    char *csv = (char *) resMng->getResSilently(l_routeInfoArcName, fileName, &fileSize).ptr();
    if (csv == nullptr) {
        snprintf(fileName, sizeof(fileName), "W%X/pointW%Xa.csv", mWorld + 1, mWorld + 1);
        unkLocal += m_08;
        csv = (char *) resMng->getRes(l_routeInfoArcName, fileName, &fileSize).ptr();
    }

    int pos = 0;
    do {
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

    snprintf(fileName, sizeof(fileName), "W%X/routeW%X.csv", mWorld + 1, mWorld + 1);
    csv = (char *) resMng->getResSilently(l_routeInfoArcName, fileName, &fileSize).ptr();
    if (csv == nullptr) {
        snprintf(fileName, sizeof(fileName), "W%X/routeW%Xa.csv", mWorld + 1, mWorld + 1);
        unkLocal += m_08;
        csv = (char *) resMng->getRes(l_routeInfoArcName, fileName, &fileSize).ptr();
    }

    pos = 0;
    do {
        ReadAnimeRouteName(csv, pos);
        pos++;
        ReadAction(csv, pos);
        pos++;
        ReadRouteFlag(csv, pos);
        if (mSubRoutes[mRouteCount].mName[0] != '0') {
            mRouteCount++;
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
            mPoints[mPointCount].mFlags |= BIT_FLAG(0);
        } else if (strcmp(pointName, "stop") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(1);
        } else if (strcmp(pointName, "link1") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(2);
        } else if (strcmp(pointName, "link2") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(3);
        } else if (strcmp(pointName, "link3") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(4);
        } else if (strcmp(pointName, "link4") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(5);
        } else if (strcmp(pointName, "link5") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(6);
        } else if (strcmp(pointName, "scroll") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(7);
        } else if (strcmp(pointName, "scale") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(8);
        } else if (strcmp(pointName, "dokan") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(10);
        } else if (strcmp(pointName, "switch") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(11);
        } else if (strcmp(pointName, "crossroad") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(18);
        } else if (strcmp(pointName, "focus") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(19);
        } else if (strncmp(pointName, "anchor", 6) == 0) {
            int len = strlen(pointName);
            if (len == 6) {
                mPoints[mPointCount].mFlags |= BIT_FLAG(12);
                mPoints[mPointCount].mFlags |= BIT_FLAG(13);
            } else if (len == 7 || len == 8) {
                if (pointName[6] == 'x') {
                    mPoints[mPointCount].mFlags |= BIT_FLAG(12);
                } else if (pointName[6] == 'y') {
                    mPoints[mPointCount].mFlags |= BIT_FLAG(13);
                } else if ('1' <= pointName[6] && pointName[6] <= '9') {
                    mPoints[mPointCount].mParam = atoi(&pointName[6]);
                }
                if ('1' <= pointName[7] && pointName[7] <= '9') {
                    mPoints[mPointCount].mParam = atoi(&pointName[7]);
                }
            }
        } else if (strcmp(pointName, "tilt") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(14);
        } else if (strcmp(pointName, "demo1") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(15);
        } else if (strcmp(pointName, "demo2") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(16);
        } else if (strcmp(pointName, "demo3") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(17);
        } else if (strcmp(pointName, "demo4") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(25);
        } else if (strcmp(pointName, "demo5") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(26);
        } else if (strcmp(pointName, "demo6") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(27);
        } else if (strcmp(pointName, "demo7") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(28);
        } else if (strcmp(pointName, "camstop") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(29);
        } else if (strcmp(pointName, "noshift") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(30);
        } else if (strncmp(pointName, "board", 5) == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(20);
        } else if (strcmp(pointName, "demostop") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(21);
        } else if (strcmp(pointName, "scrollY") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(22);
        } else if (strcmp(pointName, "sand") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(23);
        } else if (strcmp(pointName, "ice") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(24);
        } else if (strcmp(pointName, "scrollA") == 0) {
            mPoints[mPointCount].mFlags |= BIT_FLAG(31);
        }

        // Enemy points
        if (strcmp(pointName, "kuribo1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(1);
        } else if (strcmp(pointName, "kuribo2") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(2);
        } else if (strcmp(pointName, "Puku1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(3);
        } else if (strcmp(pointName, "Puku2") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(4);
        } else if (strcmp(pointName, "Pak1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(5);
        } else if (strcmp(pointName, "Pak2") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(6);
        } else if (strcmp(pointName, "Pak3") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(7);
        } else if (strcmp(pointName, "Hbros1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(8);
        } else if (strcmp(pointName, "trap1") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(10);
        } else if (strcmp(pointName, "trap2") == 0) {
            mPoints[mPointCount].mFlagsEnemy |= BIT_FLAG(11);
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

void dCsvData_c::ReadOpenPointName(char *csv, int &pos, bool first) {
    int count = 0;
    if (csv[pos] == '"' && csv[pos + 1] == '"') {
        pos += 2;
    } else if (csv[pos] == '"') {
        int x = 0;
        pos++;
        do {
            for (int i = 0; i < POINT_NAME_MAX_LEN - 1; i++) {
                if (csv[pos] != ',') {
                    if (first) {
                        mPoints[mPointCount].mOpenPointName1[x][i] = csv[pos];
                    } else {
                        mPoints[mPointCount].mOpenPointName2[x][i] = csv[pos];
                    }
                    pos++;
                }
            }
            x++;
            count++;
        } while (csv[pos++] != '"');
    } else if (csv[pos] != ',') {
        int x = 0;
        do {
            for (int i = 0; i < POINT_NAME_MAX_LEN - 1; i++) {
                if (csv[pos] != ',') {
                    if (first) {
                        mPoints[mPointCount].mOpenPointName1[x][i] = csv[pos];
                    } else {
                        mPoints[mPointCount].mOpenPointName2[x][i] = csv[pos];
                    }
                    pos++;
                }
            }
            x++;
            count++;
            while (csv[pos] != ',') {
                if (isLineEnd(csv, pos)) {
                    break;
                }
                pos++;
            }
        } while (csv[pos] != ',' && !isLineEnd(csv, pos));
    }
    if (first) {
        mPoints[mPointCount].mOpenPointNum1 = count;
    } else {
        mPoints[mPointCount].mOpenPointNum2 = count;
    }
}

void dCsvData_c::ReadOpenRouteName(char *csv, int &pos, bool first) {
    int count = 0;
    if (csv[pos] == '"' && csv[pos + 1] == '"') {
        pos += 2;
    } else if (csv[pos] == '"') {
        int x = 0;
        pos++;
        do {
            for (int i = 0; i < ROUTE_NAME_MAX_LEN - 1; i++) {
                if (csv[pos] != ',' && csv[pos] != '"') {
                    if (csv[pos] != ' ') {
                        if (first) {
                            mPoints[mPointCount].mRouteName1[x][i] = csv[pos];
                        } else {
                            mPoints[mPointCount].mRouteName2[x][i] = csv[pos];
                        }
                    }
                    pos++;
                }
            }
            x++;
            count++;
        } while (csv[pos++] != '"');
    } else if (csv[pos] != ',') {
        int x = 0;
        do {
            for (int i = 0; i < ROUTE_NAME_MAX_LEN - 1; i++) {
                if (csv[pos] != ',' && csv[pos] != '"') {
                    if (csv[pos] != ' ') {
                        if (first) {
                            mPoints[mPointCount].mRouteName1[x][i] = csv[pos];
                        } else {
                            mPoints[mPointCount].mRouteName2[x][i] = csv[pos];
                        }
                    }
                    pos++;
                }
            }
            x++;
            count++;
            while (csv[pos] != ',') {
                if (isLineEnd(csv, pos)) {
                    break;
                }
                pos++;
            }
        } while (csv[pos] != ',' && !isLineEnd(csv, pos));
    }
    if (first) {
        mPoints[mPointCount].mOpenRouteNum1 = count;
    } else {
        mPoints[mPointCount].mOpenRouteNum2 = count;
    }
}

void dCsvData_c::ReadFlagData(char *csv, int &pos, bool first) {
    int x, i;
    bool isStr = false;
    if (csv[pos] == '"') {
        isStr = true;
        pos++;
    }
    x = 0;
    while (true) {
        i = 0;
        while (true) {
            if (csv[pos] == ',' || csv[pos] == '"' || csv[pos] == '\r' && csv[pos + 1] == '\n') {
                if (first) {
                    mPoints[mPointCount].mAnimName1[x][i] = '\0';
                } else {
                    mPoints[mPointCount].mAnimName2[x][i] = '\0';
                }
                if (i > 0) {
                    if (first) {
                        mPoints[mPointCount].mAnimNum1++;
                    } else {
                        mPoints[mPointCount].mAnimNum2++;
                    }
                }
                break;
            }
            if (first) {
                mPoints[mPointCount].mAnimName1[x][i] = csv[pos];
            } else {
                mPoints[mPointCount].mAnimName2[x][i] = csv[pos];
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
        x++;
        pos++;
    }
}

void dCsvData_c::ReadAnimeRouteName(char *csv, int &pos) {
    SubRoute_s *firstFreeRoute = &mSubRoutes[mRouteCount];

    char buf[10];
    int len = read(buf, &csv[pos], sizeof(buf));

    bool found = false;
    for (int i = 0; i < mRouteCount - 1; i++) {
        if (strncmp(mSubRoutes[i].mName, buf, sizeof(buf)) == 0) {
            found = true;
        }
    }
    if (!found) {
        // Add if doesn't exist yet
        strncpy(firstFreeRoute->mName, buf, sizeof(buf));
    }

    pos += len;
}

void dCsvData_c::ReadAction(char *csv, int &pos) {
    char buf[20];
    pos += read(buf, &csv[pos], sizeof(buf));

    static const int unused[] = { 0 };

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

    static const int l_actionType[] = {
        2,
        15,
        26,
        4,
        3,
        6,
        7,
        8,
        14,
        16,
        17,
        18,
        19,
        20,
        22,
        21,
        24,
        25,
        23
    };

    if (mSubRoutes[mRouteCount].mName[0] != '0') {
        int i = 0;
        while (l_actionName[i] != nullptr) {
            if (strcmp(l_actionName[i], buf) == 0) {
                mSubRoutes[mRouteCount].mActionLabel = l_actionType[i];
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
        if (mSubRoutes[mRouteCount].mName[0] != '0') {
            if (strcmp(buf, "A") == 0) {
                mSubRoutes[mRouteCount].mFlags |= BIT_FLAG(0);
            } else if (strcmp(buf, "B") == 0) {
                mSubRoutes[mRouteCount].mFlags |= BIT_FLAG(1);
            } else if (strcmp(buf, "C") == 0) {
                mSubRoutes[mRouteCount].mFlags |= BIT_FLAG(2);
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

int dCsvData_c::GetActionLabel(const char *route) {
    for (int i = 0; i < mRouteCount; i++) {
        if (strcmp(mSubRoutes[i].mName, route) == 0) {
            return mSubRoutes[i].mActionLabel;
        }
    }
    return -1;
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
