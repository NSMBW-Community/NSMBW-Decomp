#include <cstdlib>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_actor_group_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_profile.hpp>
#include <game/bases/d_bg_parameter.hpp>

dActorCreateMng_c *dActorCreateMng_c::m_instance;

void dActorCreateMng_c::ActorCreateInfoClear() {
    mYoshiColor = 0;
    mNoSpawnOnReload = false;
    mNoSpawnOnScroll = 0;
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
        mDeleteNums[i] = 0;
    }
    if (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_0) {
        return;
    }
    mPosX = 0;
    mPosY = 0;
    mPrevEdgeTileX = 0;
    mPrevEdgeTileY = 0;
    MapActorInital_set();
}

void dActorCreateMng_c::setMapActorCreate_next() {
    if (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_0) {
        return;
    }
    if (mNoSpawnOnReload) {
        return;
    }

    for (int i = 0; i < MAX_MAP_ACTOR_COUNT; i++) {
        mSpawnFlags[i] &= ~dActor_c::ACTOR_SPAWNED;
    }
    mPosX = 0;
    mPosY = 0;
    mPrevEdgeTileX = 0;
    mPrevEdgeTileY = 0;
    MapActorInital_next();
}

void dActorCreateMng_c::MapActorInital_set() {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    MapActorInitialCreate(
        file->mpMapActors,
        mDeleteNums,
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
        &mDeleteNums[file->mMapActorIdxForArea[areaNo]],
        &mSpawnFlags[file->mMapActorIdxForArea[areaNo]],
        file->mMapActorCountByArea[areaNo],
        true
    );
}

bool dActorCreateMng_c::GlobalActorCheck(sMapActorData *data) {
    /// @unofficial
    static const ProfileName sc_globalProfiles[] = {
        fProfile::EN_PUKUCOIN,
        fProfile::EN_KARON,
        fProfile::EN_BIGKARON,
        fProfile::EN_KARON_HEAD,
        fProfile::WAKI_KERONPA,
        0xffff
    };
    u16 curr = 0;
    while (sc_globalProfiles[curr] != 0xffff) {
        if (data->mMapActorID == sc_globalProfiles[curr]) {
            break;
        }
        curr++;
    }
    if (sc_globalProfiles[curr] != 0xffff) {
        return true;
    }
    return false;
}

void dActorCreateMng_c::processGroupId(sMapActorData *data, u8 file, u32 idx) {
    dActorGroupIdMng_c::m_instance->process(data, GROUP_ID(data->mParam), file, idx);
}

bool dActorCreateMng_c::GroupIdCheck(ulong param) {
    return GROUP_ID(param) == 0;
}

