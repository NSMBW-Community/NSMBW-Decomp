#include <cstdlib>
#include <game/bases/d_actorcreate_manager.hpp>
#include <game/bases/d_actor_group_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_profile.hpp>
#include <game/bases/d_bg_parameter.hpp>

dActorCreateMng_c *dActorCreateMng_c::m_instance;
const ProfileName dActorCreateMng_c::l_GoAheadEnemy[] = {
    fProfile::EN_PUKUCOIN,
    fProfile::EN_KARON,
    fProfile::EN_BIGKARON,
    fProfile::EN_KARON_HEAD,
    fProfile::WAKI_KERONPA,
    fProfile::PROFILE_NULL
};

void dActorCreateMng_c::ActorCreateInfoClear() {
    mYoshiColor = 0;
    mIsEndingDemo = false;
    mIsGoalPoleDemo = false;
    mZPosCount = 0;
    mZPosCountLayer2 = 0;
    mMapObjZPosCount = 0;
    mMapObjZPosCountLayer2 = 0;
}

bool dActorCreateMng_c::ScroolAreaInCheck(int boundsEdge, int areaEdge, int boundMin, int boundMax, int areaMin, int areaMax) {
    if (boundsEdge == areaEdge) {
        return boundMax >= areaMin && boundMin <= areaMax;
    }
    return false;
}

bool dActorCreateMng_c::ScroolAreaInLoopCheck(int boundsEdge, int areaEdge, int boundMin, int boundMax, int areaMin, int areaMax) {
    return ScroolAreaInCheck(boundsEdge, areaEdge, boundMin, boundMax, areaMin, areaMax);
}

void dActorCreateMng_c::setMapActorCreate() {
    for (int i = 0; i < MAX_MAP_ACTOR_COUNT; i++) {
        mSpawnFlags[i] = 0;
        mDeleteVals[i] = 0;
    }

    if (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_0) {
        return;
    }

    mScreenLeftPos = 0;
    mScreenTopPos = 0;
    mPrevScrollBoundaryTileX = 0;
    mPrevScrollBoundaryTileY = 0;
    MapActorInital_set();
}

void dActorCreateMng_c::setMapActorCreate_next() {
    if (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_0) {
        return;
    }

    if (mIsEndingDemo) {
        return;
    }

    for (int i = 0; i < MAX_MAP_ACTOR_COUNT; i++) {
        mSpawnFlags[i] &= ~dActor_c::ACTOR_SPAWNED;
    }

    mScreenLeftPos = 0;
    mScreenTopPos = 0;
    mPrevScrollBoundaryTileX = 0;
    mPrevScrollBoundaryTileY = 0;
    MapActorInital_next();
}

void dActorCreateMng_c::MapActorInital_set() {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    MapActorInitialCreate(
        file->mpMapActors,
        mDeleteVals,
        mSpawnFlags,
        file->mMapActorCount,
        false
    );
}

void dActorCreateMng_c::MapActorInital_next() {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    u8 areaNo = dScStage_c::m_instance->getCurrArea();
    MapActorInitialCreate(
        file->mMapActorsByArea[areaNo],
        &mDeleteVals[file->mMapActorIdxForArea[areaNo]],
        &mSpawnFlags[file->mMapActorIdxForArea[areaNo]],
        file->mMapActorCountByArea[areaNo],
        true
    );
}

bool dActorCreateMng_c::GlobalActorCheck(sMapActorData *data) {
    ProfileName i = 0;
    while (l_GoAheadEnemy[i] != ARRAY_LAST(l_GoAheadEnemy)) {
        if (data->mMapActorID == l_GoAheadEnemy[i]) {
            break;
        }
        i++;
    }

    if (l_GoAheadEnemy[i] != ARRAY_LAST(l_GoAheadEnemy)) {
        return true;
    }

    return false;
}

void dActorCreateMng_c::processGroupId(sMapActorData *data, u8 file, u32 mapActorIdx) {
    dActorGroupIdMng_c::m_instance->process(data, GROUP_ID(data->mParam), file, mapActorIdx);
}

