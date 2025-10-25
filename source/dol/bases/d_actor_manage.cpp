#include <cstdlib>
#include <game/bases/d_actor_manage.hpp>
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
    for (int i = 0 ; i < 1000; i++) {
        m_7e0[i] = 0;
        m_10[i] = 0;
    }
    if (dInfo_c::mGameFlag & 1) {
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
    for (int i = 0 ; i < 1000; i++) {
        m_7e0[i] &= ~1;
    }
    mPosX = 0;
    mPosY = 0;
    mTileX = 0;
    mTileY = 0;
    MapActorInital_next();
}

void dActorCreateMng_c::MapActorInital_set() {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    create(file->mpActorCreates, m_10, m_7e0, file->mActorCreateCount, false);
}

void dActorCreateMng_c::MapActorInital_next() {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    u8 areaNo = dScStage_c::m_instance->getCurrArea();
    create(file->mActorCreatesByGroup[areaNo], m_10 + file->mActorOffsets[areaNo], m_7e0 + file->mActorOffsets[areaNo], file->mActorGroupCounts[areaNo], true);
}

bool dActorCreateMng_c::someCheck(sActorCreateData *data) {
    static const ProfileName c_skip_profiles[] = {
        fProfile::EN_PUKUCOIN,
        fProfile::EN_KARON,
        fProfile::EN_BIGKARON,
        fProfile::EN_KARON_HEAD,
        fProfile::WAKI_KERONPA,
        0xffff
    };
    u16 curr = 0;
    while (c_skip_profiles[curr] != 0xffff) {
        if (data->mActorCreateID == c_skip_profiles[curr]) {
            break;
        }
        curr++;
    }
    if (c_skip_profiles[curr] != 0xffff) {
        return true;
    }
    return false;
}

void dActorCreateMng_c::operate(sActorCreateData *data, u8 area, u32 idx) {
    dActorGroupIdMng_c::m_instance->operate(data, data->mParam >> 24, area, idx);
}

bool dActorCreateMng_c::GroupIdCheck(ulong param) {
    return (param >> 24) == 0;
}

void dActorCreateMng_c::create(sActorCreateData *data, u16 *data1, u8 *data2, int count, bool b) {
    if (count == 0 || data == nullptr) {
        return;
    }

    sActorCreateInfo *curr;
    dScStage_c *stage = dScStage_c::m_instance;
    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;
    int top;
    int left;
    int right;
    int bottom;
    int trueHeight;
    int trueWidth;
    int centerY;
    int centerX;
    int tmpa, tmpb, ha, hb;
    u32 i;
    bool shouldCreate2;
    dCdFile_c *file;
    bool tmp;
    bool shouldCreate;
    u8 areaNo;
    int boundX;
    int boundY;
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

    trueHeight = (top - bottom) >> 1;
    trueWidth = (right - left) >> 1;

    centerY = bottom + trueHeight;
    centerX = left + trueWidth;

    i = 0;
    file = dCd_c::m_instance->getFileP(stage->mCurrFile);
    areaNo = dScStage_c::m_instance->getCurrArea();

    while (i != count) {
        if ((*data2 & 1) == 0) {
            curr = dActorCreate::getActorCreateInfo(data->mActorCreateID);
            shouldCreate = false;
            shouldCreate2 = false;
            if (!b) {
                tmp = false;
                if ((curr->m_24 & 0x10) != 0) {
                    if (!GroupIdCheck(data->mParam)) {
                        operate(data, stage->mCurrFile, i);
                        tmp = true;
                    }
                } else if (someCheck(data)) {
                    shouldCreate = true;
                }
                if (!tmp) {
                    sRangePosSize bound2;
                    bound2.mX = data->mX;
                    bound2.mY = data->mY;
                    bound2.mWidth = curr->m_14 * 2;
                    bound2.mHeight = curr->m_18 * 2;
                    if (file->getAreaNo(&bound2) == areaNo) {
                        shouldCreate2 = true;
                    }
                }
            } else {
                if ((curr->m_24 & 0x10) != 0) {
                    if (GroupIdCheck(data->mParam)) {
                        shouldCreate2 = true;
                    }
                } else {
                    shouldCreate2 = true;
                }
            }
            if (!shouldCreate && shouldCreate2) {
                if (curr->m_24 & 0x2) {
                    shouldCreate = true;
                } else {
                    ha = curr->m_14 + ((curr->m_20 + curr->m_22) >> 1);
                    hb = curr->m_18 + ((curr->m_1c + curr->m_1e) >> 1);
                    tmpa = data->mX + curr->m_0c;
                    tmpb = data->mY - curr->m_10;
                    if (
                        abs(centerX - tmpa) <= abs(trueWidth + ha) &&
                        abs(centerY - tmpb) <= abs(trueHeight + hb)
                    ) {
                        shouldCreate = true;
                    }
                }
            }
            if (shouldCreate) {
                construct(data, curr, data2, data1, areaNo);
            }
        }
        data++;
        data2++;
        data1++;
        i++;
    }
}

