#include <game/bases/d_cd.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/mLib/m_heap.hpp>
#include <MSL/cstdio>

void dCdFile_c::loadCourseData(int fileNo) {
    mFileNo = fileNo;

    char path[0x30];
    sprintf(path, "course/course%d.bin", fileNo + 1);

    static const u32 c_data_size[CD_BLOCK_COUNT] = {
        sizeof(sTilesetData),
        sizeof(sOptionData),
        sizeof(sScrollData),
        sizeof(sBlock4Data),
        sizeof(sBgData),
        sizeof(sBgData),
        sizeof(sNextGotoData),
        sizeof(sMapActorData),
        sizeof(sBlock8Data),
        sizeof(sAreaData),
        sizeof(sRangeData),
        sizeof(sScrollAreaData),
        sizeof(sRailInfoData),
        sizeof(sRailNodeData)
    };

    sBlockInfo *res = (sBlockInfo *) dResMng_c::m_instance->getResSilently(dScStage_c::mCdArcName, path).ptr();
    for (int i = 0; i < CD_BLOCK_COUNT; i++) {
        if (res != nullptr) {
            mpBlocks[i] = (u8 *) res + res[i].mOffset;
            mBlockSizes[i] = res[i].mSize;
            mBlockElementCounts[i] = mBlockSizes[i] / c_data_size[i];
        } else {
            mpBlocks[i] = nullptr;
            mBlockSizes[i] = 0;
            mBlockElementCounts[i] = 0;
        }
    }
    for (int i = 0; i < MAX_AREAS; i++) {
        mMapActorsByArea[i] = 0;
        mMapActorCountByArea[i] = 0;
        mMapActorIdxForArea[i] = 0;
    }
    u32 count = mMapActorCount;
    if (count != 0) {
        // This assumes that the actor creates are sorted by area already.
        sMapActorData *currCreate = mpMapActors;
        for (int i = 0; i < count; i++) {
            u8 n = currCreate->mAreaNo;
            if (n < MAX_AREAS) {
                mMapActorCountByArea[n]++;
            }
            currCreate++;
        }
        int sum = 0;
        currCreate = mpMapActors;
        for (int i = 0; i < MAX_AREAS; i++) {
            mMapActorsByArea[i] = currCreate;
            currCreate += mMapActorCountByArea[i];
            mMapActorIdxForArea[i] = sum;
            sum += mMapActorCountByArea[i];
        }
    }
}

sScrollData *dCdFile_c::getScrollDataP(u8 id) {
    sScrollData *curr = mpScrollData;
    for (u32 i = 0; i < mScrollDataCount; i++) {
        u8 currID = curr->mID;
        if (currID == id) {
            return curr;
        }
        curr++;
    }
    return mpScrollData;
}

sBgData *dCdFile_c::getBg2DataP(u8 id) {
    sBgData *curr = mpBg2Data;
    for (u32 i = 0; i < mBg2DataCount; i++) {
        if (curr->mID == id) {
            return curr;
        }
        curr++;
    }
    return nullptr;
}

sBgData *dCdFile_c::getBg3DataP(u8 id) {
    sBgData *curr = mpBg3Data;
    for (u32 i = 0; i < mBg3DataCount; i++) {
        if (curr->mID == id) {
            return curr;
        }
        curr++;
    }
    return nullptr;
}

sNextGotoData *dCdFile_c::getNextGotoP(u8 id) {
    sNextGotoData *curr = mpNextGotos;
    for (int i = 0; i < mNextGotoCount; i++) {
        if (curr->mID == id) {
            return curr;
        }
        curr++;
    }
    return nullptr;
}

sAreaData *dCdFile_c::getAreaDataP(u8 id, sRangeDataF *bound) {
    sAreaData *curr = mpAreas;
    for (u32 i = 0; i < mAreaCount; i++) {
        if (curr->mID == id) {
            if (bound != nullptr) {
                bound->mOffset.x = curr->mRangeData.mX;
                bound->mOffset.y = -curr->mRangeData.mY;
                bound->mSize.x = curr->mRangeData.mWidth;
                bound->mSize.y = curr->mRangeData.mHeight;
            }
            return curr;
        }
        curr++;
    }
    return nullptr;
}

u8 dCdFile_c::getAreaID(u8 id) {
    return getAreaDataP(id, nullptr)->mID;
}

int dCdFile_c::getAreaScroll(u8 id) {
    return getAreaDataP(id, nullptr)->mScroll;
}

int dCdFile_c::getAreaZoomType(u8 id) {
    return getAreaDataP(id, nullptr)->mZoomType;
}

int dCdFile_c::getAreaZoomID(u8 id) {
    return getAreaDataP(id, nullptr)->mZoomID;
}