bool dActorCreateMng_c::GroupIdCheck(ulong param) {
    return GROUP_ID(param) == 0;
}

void dActorCreateMng_c::MapActorInitialCreate(sMapActorData *levelData, u16 *deleteVal, u8 *spawnFlags, int mapActorCount, bool isAreaReload) {
    if (mapActorCount == 0 || levelData == nullptr) {
        return;
    }

    dScStage_c *stage = dScStage_c::m_instance;
    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;

    float screenPosX = bgParam->pos().x;
    float screenPosY = bgParam->pos().y;
    float screenWidth = bgParam->size().x;
    float screenHeight = bgParam->size().y;

    int screenLeft = screenPosX;
    int screenTop = -(int) screenPosY;

    int boundTop = screenTop - 32;
    int boundBottom = screenTop + (int) screenHeight + 48;
    int boundLeft = screenLeft - 48;
    int boundRight = screenLeft + (int) screenWidth + 64;

    int boundHalfHeight = (boundBottom - boundTop) >> 1;
    int boundHalfWidth = (boundRight - boundLeft) >> 1;
    int boundCenterY = boundTop + boundHalfHeight;
    int boundCenterX = boundLeft + boundHalfWidth;

    int actorCenterX, actorCenterY, actorHalfWidth, actorHalfHeight;

    u32 i = 0;
    dCdFile_c *file = dCd_c::m_instance->getFileP(stage->mCurrFile);
    u8 areaNo = dScStage_c::m_instance->getCurrArea();

    while (i != mapActorCount) {
        if ((*spawnFlags & dActor_c::ACTOR_SPAWNED) == 0) {
            dActorData_c *mapActorInfo = dActorData_c::getMapActorInfo(levelData->mMapActorID);

            bool globalActor = false;
            bool canSpawn = false;

            if (!isAreaReload) {
                bool groupedActor = false;

                // If the actor can be grouped and a group ID is defined, process the group
                if (mapActorInfo->mFlags & ACTOR_CREATE_GROUPABLE) {
                    if (!GroupIdCheck(levelData->mParam)) {
                        processGroupId(levelData, stage->mCurrFile, i);
                        groupedActor = true;
                    }
                } else if (GlobalActorCheck(levelData)) {
                    globalActor = true;
                }

                // Do not process grouped actors and let the Group Controller deal with it
                // Otherwise, verify the actor's spawn range is within the zone boundaries
                if (!groupedActor) {
                    sRangePosSize bound = {
                        levelData->mX,
                        levelData->mY,
                        (u16) (mapActorInfo->mSpawnRangeHalfWidth * 2),
                        (u16) (mapActorInfo->mSpawnRangeHalfHeight * 2)
                    };
                    if (file->getAreaNo(&bound) == areaNo) {
                        canSpawn = true;
                    }
                }

            // On zone reload, spawn all non-grouped actors
            } else {
                if (mapActorInfo->mFlags & ACTOR_CREATE_GROUPABLE) {
                    if (GroupIdCheck(levelData->mParam)) {
                        canSpawn = true;
                    }
                } else {
                    canSpawn = true;
                }
            }

            bool spawnActor = globalActor;
            if (!globalActor && canSpawn) {
                if (mapActorInfo->mFlags & ACTOR_CREATE_GLOBAL) {
                    spawnActor = true;
                } else {
                    // If the distance between the boundary center and the actor center is less than their combined half-widths,
                    // the two boxes are touching, and the actor can be spawned
                    actorHalfWidth = mapActorInfo->mSpawnRangeHalfWidth + ((mapActorInfo->mSpawnMarginLeft + mapActorInfo->mSpawnMarginRight) >> 1);
                    actorHalfHeight = mapActorInfo->mSpawnRangeHalfHeight + ((mapActorInfo->mSpawnMarginTop + mapActorInfo->mSpawnMarginBottom) >> 1);
                    actorCenterX = levelData->mX + mapActorInfo->mSpawnRangeOffsetX;
                    actorCenterY = levelData->mY - mapActorInfo->mSpawnRangeOffsetY;
                    if (
                        abs(boundCenterX - actorCenterX) <= abs(boundHalfWidth + actorHalfWidth) &&
                        abs(boundCenterY - actorCenterY) <= abs(boundHalfHeight + actorHalfHeight)
                    ) {
                        spawnActor = true;
                    }
                }
            }

            if (spawnActor) {
                mapActorSpawn(levelData, mapActorInfo, spawnFlags, deleteVal, areaNo);
            }
        }

        levelData++;
        spawnFlags++;
        deleteVal++;
        i++;
    }
}