void dActorCreateMng_c::MapActorScroolCreateCheck() {
    if (dInfo_c::mGameFlag & 1) {
        return;
    }
    if (m_bcb == 1) {
        return;
    }

    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;
    int boundX, boundY;
    int m_18;
    int m_1c;
    int actualLeft, actualRight, actualTop, actualBottom;
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
    boundY = -(int)py;

    actualLeft = (boundX - 32) >> 4;
    actualRight = (boundX + (int) sx + 48) >> 4;
    actualTop = (boundY - 16) >> 4;
    actualBottom = (boundY + (int) sy + 32) >> 4;

    sSomeStruct s;
    if (dBg_c::m_bg_p->getM_8ffa4() != dBg_c::m_bg_p->getDispScale()) {
        s.m_00 = actualLeft;
        s.m_04 = actualBottom;
        s.mTileLeft = actualLeft;
        s.mTileRight = actualRight;
        s.mTileTop = actualTop;
        s.mTileBottom = actualBottom;
        s.m_18 = 0;
        s.m_1c = -1;
        dealWithSomeStruct(&s, false);
        s.m_00 = actualRight;
        s.m_04 = actualBottom;
        s.mTileLeft = actualLeft;
        s.mTileRight = actualRight;
        s.mTileTop = actualTop;
        s.mTileBottom = actualBottom;
        s.m_18 = -1;
        s.m_1c = -1;
        dealWithSomeStruct(&s, false);
        endTileX = actualRight;
        endTileY = actualTop;
        mPosX = boundX;
        mPosY = boundY;
        s.m_00 = actualRight;
        s.m_04 = actualTop;
        s.m_18 = -1;
        s.m_1c = 0;
        dealWithSomeStruct(&s, false);
    } else {
        m80 = bgParam->m_80;
        m81 = bgParam->m_81;
        if (m80 != 0) {
            if (m80 == 2) {
                endTileX = actualRight;
                m_18 = -1;
            } else {
                endTileX = actualLeft;
            }
        } else {
            endTileX = mTileY;
            m_18 = 1;
        }
        if (m81 != 0) {
            if (m81 == 1) {
                endTileY = actualBottom;
                m_1c = -1;
            } else {
                endTileY = actualTop;
            }
        } else {
            endTileY = mTileY;
            m_1c = 1;
        }
        if (mTileX != endTileX || mTileY != endTileY) {
            s.m_00 = endTileX;
            s.m_04 = endTileY;
            s.mTileLeft = actualLeft;
            s.mTileRight = actualRight;
            s.mTileTop = actualTop;
            s.mTileBottom = actualBottom;
            s.m_18 = m_18;
            s.m_1c = m_1c;
            dealWithSomeStruct(&s, true);
        }
    }
    mPosX = boundX;
    mPosY = boundY;
    mTileX = endTileX;
    mTileY = endTileY;
}

