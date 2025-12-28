#pragma once
#include <types.h>
#include <constants/game_constants.h>
#include <game/bases/d_cd_data.hpp>
#include <game/sLib/s_RangeData.hpp>
#include <lib/egg/core/eggHeap.h>

/// @brief Course data file holder.
/// A course data file contains the actual course elements -
/// areas, map actor information, background data, etc.
class dCdFile_c {
public:
    dCdFile_c() {}
    virtual ~dCdFile_c() {}

    void loadCourseData(int fileNo);
    sScrollData *getScrollDataP(u8 id);
    sBgData *getBg2DataP(u8 id);
    sBgData *getBg3DataP(u8 id);
    sNextGotoData *getNextGotoP(u8 id);
    sAreaData *getAreaDataP(u8 id, sRangeDataF *bound);
    u8 getAreaID(u8 id);
    int getAreaScroll(u8 id);
    int getAreaZoomType(u8 id);
    int getAreaZoomID(u8 id);
    int getAreaZoomChange(u8 id);
    int getAreaBgm(u8 id);
    int getAreaBgmMode(u8 id);
    int getAreaMaskNo(u8 id);
    int getAreaBg2(u8 id);
    int getAreaBg3(u8 id);
    int getAreaColorOBJ(u8 id);
    int getAreaColorBG(u8 id);
    int getAreaDirection(u8 id);
    int getAreaSizeX(u8 id);
    u8 getAreaNo(mVec3_c *pos);
    u8 getAreaNo(sRangePosSize *bound);
    sRangeData *getRangeDataP(u8 id, sRangeDataF *bound);
    bool checkRange(mVec3_c *pos, sRangeData *range);
    bool checkRange(sRangePosSize *range1, sRangePosSize *range2);
    sScrollAreaData *getScrlAreaDataP(u8 id);

    int mFileNo;
    union {
        void *mpBlocks[CD_BLOCK_COUNT];
        struct {
            sTilesetData *mpTilesetNames;
            sOptionData *mpOptions;
            sScrollData *mpScrollData;
            sBlock4Data *mBlock4;
            sBgData *mpBg2Data;
            sBgData *mpBg3Data;
            sNextGotoData *mpNextGotos;
            sMapActorData *mpMapActors;
            sBlock8Data *mBlock8;
            sAreaData *mpAreas;
            sRangeData *mpRangeData;
            sScrollAreaData *mpScrlAreas;
            sRailInfoData *mpRails;
            sRailNodeData *mpRailNodes;
        };
    };
    u32 mBlockSizes[CD_BLOCK_COUNT];
    union {
        u32 mBlockElementCounts[CD_BLOCK_COUNT];
        struct {
            int mTilesetsCount;
            int mOptionsCount;
            int mScrollDataCount;
            int mBlock4ElementCount;
            int mBg2DataCount;
            int mBg3DataCount;
            int mNextGotoCount;
            int mMapActorCount;
            int mBlock8ElementCount;
            int mAreaCount;
            int mRangeCount;
            int mScrlAreaCount;
            int mRailCount;
            int mRailNodeCount;
        };
    };
    sMapActorData *mMapActorsByArea[MAX_AREAS]; ///< Pointers to the first map actor for an area.
    u32 mMapActorCountByArea[MAX_AREAS]; ///< Number of map actors per area.
    u32 mMapActorIdxForArea[MAX_AREAS]; ///< Index of the first map actor for an area.
};

/// @brief Current course data holder.
/// A course is made up of multiple files [Reggie terminology: "areas"] - see dCdFile_c.
class dCd_c {
public:
    dCd_c() { m_instance = this; }
    ~dCd_c() { m_instance = nullptr; }

    void loadCourseData();

    static void setCourseArcName(int world, int course, char *buffer);
    static void createInstance(EGG::Heap *heap);
    static void deleteInstance();

    dCdFile_c *getFileP(int idx) {
        dCdFile_c *course = &mFiles[idx];
        if (course->mpAreas != nullptr) {
            return course;
        }
        return nullptr;
    }

    dCdFile_c mFiles[COURSE_FILE_COUNT];

    static dCd_c *m_instance;
};