void dActorCreateMng_c::MapActorScroolCreateCheck() {
    if (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_0) {
        return;
    }

    if (mIsGoalPoleDemo == true) {
        return;
    }

    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;

    sMapActorCreateBounds::BOUND_SCROLL_DIR_X_e scrollDirX = sMapActorCreateBounds::BOUND_SCROLL_X_LEFT;
    sMapActorCreateBounds::BOUND_SCROLL_DIR_Y_e scrollDirY = sMapActorCreateBounds::BOUND_SCROLL_Y_UP;

    float screenPosX = bgParam->pos().x;
    float screenPosY = bgParam->pos().y;
    float screenWidth = bgParam->size().x;
    float screenHeight = bgParam->size().y;

    int screenLeft = screenPosX;
    int screenTop = -(int) screenPosY;

    int tileLeft = (screenLeft - 32) >> 4;
    int tileRight = (screenLeft + (int) screenWidth + 48) >> 4;
    int tileTop = (screenTop - 16) >> 4;
    int tileBottom = (screenTop + (int) screenHeight + 32) >> 4;

    sMapActorCreateBounds createBounds;

    int scrollBoundaryTileX, scrollBoundaryTileY;

    if (dBg_c::m_bg_p->getDispScale() != dBg_c::m_bg_p->getPrevDispScale()) {
        createBounds.mScrollBoundaryTileX = tileLeft;
        createBounds.mScrollBoundaryTileY = tileBottom;
        createBounds.mTileLeft = tileLeft;
        createBounds.mTileRight = tileRight;
        createBounds.mTileTop = tileTop;
        createBounds.mTileBottom = tileBottom;
        createBounds.mScrollDirX = sMapActorCreateBounds::BOUND_SCROLL_X_LEFT;
        createBounds.mScrollDirY = sMapActorCreateBounds::BOUND_SCROLL_Y_DOWN;
        MapActorScrollCreate(&createBounds, false);

        // [Possible optimization: MapActorScrollCreate can be called with scroll directions
        // X_RIGHT and Y_UP, making the third call redundant as each axis is handled individually]
        createBounds.mScrollBoundaryTileX = tileRight;
        createBounds.mScrollBoundaryTileY = tileBottom;
        createBounds.mTileLeft = tileLeft;
        createBounds.mTileRight = tileRight;
        createBounds.mTileTop = tileTop;
        createBounds.mTileBottom = tileBottom;
        createBounds.mScrollDirX = sMapActorCreateBounds::BOUND_SCROLL_X_RIGHT;
        createBounds.mScrollDirY = sMapActorCreateBounds::BOUND_SCROLL_Y_DOWN;
        MapActorScrollCreate(&createBounds, false);

        mScreenLeftPos = screenLeft; // [Unnecessary assignment]
        mScreenTopPos = screenTop; // [Unnecessary assignment]
        scrollBoundaryTileX = tileRight;
        scrollBoundaryTileY = tileTop;

        createBounds.mScrollBoundaryTileX = tileRight;
        createBounds.mScrollBoundaryTileY = tileTop;
        createBounds.mScrollDirX = sMapActorCreateBounds::BOUND_SCROLL_X_RIGHT;
        createBounds.mScrollDirY = sMapActorCreateBounds::BOUND_SCROLL_Y_UP;
        MapActorScrollCreate(&createBounds, false);
    } else {
        int bgScrollDirY = bgParam->mScrollDirY;
        int bgScrollDirX = bgParam->mScrollDirX;

        if (bgScrollDirX != dBgParameter_c::BG_SCROLL_X_NONE) {
            if (bgScrollDirX == dBgParameter_c::BG_SCROLL_X_RIGHT) {
                // Right
                scrollBoundaryTileX = tileRight;
                scrollDirX = sMapActorCreateBounds::BOUND_SCROLL_X_RIGHT;
            } else {
                // Left
                scrollBoundaryTileX = tileLeft;
            }
        } else {
            // None
            scrollBoundaryTileX = mPrevScrollBoundaryTileY; // [Bug? Should be mPrevScrollBoundaryTileX]
            scrollDirX = sMapActorCreateBounds::BOUND_SCROLL_X_NONE;
        }

        if (bgScrollDirY != dBgParameter_c::BG_SCROLL_Y_NONE) {
            if (bgScrollDirY == (u32)dBgParameter_c::BG_SCROLL_Y_DOWN) {
                // Down
                scrollBoundaryTileY = tileBottom;
                scrollDirY = sMapActorCreateBounds::BOUND_SCROLL_Y_DOWN;
            } else {
                // Up
                scrollBoundaryTileY = tileTop;
            }
        } else {
            // None
            scrollBoundaryTileY = mPrevScrollBoundaryTileY;
            scrollDirY = sMapActorCreateBounds::BOUND_SCROLL_Y_NONE;
        }

        // Only spawn actors if the camera has moved from the previous frame
        if (mPrevScrollBoundaryTileX != scrollBoundaryTileX || mPrevScrollBoundaryTileY != scrollBoundaryTileY) {
            createBounds.mScrollBoundaryTileX = scrollBoundaryTileX;
            createBounds.mScrollBoundaryTileY = scrollBoundaryTileY;
            createBounds.mTileLeft = tileLeft;
            createBounds.mTileRight = tileRight;
            createBounds.mTileTop = tileTop;
            createBounds.mTileBottom = tileBottom;
            createBounds.mScrollDirX = scrollDirX;
            createBounds.mScrollDirY = scrollDirY;
            MapActorScrollCreate(&createBounds, true);
        }
    }

    mScreenLeftPos = screenLeft;
    mScreenTopPos = screenTop;
    mPrevScrollBoundaryTileX = scrollBoundaryTileX;
    mPrevScrollBoundaryTileY = scrollBoundaryTileY;
}

