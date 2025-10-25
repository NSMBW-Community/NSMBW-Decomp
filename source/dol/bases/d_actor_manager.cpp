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
    m_bc8 = 0;
    m_bca = false;
    m_bcb = 0;
    mZPosCount = 0;
    mZPosCountLayer2 = 0;
    mMapObjZPosCount = 0;
    mMapObjZPosCountLayer2 = 0;
}

bool dActorCreateMng_c::ScroolAreaInCheck(int a, int b, int c, int d, int e, int f) {
    if (a == b) {
        return d >= e && c <= f;
    }
    return false;
}

bool dActorCreateMng_c::ScroolAreaInLoopCheck(int a, int b, int c, int d, int e, int f) {
    return ScroolAreaInCheck(a, b, c, d, e, f);
}

void dActorCreateMng_c::setMapActorCreate() {
    for (int i = 0 ; i < MAX_ACTOR_COUNT; i++) {
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
    if (dInfo_c::mGameFlag & 1) {
        return;
    }
    if (m_bca) {
        return;
    }
    for (int i = 0; i < MAX_ACTOR_COUNT; i++) {
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
    create(file->mpActorCreates, mDeleteNums, mSpawnFlags, file->mActorCreateCount, false);
}

void dActorCreateMng_c::MapActorInital_next() {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    u8 areaNo = dScStage_c::m_instance->getCurrArea();
    create(
        file->mActorCreatesByArea[areaNo],
        &mDeleteNums[file->mActorCreateIdxForArea[areaNo]],
        &mSpawnFlags[file->mActorCreateIdxForArea[areaNo]],
        file->mActorCreateCountByArea[areaNo],
        true
    );
}

bool dActorCreateMng_c::GlobalActorCheck(sActorCreateData *data) {
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

void dActorCreateMng_c::processGroupId(sActorCreateData *data, u8 file, u32 idx) {
    dActorGroupIdMng_c::m_instance->process(data, data->mParam >> 24, file, idx);
}

bool dActorCreateMng_c::GroupIdCheck(ulong param) {
    return (param >> 24) == 0;
}

void dActorCreateMng_c::create(sActorCreateData *data, u16 *deleteNum, u8 *spawnFlags, int count, bool b) {
    if (count == 0 || data == nullptr) {
        return;
    }

    sActorCreateInfo *curr;
    dScStage_c *stage = dScStage_c::m_instance;
    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;

    int top, left, right, bottom;
    int boundHeight, boundWidth, boundCenterY, boundCenterX;
    int actorCenterX, actorCenterY, actorWidth, actorHeight;
    u32 i;
    bool spawnIfInBounds;
    dCdFile_c *file;
    bool canSpawnWithAreaCheck;
    bool spawnActor;
    u8 areaNo;
    int boundX, boundY;
    float px, py, sx, sy;

    px = bgParam->pos().x;
    py = bgParam->pos().y;
    sx = bgParam->size().x;
    sy = bgParam->size().y;

    boundX = px;
    boundY = -(int) py;

    bottom = boundY - 32;
    top = boundY + (int)sy + 48;

    left = boundX - 48;
    right = boundX + (int)sx + 64;

    boundHeight = (top - bottom) >> 1;
    boundWidth = (right - left) >> 1;

    boundCenterY = bottom + boundHeight;
    boundCenterX = left + boundWidth;

    i = 0;
    file = dCd_c::m_instance->getFileP(stage->mCurrFile);
    areaNo = dScStage_c::m_instance->getCurrArea();

    while (i != count) {
        if ((*spawnFlags & dActor_c::ACTOR_SPAWNED) == 0) {
            curr = dActorCreate::getActorCreateInfo(data->mActorCreateID);
            spawnActor = false;
            spawnIfInBounds = false;
            if (!b) {
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
                construct(data, curr, spawnFlags, deleteNum, areaNo);
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
    if (m_bcb == 1) {
        return;
    }

    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;
    int boundX, boundY;
    int m_18, m_1c;
    int tileLeft, tileRight, tileTop, tileBottom;
    int endTileX, endTileY;
    u8 m81;
    int m80;
    float px, py, sx, sy;

    m_18 = 0;
    m_1c = 0;

    px = bgParam->pos().x;
    py = bgParam->pos().y;
    sx = bgParam->size().x;
    sy = bgParam->size().y;

    boundX = px;
    boundY = -(int) py;

    tileLeft = (boundX - 32) >> 4;
    tileRight = (boundX + (int) sx + 48) >> 4;
    tileTop = (boundY - 16) >> 4;
    tileBottom = (boundY + (int) sy + 32) >> 4;

    sSomeStruct s;
    if (dBg_c::m_bg_p->get_8ffa4() != dBg_c::m_bg_p->get_8ffac()) {
        s.m_00 = tileLeft;
        s.m_04 = tileBottom;
        s.mTileLeft = tileLeft;
        s.mTileRight = tileRight;
        s.mTileTop = tileTop;
        s.mTileBottom = tileBottom;
        s.m_18 = 0;
        s.m_1c = -1;
        MapActorScrollCreate(&s, false);
        s.m_00 = tileRight;
        s.m_04 = tileBottom;
        s.mTileLeft = tileLeft;
        s.mTileRight = tileRight;
        s.mTileTop = tileTop;
        s.mTileBottom = tileBottom;
        s.m_18 = -1;
        s.m_1c = -1;
        MapActorScrollCreate(&s, false);
        endTileX = tileRight;
        endTileY = tileTop;
        mPosX = boundX;
        mPosY = boundY;
        s.m_00 = tileRight;
        s.m_04 = tileTop;
        s.m_18 = -1;
        s.m_1c = 0;
        MapActorScrollCreate(&s, false);
    } else {
        m80 = bgParam->m_80;
        m81 = bgParam->m_81;
        if (m80 != 0) {
            if (m80 == 2) {
                endTileX = tileRight;
                m_18 = -1;
            } else {
                endTileX = tileLeft;
            }
        } else {
            endTileX = mTileY; // [Bug? Should be mTileX]
            m_18 = 1;
        }
        if (m81 != 0) {
            if (m81 == 1) {
                endTileY = tileBottom;
                m_1c = -1;
            } else {
                endTileY = tileTop;
            }
        } else {
            endTileY = mTileY;
            m_1c = 1;
        }
        if (mTileX != endTileX || mTileY != endTileY) {
            s.m_00 = endTileX;
            s.m_04 = endTileY;
            s.mTileLeft = tileLeft;
            s.mTileRight = tileRight;
            s.mTileTop = tileTop;
            s.mTileBottom = tileBottom;
            s.m_18 = m_18;
            s.m_1c = m_1c;
            MapActorScrollCreate(&s, true);
        }
    }
    mPosX = boundX;
    mPosY = boundY;
    mTileX = endTileX;
    mTileY = endTileY;
}

void dActorCreateMng_c::MapActorScrollCreate(sSomeStruct *s, int b) {
    dScStage_c *stage = dScStage_c::m_instance;
    u8 areaNo = stage->mCurrArea;
    dCdFile_c *file = dCd_c::m_instance->getFileP(stage->mCurrFile);

    int n = file->mActorCreateCountByArea[areaNo];
    if (n == 0) {
        return;
    }
    sActorCreateInfo *curr;
    sActorCreateData *data = file->mActorCreatesByArea[areaNo];
    if (data == nullptr) {
        return;
    }
    u16 *deleteNum = &mDeleteNums[file->mActorCreateIdxForArea[areaNo]];
    u8 *spawnFlags = &mSpawnFlags[file->mActorCreateIdxForArea[areaNo]];
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
    int c18 = s->m_18;
    int c1c = s->m_1c;
    for (u32 i = 0; i != n; i++) {
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
            if (c18 != 0) {
                iVar6 = tileL;
            }
            iVar17 = tileB;
            if (c1c != 0) {
                iVar17 = tileT;
            }
            bool noSpawn = false;
            if (curr->mFlags & ACTOR_CREATE_GROUPED) {
                if (!GroupIdCheck(data->mParam)) {
                    noSpawn = true;
                }
            }
            if (!noSpawn) {
                if (b) {
                    if (iVar6 == c0) {
                        if (c18 != 1) {
                            if (c18 < 0) {
                                spawnActor = ScroolAreaInCheck(ctr, tileL, ctt, ctb, tileT, tileB);
                            } else {
                                spawnActor = ScroolAreaInCheck(ctl, tileR, ctt, ctb, tileT, tileB);
                            }
                        }
                    } else if (iVar17 == c4) {
                        if (c1c != 1) {
                            if (c1c < 0) {
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
                construct(data, curr, spawnFlags, deleteNum, areaNo);
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

dActor_c *dActorCreateMng_c::construct(sActorCreateData *data, sActorCreateInfo *info, u8 *spawnFlags, u16 *deleteNum, u8 areaNo) {
    sActorCreateInfo *ci = dActorCreate::getActorCreateInfo(data->mActorCreateID);
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

    u8 b1 = dActor_c::m_flag_keep[1];
    u8 b0 = dActor_c::m_flag_keep[0];
    dActor_c::m_flagbit_keep = 0;
    if (b1 != 0) {
        dActor_c::m_flagbit_keep = 1LL << (b1 - 1);
    }
    if (b0 != 0) {
        dActor_c::m_flagbit_keep = 1LL << (b0 - 1);
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
