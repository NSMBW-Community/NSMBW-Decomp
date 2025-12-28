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

bool dActorCreateMng_c::ScroolAreaInCheck(int a, int b, int boundMin, int boundMax, int areaMin, int areaMax) {
    if (a == b) {
        return boundMax >= areaMin && boundMin <= areaMax;
    }
    return false;
}

bool dActorCreateMng_c::ScroolAreaInLoopCheck(int a, int b, int boundMin, int boundMax, int areaMin, int areaMax) {
    return ScroolAreaInCheck(a, b, boundMin, boundMax, areaMin, areaMax);
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
    mTileX = 0;
    mTileY = 0;
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
    mTileX = 0;
    mTileY = 0;
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
        if (data->mActorCreateID == sc_globalProfiles[curr]) {
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

    bool spawnActor;
    bool spawnIfInBounds;
    bool canSpawnWithAreaCheck;

    while (i != count) {
        if ((*spawnFlags & dActor_c::ACTOR_SPAWNED) == 0) {
            sMapActorInfo *curr = dActorCreate::getActorCreateInfo(data->mActorCreateID);
            spawnActor = false;
            spawnIfInBounds = false;
            if (!isAreaReload) {
                canSpawnWithAreaCheck = false;
                if (curr->mFlags & ACTOR_CREATE_GROUPED) {
                    if (!GroupIdCheck(data->mParam)) {
                        processGroupId(data, stage->mCurrFile, i);
                        canSpawnWithAreaCheck = true;
                    }
                } else if (GlobalActorCheck(data)) {
                    spawnActor = true;
                }
                if (!canSpawnWithAreaCheck) {
                    sRangePosSize bound;
                    bound.mX = data->mX;
                    bound.mY = data->mY;
                    bound.mWidth = curr->mSpawnRangeWidth * 2;
                    bound.mHeight = curr->mSpawnRangeHeight * 2;
                    if (file->getAreaNo(&bound) == areaNo) {
                        spawnIfInBounds = true;
                    }
                }
            } else {
                if (curr->mFlags & ACTOR_CREATE_GROUPED) {
                    if (GroupIdCheck(data->mParam)) {
                        spawnIfInBounds = true;
                    }
                } else {
                    spawnIfInBounds = true;
                }
            }
            if (!spawnActor && spawnIfInBounds) {
                if (curr->mFlags & ACTOR_CREATE_ALWAYS_SPAWN) {
                    spawnActor = true;
                } else {
                    actorWidth = curr->mSpawnRangeWidth + ((curr->mSpawnMarginLeft + curr->mSpawnMarginRight) >> 1);
                    actorHeight = curr->mSpawnRangeHeight + ((curr->mSpawnMarginTop + curr->mSpawnMarginBottom) >> 1);
                    actorCenterX = data->mX + curr->mSpawnRangeOffsetX;
                    actorCenterY = data->mY - curr->mSpawnRangeOffsetY;
                    if (
                        abs(boundCenterX - actorCenterX) <= abs(boundWidth + actorWidth) &&
                        abs(boundCenterY - actorCenterY) <= abs(boundHeight + actorHeight)
                    ) {
                        spawnActor = true;
                    }
                }
            }
            if (spawnActor) {
                mapActorSpawn(data, curr, spawnFlags, deleteNum, areaNo);
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

    sMapActorCreateBounds s;

    int endTileX, endTileY;

    if (dBg_c::m_bg_p->getDispScale() != dBg_c::m_bg_p->getPrevDispScale()) {
        s.m_00 = tileLeft;
        s.m_04 = tileBottom;
        s.mTileLeft = tileLeft;
        s.mTileRight = tileRight;
        s.mTileTop = tileTop;
        s.mTileBottom = tileBottom;
        s.mScrollDirX = SCROLL_X_LEFT;
        s.mScrollDirY = SCROLL_Y_DOWN;
        MapActorScrollCreate(&s, false);
        s.m_00 = tileRight;
        s.m_04 = tileBottom;
        s.mTileLeft = tileLeft;
        s.mTileRight = tileRight;
        s.mTileTop = tileTop;
        s.mTileBottom = tileBottom;
        s.mScrollDirX = SCROLL_X_RIGHT;
        s.mScrollDirY = SCROLL_Y_DOWN;
        MapActorScrollCreate(&s, false);

        mPosX = boundX;
        mPosY = boundY;
        endTileX = tileRight;
        endTileY = tileTop;

        s.m_00 = tileRight;
        s.m_04 = tileTop;
        s.mScrollDirX = SCROLL_X_RIGHT;
        s.mScrollDirY = SCROLL_Y_UP;
        MapActorScrollCreate(&s, false);
    } else {
        int bgScrollDirY = bgParam->mScrollDirY;
        int bgScrollDirX = bgParam->mScrollDirX;

        if (bgScrollDirX != 0) {
            if (bgScrollDirX == 2) {
                // right
                endTileX = tileRight;
                scrollDirX = SCROLL_X_RIGHT;
            } else {
                // left
                endTileX = tileLeft;
            }
        } else {
            // none
            endTileX = mTileY; // [Bug? Should be mTileX]
            scrollDirX = SCROLL_X_NONE;
        }

        if (bgScrollDirY != 0) {
            if (bgScrollDirY == 1U) {
                // down
                endTileY = tileBottom;
                scrollDirY = SCROLL_Y_DOWN;
            } else {
                // up
                endTileY = tileTop;
            }
        } else {
            // none
            endTileY = mTileY;
            scrollDirY = SCROLL_Y_NONE;
        }

        if (mTileX != endTileX || mTileY != endTileY) {
            s.m_00 = endTileX;
            s.m_04 = endTileY;
            s.mTileLeft = tileLeft;
            s.mTileRight = tileRight;
            s.mTileTop = tileTop;
            s.mTileBottom = tileBottom;
            s.mScrollDirX = scrollDirX;
            s.mScrollDirY = scrollDirY;
            MapActorScrollCreate(&s, true);
        }
    }
    mPosX = boundX;
    mPosY = boundY;
    mTileX = endTileX;
    mTileY = endTileY;
}

void dActorCreateMng_c::MapActorScrollCreate(sMapActorCreateBounds *s, int noDispScaleChange) {
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
    int iVar6, tileL, tileR;
    int iVar17, tileT, tileB;

    int c0 = s->m_00;
    int c4 = s->m_04;
    int ctl = s->mTileLeft;
    int ctr = s->mTileRight;
    int ctt = s->mTileTop;
    int ctb = s->mTileBottom;
    SCROLL_DIR_X_e dirX = s->mScrollDirX;
    SCROLL_DIR_Y_e dirY = s->mScrollDirY;

    for (u32 i = 0; i != mapActorCount; i++) {
        if ((*spawnFlags & dActor_c::ACTOR_SPAWNED) == 0) {
            curr = dActorCreate::getActorCreateInfo(data->mActorCreateID);
            spawnActor = false;
            centerX = data->mX + curr->mSpawnRangeOffsetX;
            centerY = data->mY - curr->mSpawnRangeOffsetY;
            tileL = (centerX - curr->mSpawnRangeWidth - curr->mSpawnMarginLeft) >> 4;
            tileR = (centerX + curr->mSpawnRangeWidth + curr->mSpawnMarginRight) >> 4;
            tileT = (centerY - curr->mSpawnRangeHeight - curr->mSpawnMarginTop) >> 4;
            tileB = (centerY + curr->mSpawnRangeHeight + curr->mSpawnMarginBottom) >> 4;
            iVar6 = tileR;
            if (dirX != 0) {
                iVar6 = tileL;
            }
            iVar17 = tileB;
            if (dirY != 0) {
                iVar17 = tileT;
            }
            bool wrongGroupID = false;
            if (curr->mFlags & ACTOR_CREATE_GROUPED) {
                if (!GroupIdCheck(data->mParam)) {
                    wrongGroupID = true;
                }
            }
            if (!wrongGroupID) {
                if (noDispScaleChange) {
                    if (iVar6 == c0) {
                        if (dirX != SCROLL_X_NONE) {
                            if (dirX < 0) {
                                spawnActor = ScroolAreaInCheck(ctr, tileL, ctt, ctb, tileT, tileB);
                            } else {
                                spawnActor = ScroolAreaInCheck(ctl, tileR, ctt, ctb, tileT, tileB);
                            }
                        }
                    } else if (iVar17 == c4) {
                        if (dirY != SCROLL_Y_NONE) {
                            if (dirY < 0) {
                                spawnActor = ScroolAreaInLoopCheck(ctb, tileT, ctl, ctr, tileL, tileR);
                            } else {
                                spawnActor = ScroolAreaInLoopCheck(ctt, tileB, ctl, ctr, tileL, tileR);
                            }
                        }
                    }
                } else {
                    if (ScroolAreaInCheck(c0, iVar6, ctt, ctb, tileT, tileB)) {
                        spawnActor = true;
                    }
                    if (ScroolAreaInCheck(c4, iVar17, ctl, ctr, tileL, tileR)) {
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
    sMapActorInfo *ci = dActorCreate::getActorCreateInfo(data->mActorCreateID);
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

    ci = dActorCreate::getActorCreateInfo(data->mActorCreateID);
    dActor_c *actor = dActor_c::construct(ci->mProfileName, data->mParam, &pos, nullptr, data->mLayer);
    if (actor != nullptr) {
        if (data->mActorCreateID >= 10) {
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