void dActorCreateMng_c::MapActorScrollCreate(sMapActorCreateBounds *createBounds, int isScreenScroll) {
    dScStage_c *stage = dScStage_c::m_instance;
    u8 areaNo = stage->mCurrArea;
    dCdFile_c *file = dCd_c::m_instance->getFileP(stage->mCurrFile);

    int mapActorCount = file->mMapActorCountByArea[areaNo];
    if (mapActorCount == 0) {
        return;
    }

    sMapActorData *data = file->mMapActorsByArea[areaNo];
    if (data == nullptr) {
        return;
    }

    u16 *deleteVal = &mDeleteVals[file->mMapActorIdxForArea[areaNo]];
    u8 *spawnFlags = &mSpawnFlags[file->mMapActorIdxForArea[areaNo]];

    bool spawnActor;
    int actorCenterX, actorCenterY;
    int scrollBoundaryTileX, actorTileLeft, actorTileRight;
    int scrollBoundaryTileY, actorTileTop, actorTileBottom;

    int createBoundaryTileX = createBounds->mScrollBoundaryTileX;
    int createBoundaryTileY = createBounds->mScrollBoundaryTileY;
    int boundsLeft = createBounds->mTileLeft;
    int boundsRight = createBounds->mTileRight;
    int boundsTop = createBounds->mTileTop;
    int boundsBottom = createBounds->mTileBottom;
    sMapActorCreateBounds::BOUND_SCROLL_DIR_X_e scrollDirX = createBounds->mScrollDirX;
    sMapActorCreateBounds::BOUND_SCROLL_DIR_Y_e scrollDirY = createBounds->mScrollDirY;

    for (u32 i = 0; i != mapActorCount; i++) {
        if ((*spawnFlags & dActor_c::ACTOR_SPAWNED) == 0) {
            dActorData_c *mapActorInfo = dActorData_c::getMapActorInfo(data->mMapActorID);
            spawnActor = false;
            actorCenterX = data->mX + mapActorInfo->mSpawnRangeOffsetX;
            actorCenterY = data->mY - mapActorInfo->mSpawnRangeOffsetY;
            actorTileLeft = (actorCenterX - mapActorInfo->mSpawnRangeHalfWidth - mapActorInfo->mSpawnMarginLeft) >> 4;
            actorTileRight = (actorCenterX + mapActorInfo->mSpawnRangeHalfWidth + mapActorInfo->mSpawnMarginRight) >> 4;
            actorTileTop = (actorCenterY - mapActorInfo->mSpawnRangeHalfHeight - mapActorInfo->mSpawnMarginTop) >> 4;
            actorTileBottom = (actorCenterY + mapActorInfo->mSpawnRangeHalfHeight + mapActorInfo->mSpawnMarginBottom) >> 4;
            scrollBoundaryTileX = actorTileRight;
            if (scrollDirX != sMapActorCreateBounds::BOUND_SCROLL_X_LEFT) {
                scrollBoundaryTileX = actorTileLeft;
            }
            scrollBoundaryTileY = actorTileBottom;
            if (scrollDirY != sMapActorCreateBounds::BOUND_SCROLL_Y_UP) {
                scrollBoundaryTileY = actorTileTop;
            }

            bool groupedActor = false;
            if (mapActorInfo->mFlags & ACTOR_CREATE_GROUPABLE) {
                if (!GroupIdCheck(data->mParam)) {
                    groupedActor = true;
                }
            }

            if (!groupedActor) {
                if (isScreenScroll) {
                    if (scrollBoundaryTileX == createBoundaryTileX) {
                        if (scrollDirX != sMapActorCreateBounds::BOUND_SCROLL_X_NONE) {
                            if (scrollDirX < 0) {
                                spawnActor = ScroolAreaInCheck(boundsRight, actorTileLeft, boundsTop, boundsBottom, actorTileTop, actorTileBottom);
                            } else {
                                spawnActor = ScroolAreaInCheck(boundsLeft, actorTileRight, boundsTop, boundsBottom, actorTileTop, actorTileBottom);
                            }
                        }
                    } else if (scrollBoundaryTileY == createBoundaryTileY) {
                        if (scrollDirY != sMapActorCreateBounds::BOUND_SCROLL_Y_NONE) {
                            if (scrollDirY < 0) {
                                spawnActor = ScroolAreaInLoopCheck(boundsBottom, actorTileTop, boundsLeft, boundsRight, actorTileLeft, actorTileRight);
                            } else {
                                spawnActor = ScroolAreaInLoopCheck(boundsTop, actorTileBottom, boundsLeft, boundsRight, actorTileLeft, actorTileRight);
                            }
                        }
                    }
                } else {
                    if (ScroolAreaInCheck(createBoundaryTileX, scrollBoundaryTileX, boundsTop, boundsBottom, actorTileTop, actorTileBottom)) {
                        spawnActor = true;
                    }
                    if (ScroolAreaInCheck(createBoundaryTileY, scrollBoundaryTileY, boundsLeft, boundsRight, actorTileLeft, actorTileRight)) {
                        spawnActor = true;
                    }
                }
            }

            if (spawnActor) {
                mapActorSpawn(data, mapActorInfo, spawnFlags, deleteVal, areaNo);
            }
        }

        data++;
        deleteVal++;
        spawnFlags++;
    }
}