int dCdFile_c::getAreaZoomChange(u8 id) {
    return getAreaDataP(id, nullptr)->mZoomChange;
}

int dCdFile_c::getAreaBgm(u8 id) {
    return getAreaDataP(id, nullptr)->mAreaBgm;
}

int dCdFile_c::getAreaBgmMode(u8 id) {
    return getAreaDataP(id, nullptr)->mBgmMode;
}

int dCdFile_c::getAreaMaskNo(u8 id) {
    return getAreaDataP(id, nullptr)->mMaskNo;
}

int dCdFile_c::getAreaBg2(u8 id) {
    return getAreaDataP(id, nullptr)->mBg2;
}

int dCdFile_c::getAreaBg3(u8 id) {
    return getAreaDataP(id, nullptr)->mBg3;
}

int dCdFile_c::getAreaColorOBJ(u8 id) {
    return getAreaDataP(id, nullptr)->mColorObj;
}

int dCdFile_c::getAreaColorBG(u8 id) {
    return getAreaDataP(id, nullptr)->mColorBG;
}

int dCdFile_c::getAreaDirection(u8 id) {
    return getAreaDataP(id, nullptr)->mDirection;
}

int dCdFile_c::getAreaSizeX(u8 id) {
    return getAreaDataP(id, nullptr)->mRangeData.mWidth;
}

u8 dCdFile_c::getAreaNo(mVec3_c *pos) {
    sAreaData *curr = mpAreas;
    for (u32 i = 0; i < mAreaCount; i++) {
        if (checkRange(pos, (sRangeData *) &curr->mRangeData)) {
            return curr->mID;
        }
        curr++;
    }
    return -1;
}

u8 dCdFile_c::getAreaNo(sRangePosSize *bound) {
    sAreaData *curr = mpAreas;
    for (u32 i = 0; i < mAreaCount; i++) {
        if (checkRange(bound, &curr->mRangeData)) {
            return curr->mID;
        }
        curr++;
    }
    return 0;
}

sRangeData *dCdFile_c::getRangeDataP(u8 id, sRangeDataF *bound) {
    sRangeData *curr = mpRangeData;
    for (u32 i = 0; i < mRangeCount; i++) {
        if (curr->mID == id) {
            if (bound != nullptr) {
                bound->mOffset.x = curr->mX;
                bound->mOffset.y = -curr->mY;
                bound->mSize.x = curr->mWidth;
                bound->mSize.y = curr->mHeight;
            }
            return curr;
        }
        curr++;
    }
    return nullptr;
}

bool dCdFile_c::checkRange(mVec3_c *pos, sRangeData *range) {
    int x = dScStage_c::getLoopPosX(pos->x);
    int y = -pos->y;
    if (x < range->mX - 128) {
        return false;
    }
    if (y < range->mY - 128) {
        return false;
    }
    if (x >= range->mX + range->mWidth + 128) {
        return false;
    }
    if (y >= range->mY + range->mHeight + 128) {
        return false;
    }
    return true;
}

bool dCdFile_c::checkRange(sRangePosSize *a, sRangePosSize *b) {
    short aCenterX = a->mX + a->mWidth / 2;
    short bCenterX = b->mX + b->mWidth / 2;
    short aCenterY = a->mY + a->mHeight / 2;
    short bCenterY = b->mY + b->mHeight / 2;

    short centerDistX = aCenterX - bCenterX;
    if (centerDistX < 0) {
        centerDistX = -centerDistX;
    }

    short centerDistY = aCenterY - bCenterY;
    if (centerDistY < 0) {
        centerDistY = -centerDistY;
    }

    short combinedHalfWidth = (a->mWidth + b->mWidth) / 2;
    short combinedHalfHeight = (a->mHeight + b->mHeight) / 2;

    if (centerDistX - (128 + combinedHalfWidth) < 0 && centerDistY - (128 + combinedHalfHeight) < 0) {
        return true;
    }
    return false;
}

sScrollAreaData *dCdFile_c::getScrlAreaDataP(u8 id) {
    if ((u32) mScrlAreaCount > id) {
        return &mpScrlAreas[id];
    }
    return nullptr;
}

void dCd_c::createInstance(EGG::Heap *heap) {
    EGG::Heap *prevHeap = mHeap::setCurrentHeap(heap);
    new dCd_c();
    mHeap::setCurrentHeap(prevHeap);
}

void dCd_c::deleteInstance() {
    if (m_instance != nullptr) {
        delete m_instance;
    }
}

void dCd_c::loadCourseData() {
    for (int i = 0; i < COURSE_FILE_COUNT; i++) {
        mFiles[i].loadCourseData(i);
    }
}

void dCd_c::setCourseArcName(int world, int course, char *buffer) {
    sprintf(buffer, "%02d-%02d", world + 1, course + 1);
}