void dActorCreateMng_c::MapActorInitialCreate(sMapActorData *data, u16 *deleteNum, u8 *spawnFlags, int count, bool isAreaReload) {
    if (count == 0 || data == nullptr) {
        return;
    }

    dScStage_c *stage = dScStage_c::m_instance;
    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;

    float bgPosX = bgParam->pos().x;
    float bgPosY = bgParam->pos().y;
    float bgSizeX = bgParam->size().x;
    float bgSizeY = bgParam->size().y;

    int boundX = bgPosX;
    int boundY = -(int) bgPosY;

    int top = boundY - 32;
    int bottom = boundY + (int) bgSizeY + 48;
    int left = boundX - 48;
    int right = boundX + (int) bgSizeX + 64;

    int boundHeight = (bottom - top) >> 1;
    int boundWidth = (right - left) >> 1;
    int boundCenterY = top + boundHeight;
    int boundCenterX = left + boundWidth;

    int actorCenterX, actorCenterY, actorWidth, actorHeight;

    u32 i = 0;
    dCdFile_c *file = dCd_c::m_instance->getFileP(stage->mCurrFile);
    u8 areaNo = dScStage_c::m_instance->getCurrArea();

    while (i != count) {
        if ((*spawnFlags & dActor_c::ACTOR_SPAWNED) == 0) {
            sMapActorInfo *mapActorInfo = dActorCreate::getMapActorInfo(data->mMapActorID);

            bool globalActor = false;
            bool canSpawn = false;
            if (!isAreaReload) {
                bool invalidGroupId = false;
                if (mapActorInfo->mFlags & ACTOR_CREATE_GROUP_0) {
                    if (!GroupIdCheck(data->mParam)) {
                        processGroupId(data, stage->mCurrFile, i);
                        invalidGroupId = true;
                    }
                } else if (GlobalActorCheck(data)) {
                    globalActor = true;
                }

                if (!invalidGroupId) {
                    sRangePosSize bound = {
                        data->mX,
                        data->mY,
                        (u16) (mapActorInfo->mSpawnRangeWidth * 2),
                        (u16) (mapActorInfo->mSpawnRangeHeight * 2)
                    };
                    if (file->getAreaNo(&bound) == areaNo) {
                        canSpawn = true;
                    }
                }
            } else {
                if (mapActorInfo->mFlags & ACTOR_CREATE_GROUP_0) {
                    if (GroupIdCheck(data->mParam)) {
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
                    actorWidth = mapActorInfo->mSpawnRangeWidth + ((mapActorInfo->mSpawnMarginLeft + mapActorInfo->mSpawnMarginRight) >> 1);
                    actorHeight = mapActorInfo->mSpawnRangeHeight + ((mapActorInfo->mSpawnMarginTop + mapActorInfo->mSpawnMarginBottom) >> 1);
                    actorCenterX = data->mX + mapActorInfo->mSpawnRangeOffsetX;
                    actorCenterY = data->mY - mapActorInfo->mSpawnRangeOffsetY;
                    if (
                        abs(boundCenterX - actorCenterX) <= abs(boundWidth + actorWidth) &&
                        abs(boundCenterY - actorCenterY) <= abs(boundHeight + actorHeight)
                    ) {
                        spawnActor = true;
                    }
                }
            }

            if (spawnActor) {
                mapActorSpawn(data, mapActorInfo, spawnFlags, deleteNum, areaNo);
            }
        }
        data++;
        spawnFlags++;
        deleteNum++;
        i++;
    }
}

void dActorCreateMng_c::MapActorScroolCreateCheck() {
    if (dInfo_c::mGameFlag & dInfo_c::GAME_FLAG_0) {
        return;
    }
    if (mNoSpawnOnScroll == true) {
        return;
    }

    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;

    SCROLL_DIR_X_e scrollDirX = SCROLL_X_LEFT;
    SCROLL_DIR_Y_e scrollDirY = SCROLL_Y_UP;

    float bgPosX = bgParam->pos().x;
    float bgPosY = bgParam->pos().y;
    float bgSizeX = bgParam->size().x;
    float bgSizeY = bgParam->size().y;

    int boundX = bgPosX;
    int boundY = -(int) bgPosY;

    int tileLeft = (boundX - 32) >> 4;
    int tileRight = (boundX + (int) bgSizeX + 48) >> 4;
    int tileTop = (boundY - 16) >> 4;
    int tileBottom = (boundY + (int) bgSizeY + 32) >> 4;

    sMapActorCreateBounds createBounds;

    int edgeTileX, edgeTileY;

    if (dBg_c::m_bg_p->getDispScale() != dBg_c::m_bg_p->getPrevDispScale()) {
        createBounds.mEdgeTileX = tileLeft;
        createBounds.mEdgeTileY = tileBottom;
        createBounds.mTileLeft = tileLeft;
        createBounds.mTileRight = tileRight;
        createBounds.mTileTop = tileTop;
        createBounds.mTileBottom = tileBottom;
        createBounds.mScrollDirX = SCROLL_X_LEFT;
        createBounds.mScrollDirY = SCROLL_Y_DOWN;
        MapActorScrollCreate(&createBounds, false);

        createBounds.mEdgeTileX = tileRight;
        createBounds.mEdgeTileY = tileBottom;
        createBounds.mTileLeft = tileLeft;
        createBounds.mTileRight = tileRight;
        createBounds.mTileTop = tileTop;
        createBounds.mTileBottom = tileBottom;
        createBounds.mScrollDirX = SCROLL_X_RIGHT;
        createBounds.mScrollDirY = SCROLL_Y_DOWN;
        MapActorScrollCreate(&createBounds, false);

        mPosX = boundX;
        mPosY = boundY;
        edgeTileX = tileRight;
        edgeTileY = tileTop;

        createBounds.mEdgeTileX = tileRight;
        createBounds.mEdgeTileY = tileTop;
        createBounds.mScrollDirX = SCROLL_X_RIGHT;
        createBounds.mScrollDirY = SCROLL_Y_UP;
        MapActorScrollCreate(&createBounds, false);
    } else {
        int bgScrollDirY = bgParam->mScrollDirY;
        int bgScrollDirX = bgParam->mScrollDirX;

        if (bgScrollDirX != 0) {
            if (bgScrollDirX == 2) {
                // right
                edgeTileX = tileRight;
                scrollDirX = SCROLL_X_RIGHT;
            } else {
                // left
                edgeTileX = tileLeft;
            }
        } else {
            // none
            edgeTileX = mPrevEdgeTileY; // [Bug? Should be mTileX]
            scrollDirX = SCROLL_X_NONE;
        }

        if (bgScrollDirY != 0) {
            if (bgScrollDirY == 1U) {
                // down
                edgeTileY = tileBottom;
                scrollDirY = SCROLL_Y_DOWN;
            } else {
                // up
                edgeTileY = tileTop;
            }
        } else {
            // none
            edgeTileY = mPrevEdgeTileY;
            scrollDirY = SCROLL_Y_NONE;
        }

        // Only if the camera actually moved
        if (mPrevEdgeTileX != edgeTileX || mPrevEdgeTileY != edgeTileY) {
            createBounds.mEdgeTileX = edgeTileX;
            createBounds.mEdgeTileY = edgeTileY;
            createBounds.mTileLeft = tileLeft;
            createBounds.mTileRight = tileRight;
            createBounds.mTileTop = tileTop;
            createBounds.mTileBottom = tileBottom;
            createBounds.mScrollDirX = scrollDirX;
            createBounds.mScrollDirY = scrollDirY;
            MapActorScrollCreate(&createBounds, true);
        }
    }
    mPosX = boundX;
    mPosY = boundY;
    mPrevEdgeTileX = edgeTileX;
    mPrevEdgeTileY = edgeTileY;
}

void dActorCreateMng_c::MapActorScrollCreate(sMapActorCreateBounds *createBounds, int noDispScaleChange) {
    dScStage_c *stage = dScStage_c::m_instance;
    u8 areaNo = stage->mCurrArea;
    dCdFile_c *file = dCd_c::m_instance->getFileP(stage->mCurrFile);

    int mapActorCount = file->mMapActorCountByArea[areaNo];
    if (mapActorCount == 0) {
        return;
    }

    sMapActorInfo *curr;
    sMapActorData *data = file->mMapActorsByArea[areaNo];
    if (data == nullptr) {
        return;
    }

    u16 *deleteNum = &mDeleteNums[file->mMapActorIdxForArea[areaNo]];
    u8 *spawnFlags = &mSpawnFlags[file->mMapActorIdxForArea[areaNo]];

    bool spawnActor;
    int centerX, centerY;
    int edgeTileX, tileL, tileR;
    int edgeTileY, tileT, tileB;

    int boundsEdgeX = createBounds->mEdgeTileX;
    int boundsEdgeY = createBounds->mEdgeTileY;
    int boundsLeft = createBounds->mTileLeft;
    int boundsRight = createBounds->mTileRight;
    int boundsTop = createBounds->mTileTop;
    int boundsBottom = createBounds->mTileBottom;
    SCROLL_DIR_X_e dirX = createBounds->mScrollDirX;
    SCROLL_DIR_Y_e dirY = createBounds->mScrollDirY;

    for (u32 i = 0; i != mapActorCount; i++) {
        if ((*spawnFlags & dActor_c::ACTOR_SPAWNED) == 0) {
            curr = dActorCreate::getMapActorInfo(data->mMapActorID);
            spawnActor = false;
            centerX = data->mX + curr->mSpawnRangeOffsetX;
            centerY = data->mY - curr->mSpawnRangeOffsetY;
            tileL = (centerX - curr->mSpawnRangeWidth - curr->mSpawnMarginLeft) >> 4;
            tileR = (centerX + curr->mSpawnRangeWidth + curr->mSpawnMarginRight) >> 4;
            tileT = (centerY - curr->mSpawnRangeHeight - curr->mSpawnMarginTop) >> 4;
            tileB = (centerY + curr->mSpawnRangeHeight + curr->mSpawnMarginBottom) >> 4;
            edgeTileX = tileR;
            if (dirX != SCROLL_X_LEFT) {
                edgeTileX = tileL;
            }
            edgeTileY = tileB;
            if (dirY != SCROLL_Y_UP) {
                edgeTileY = tileT;
            }

            bool wrongGroupID = false;
            if (curr->mFlags & ACTOR_CREATE_GROUP_0) {
                if (!GroupIdCheck(data->mParam)) {
                    wrongGroupID = true;
                }
            }

            if (!wrongGroupID) {
                if (noDispScaleChange) {
                    if (edgeTileX == boundsEdgeX) {
                        if (dirX != SCROLL_X_NONE) {
                            if (dirX < 0) {
                                spawnActor = ScroolAreaInCheck(boundsRight, tileL, boundsTop, boundsBottom, tileT, tileB);
                            } else {
                                spawnActor = ScroolAreaInCheck(boundsLeft, tileR, boundsTop, boundsBottom, tileT, tileB);
                            }
                        }
                    } else if (edgeTileY == boundsEdgeY) {
                        if (dirY != SCROLL_Y_NONE) {
                            if (dirY < 0) {
                                spawnActor = ScroolAreaInLoopCheck(boundsBottom, tileT, boundsLeft, boundsRight, tileL, tileR);
                            } else {
                                spawnActor = ScroolAreaInLoopCheck(boundsTop, tileB, boundsLeft, boundsRight, tileL, tileR);
                            }
                        }
                    }
                } else {
                    if (ScroolAreaInCheck(boundsEdgeX, edgeTileX, boundsTop, boundsBottom, tileT, tileB)) {
                        spawnActor = true;
                    }
                    if (ScroolAreaInCheck(boundsEdgeY, edgeTileY, boundsLeft, boundsRight, tileL, tileR)) {
                        spawnActor = true;
                    }
                }
            }

            if (spawnActor) {
                mapActorSpawn(data, curr, spawnFlags, deleteNum, areaNo);
            }
        }
        data++;
        deleteNum++;
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

dActor_c *dActorCreateMng_c::mapActorSpawn(sMapActorData *data, sMapActorInfo *info, u8 *spawnFlags, u16 *deleteNum, u8 areaNo) {
    sMapActorInfo *ci = dActorCreate::getMapActorInfo(data->mMapActorID);
    if (ci->mProfileName == fProfile::DUMMY_ACTOR) {
        return nullptr;
    }

    float zPos;
    if (info->mFlags & ACTOR_CREATE_MAPOBJ) {
        if (data->mLayer != 0) {
            zPos = addMapObjZposCount_layer2();
        } else {
            zPos = addMapObjZposCount();
        }
    } else {
        if (data->mLayer != 0) {
            zPos = addZposCount_layer2();
        } else {
            zPos = addZposCount();
        }
    }
    mVec3_c pos(data->mX + info->mSpawnOffsetX, -(data->mY - info->mSpawnOffsetY), zPos);

    dActor_c::m_read_p_keep = spawnFlags;
    *(u16 *) dActor_c::m_flag_keep = *(u16 *) data->mEventNums; // [Works in MWCC without the casts too]
    dActor_c::m_mbgchoice_keep = data->mLayer;

    u8 flagKeepHi = dActor_c::m_flag_keep[1];
    u8 flagKeepLo = dActor_c::m_flag_keep[0];
    dActor_c::m_flagbit_keep = 0;
    if (flagKeepHi != 0) {
        dActor_c::m_flagbit_keep = 1LL << (flagKeepHi - 1);
    }
    if (flagKeepLo != 0) {
        dActor_c::m_flagbit_keep = 1LL << (flagKeepLo - 1);
    }

    ci = dActorCreate::getMapActorInfo(data->mMapActorID);
    dActor_c *actor = dActor_c::construct(ci->mProfileName, data->mParam, &pos, nullptr, data->mLayer);
    if (actor != nullptr) {
        if (data->mMapActorID >= 10) {
            actor->mAreaNo = areaNo;
            actor->mpSpawnFlags = spawnFlags;
            actor->mpDeleteVal = deleteNum;
            *(u16 *) actor->mEventNums = *(u16 *) dActor_c::m_flag_keep;
            actor->mEventMask = dActor_c::m_flagbit_keep;
            actor->mBlockHit = false;
            actor->mSpriteSpawnFlags = info->mFlags;
            // [Couldn't get this to match in a better way...]
            float areaX = actor->mVisibleAreaSize.x;
            if (!areaX) {
                float areaY = actor->mVisibleAreaSize.y;
                if (!areaY) {
                    actor->mVisibleAreaSize.set(info->mSpawnRangeWidth * 2.0f, info->mSpawnRangeHeight * 2.0f);
                }
            }
        }
        *spawnFlags |= dActor_c::ACTOR_SPAWNED;
    }
    return actor;
}