void dActorCreateMng_c::incZposCount() {
    mZPosCount++;
    if (mZPosCount >= Z_POS_COUNT) {
        mZPosCount = 0;
    }
}

float dActorCreateMng_c::addZposCount() {
    float res = 1900.0f - mZPosCount * 16.0f;
    incZposCount();
    return res;
}

void dActorCreateMng_c::incZposCount_layer2() {
    mZPosCountLayer2++;
    if (mZPosCountLayer2 >= Z_POS_COUNT) {
        mZPosCountLayer2 = 0;
    }
}

float dActorCreateMng_c::addZposCount_layer2() {
    float res = -2500.0f + mZPosCountLayer2 * 16.0f;
    incZposCount_layer2();
    return res;
}

void dActorCreateMng_c::incMapObjZposCount() {
    mMapObjZPosCount++;
    if (mMapObjZPosCount >= Z_POS_COUNT) {
        mMapObjZPosCount = 0;
    }
}

float dActorCreateMng_c::addMapObjZposCount() {
    float res = 400.0f - mMapObjZPosCount * 8.0f;
    incMapObjZposCount();
    return res;
}

void dActorCreateMng_c::incMapObjZposCount_layer2() {
    mMapObjZPosCountLayer2++;
    if (mMapObjZPosCountLayer2 >= Z_POS_COUNT) {
        mMapObjZPosCountLayer2 = 0;
    }
}