void dActorCreateMng_c::dealWithSomeStruct(sSomeStruct *s, int b) {
    dScStage_c *stage = dScStage_c::m_instance;
    u8 areaNo = stage->mCurrArea;
    dCdFile_c *file = dCd_c::m_instance->getFileP(stage->mCurrFile);

    int n = file->mActorGroupCounts[areaNo];
    if (n == 0) {
        return;
    }
    sActorCreateInfo *curr;
    sActorCreateData *x = file->mActorCreatesByGroup[areaNo];
    if (x == nullptr) {
        return;
    }
    u16 *currD1 = m_10 + file->mActorOffsets[areaNo];
    u8 *currD2 = m_7e0 + file->mActorOffsets[areaNo];
    bool shouldCreate;
    int tmpa, tmpb;
    int iVar6, tileR, tileL;
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
        if ((*currD2 & 1) == 0) {
            curr = dActorCreate::getActorCreateInfo(x->mActorCreateID);
            shouldCreate = false;
            tmpa = x->mX + curr->m_0c;
            tmpb = x->mY - curr->m_10;
            tileR = (tmpa - curr->m_14 - curr->m_20) >> 4;
            tileL = (tmpa + curr->m_14 + curr->m_22) >> 4;
            tileT = (tmpb - curr->m_18 - curr->m_1c) >> 4;
            tileB = (tmpb + curr->m_18 + curr->m_1e) >> 4;
            iVar6 = tileL;
            if (c18 != 0) {
                iVar6 = tileR;
            }
            iVar17 = tileB;
            if (c1c != 0) {
                iVar17 = tileT;
            }
            bool skipCheck = false;
            if (curr->m_24 & 0x10) {
                if (!GroupIdCheck(x->mParam)) {
                    skipCheck = true;
                }
            }
            if (!skipCheck) {
                if (b) {
                    if (iVar6 == c0) {
                        if (c18 != 1) {
                            if (c18 < 0) {
                                shouldCreate = ScroolAreaInCheck(ctr, tileR, ctt, ctb, tileT, tileB);
                            } else {
                                shouldCreate = ScroolAreaInCheck(ctl, tileL, ctt, ctb, tileT, tileB);
                            }
                        }
                    } else if (iVar17 == c4) {
                        if (c1c != 1) {
                            if (c1c < 0) {
                                shouldCreate = ScroolAreaInLoopCheck(ctb, tileT, ctl, ctr, tileR, tileL);
                            } else {
                                shouldCreate = ScroolAreaInLoopCheck(ctt, tileB, ctl, ctr, tileR, tileL);
                            }
                        }
                    }
                } else {
                    if (ScroolAreaInCheck(c0, iVar6, ctt, ctb, tileT, tileB)) {
                        shouldCreate = true;
                    }
                    if (ScroolAreaInCheck(c4, iVar17, ctl, ctr, tileR, tileL)) {
                        shouldCreate = true;
                    }
                }
            }
            if (shouldCreate) {
                construct(x, curr, currD2, currD1, areaNo);
            }
        }
        x++;
        currD1++;
        currD2++;
    }
}

void dActorCreateMng_c::incZposCount() {
    mZPosCount++;
    if (mZPosCount >= 30) {
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
    if (mZPosCountLayer2 >= 30) {
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
    if (mMapObjZPosCount >= 30) {
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
    if (mMapObjZPosCountLayer2 >= 30) {
        mMapObjZPosCountLayer2 = 0;
    }
}

float dActorCreateMng_c::addMapObjZposCount_layer2() {
    float res = -3100.0f - mMapObjZPosCountLayer2 * 8.0f;
    incMapObjZposCount_layer2();
    return res;
}

dActor_c *dActorCreateMng_c::construct(sActorCreateData *data, sActorCreateInfo *info, u8 *data2, u16 *data1, u8 area) {
    sActorCreateInfo *ci = dActorCreate::getActorCreateInfo(data->mActorCreateID);
    if (ci->mProfileName == fProfile::DUMMY_ACTOR) {
        return nullptr;
    }

    float zPos;
    if ((info->m_24 & 8) != 0) {
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
    mVec3_c pos(data->mX + info->m_04, -(data->mY - info->m_08), zPos);

    dActor_c::m_read_p_keep = data2;
    *(u16 *) dActor_c::m_flag_keep = *(u16 *) data->mEventNums; // [Works in MWCC without the casts too]
    dActor_c::m_mbgchoice_keep = data->mLayer;
    dActor_c::m_flagbit_keep = 0;
    u64 fb = 0;

    u8 b1 = dActor_c::m_flag_keep[1];
    u8 b0 = dActor_c::m_flag_keep[0];
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
            actor->mAreaNo = area;
            actor->mpSpawnFlags = data2;
            actor->mpDeleteVal = data1;
            *(u16 *) actor->mEventNums = *(u16 *) dActor_c::m_flag_keep;
            actor->mEventMask = dActor_c::m_flagbit_keep;
            actor->mBlockHit = false;
            actor->mSpriteSpawnFlags = info->m_24;
            // [Couldn't get this to match in a better way...]
            float areaX = actor->mVisibleAreaSize.x;
            if (!areaX) {
                float areaY = actor->mVisibleAreaSize.y;
                if (!areaY) {
                    actor->mVisibleAreaSize.set(info->m_14 * 2.0f, info->m_18 * 2.0f);
                }
            }
        }
        *data2 |= 1;
    }
    return actor;
}