float dActorCreateMng_c::addMapObjZposCount_layer2() {
    float res = -3100.0f - mMapObjZPosCountLayer2 * 8.0f;
    incMapObjZposCount_layer2();
    return res;
}

dActor_c *dActorCreateMng_c::mapActorSpawn(sMapActorData *data, dActorData_c *info, u8 *spawnFlags, u16 *deleteVal, u8 areaNo) {
    if (data->getProfileName() == fProfile::DUMMY_ACTOR) {
        return nullptr;
    }

    float zPos;
    if (info->mFlags & ACTOR_CREATE_MAPOBJ) {
        if (data->mLayer != LAYER_1) {
            zPos = addMapObjZposCount_layer2();
        } else {
            zPos = addMapObjZposCount();
        }
    } else {
        if (data->mLayer != LAYER_1) {
            zPos = addZposCount_layer2();
        } else {
            zPos = addZposCount();
        }
    }
    mVec3_c pos(data->mX + info->mSpawnOffsetX, -(data->mY - info->mSpawnOffsetY), zPos);

    dActor_c::m_read_p_keep = spawnFlags;
    *(u16 *) dActor_c::m_flag_keep = *(u16 *) data->mEventNums; // [Works in MWCC without the casts too]
    dActor_c::m_mbgchoice_keep = data->mLayer;

    u8 eventId1 = dActor_c::m_flag_keep[1];
    u8 eventId2 = dActor_c::m_flag_keep[0];
    dActor_c::m_flagbit_keep = 0;
    if (eventId1 != 0) {
        dActor_c::m_flagbit_keep = 1LL << (eventId1 - 1);
    }
    if (eventId2 != 0) {
        dActor_c::m_flagbit_keep = 1LL << (eventId2 - 1);
    }

    dActor_c *actor = dActor_c::construct(data->getProfileName(), data->mParam, &pos, nullptr, data->mLayer);
    if (actor != nullptr) {
        if (data->mMapActorID >= 10) {
            actor->mAreaNo = areaNo;
            actor->mpSpawnFlags = spawnFlags;
            actor->mpDeleteVal = deleteVal;
            *(u16 *) actor->mEventNums = *(u16 *) dActor_c::m_flag_keep;
            actor->mEventMask = dActor_c::m_flagbit_keep;
            actor->mBlockHit = false;
            actor->mSpriteSpawnFlags = info->mFlags;
            // [Couldn't get this to match in a better way...]
            float areaX = actor->mVisibleAreaSize.x;
            if (!areaX) {
                float areaY = actor->mVisibleAreaSize.y;
                if (!areaY) {
                    actor->mVisibleAreaSize.set(info->mSpawnRangeHalfWidth * 2.0f, info->mSpawnRangeHalfHeight * 2.0f);
                }
            }
        }
        *spawnFlags |= dActor_c::ACTOR_SPAWNED;
    }
    return actor;
}
