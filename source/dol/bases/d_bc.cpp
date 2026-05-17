#include <game/bases/d_bc.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_actor.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_block_manager.hpp>
#include <game/bases/d_effectmanager.hpp>
#include <constants/sound_list.h>

const float l_saka_data[][2] = {
    { 16.0f, -1.0f },
    { 0.0f, 1.0f },
    { 16.0f, -0.5f },
    { 8.0f, -0.5f },
    { 0.0f, 0.5f },
    { 8.0f, 0.5f },
    { 16.0f, -2.0f },
    { 32.0f, -2.0f },
    { -16.0f, 2.0f },
    { 0.0f, 2.0f },
    { 0.0f, 0.0f },
    { 16.0f, -0.25f },
    { 12.0f, -0.25f },
    { 8.0f, -0.25f },
    { 4.0f, -0.25f },
    { 0.0f, 0.25f },
    { 4.0f, 0.25f },
    { 8.0f, 0.25f },
    { 12.0f, 0.25f }
};

const float l_head_saka_data[][2] = {
    { 0.0f, 1.0f },
    { 16.0f, -1.0f },
    { 0.0f, 0.5f },
    { 8.0f, 0.5f },
    { 16.0f, -0.5f },
    { 8.0f, -0.5f },
    { 0.0f, 2.0f },
    { -16.0f, 2.0f },
    { 32.0f, -2.0f },
    { 16.0f, -2.0f },
    { 16.0f, 0.0f },
    { 0.0f, 0.25f },
    { 4.0f, 0.25f },
    { 8.0f, 0.25f },
    { 12.0f, 0.25f },
    { 16.0f, -0.25f },
    { 12.0f, -0.25f },
    { 8.0f, -0.25f },
    { 4.0f, -0.25f }
};

const u8 l_saka_type[] = {
    3, 3,
    2, 2, 2, 2,
    4, 4, 4, 4,
    0,
    1, 1, 1, 1,
    1, 1, 1, 1
};

const u8 l_saka_dir[] = {
    DIR_LR_L, DIR_LR_R,
    DIR_LR_L, DIR_LR_L,
    DIR_LR_R, DIR_LR_R,
    DIR_LR_L, DIR_LR_L,
    DIR_LR_R, DIR_LR_R,
    DIR_LR_R,
    DIR_LR_L, DIR_LR_L,
    DIR_LR_L, DIR_LR_L,
    DIR_LR_R, DIR_LR_R,
    DIR_LR_R, DIR_LR_R
};

const s16 l_saka_angle[] = {
    DEG_TO_ANGLE(45), -DEG_TO_ANGLE(45), // 1/1 slope
    DEG_TO_ANGLE(26.6), DEG_TO_ANGLE(26.6), // 1/2 slope
    -DEG_TO_ANGLE(26.6), -DEG_TO_ANGLE(26.6), // 1/2 slope
    DEG_TO_ANGLE(63.404), DEG_TO_ANGLE(63.404), // 2/1 slope
    -DEG_TO_ANGLE(63.404), -DEG_TO_ANGLE(63.404), // 2/1 slope
    0, // no slope
    DEG_TO_ANGLE(14.036), DEG_TO_ANGLE(14.036), // 1/4 slope
    DEG_TO_ANGLE(14.036), DEG_TO_ANGLE(14.036), // 1/4 slope
    -DEG_TO_ANGLE(14.036), -DEG_TO_ANGLE(14.036), // 1/4 slope
    -DEG_TO_ANGLE(14.036), -DEG_TO_ANGLE(14.036), // 1/4 slope

};

const s16 l_head_saka_angle[] = {
    -DEG_TO_ANGLE(45), DEG_TO_ANGLE(45), // 1/1 slope
    -DEG_TO_ANGLE(26.6), -DEG_TO_ANGLE(26.6), // 1/2 slope
    DEG_TO_ANGLE(26.6), DEG_TO_ANGLE(26.6), // 1/2 slope
    -DEG_TO_ANGLE(63.404), -DEG_TO_ANGLE(63.404), // 2/1 slope
    DEG_TO_ANGLE(63.404), DEG_TO_ANGLE(63.404), // 2/1 slope
    0, // no slope
    -DEG_TO_ANGLE(14.036), -DEG_TO_ANGLE(14.036), // 1/4 slope
    -DEG_TO_ANGLE(14.036), -DEG_TO_ANGLE(14.036), // 1/4 slope
    DEG_TO_ANGLE(14.036), DEG_TO_ANGLE(14.036), // 1/4 slope
    DEG_TO_ANGLE(14.036), DEG_TO_ANGLE(14.036), // 1/4 slope
};

dBc_c::dBc_c() {
    init();
    mpRc = nullptr;
    mAmiLine = 1;
    mLayer = 0;
    mpLayer = &mLayer;
}

dBc_c::~dBc_c() {}

void dBc_c::init() {
    mpSensorFoot = nullptr;
    mpSensorHead = nullptr;
    mpSensorWall = nullptr;
    mOwningPlrNo = -1;
    mRidePlrNo = -1;
    clear();
}

void dBc_c::clear() {
    mFlags = 0;
    mSlopeType = SAKA_NONE;
    mSlopeAngle = l_saka_angle[mSlopeType];
    mFlippedSlopeType = SAKA_NONE;
    mFlippedSlopeAngle = l_head_saka_angle[mFlippedSlopeType];
    mAdjacentSlopeAngle = 0;
    m_e2 = 2;

    mFootUnitType = 0;
    mHeadUnitType = 0;
    mWallUnitType[0] = 0;
    mWallUnitType[1] = 0;

    clearDamage();
    mPushForce.set(0.0f, 0.0f, 0.0f);
}

void dBc_c::set(dActor_c *owner, sBcSensorIf_c *sensorFoot, sBcSensorIf_c *sensorHead, sBcSensorIf_c *sensorWall) {
    mpOwner = owner;
    mpSensorFoot = sensorFoot;
    mpSensorHead = sensorHead;
    mpSensorWall = sensorWall;
    mpLayer = &mLayer;
    if (owner != nullptr) {
        mpOwnerPos = &owner->mPos;
        mpOwnerLastPos = &owner->mLastPos;
        mpOwnerSpeed = &owner->mSpeed;
        mOwningPlrNo = owner->getPlrNo();
        mpLayer = &owner->mLayer;
    }
}

bool dBc_c::checkManyPoint(float *p, float f) {
    if (*p >= f) {
        return true;
    }
    *p += 8.0f;
    if (*p > f) {
        *p = f;
    }
    return false;
}

bool dBc_c::isBGCoin(ulong flags) {
    return (flags >> 1) & 1;
}

bool dBc_c::getBGCoin(ulong flags, ulong p2, ulong p3, mVec3_c *pos, int p5) {
    s8 plrNo = getPlrNo(pos->y);
    if (plrNo == -1) {
        return false;
    }
    if (isBGCoin(flags)) {
        if (p3 & 0x40000) {
            if ((p2 & 0xff) == 2) {
                return true;
            }
            mVec3_c tmp = *pos;
            daPyMng_c::incCoin(plrNo);
            daPyMng_c::addScore(100, plrNo);
            if ((p2 & 0xff) == 4) {
                EffectManager_c::SetBlueCoinEffect(&tmp);
            } else {
                EffectManager_c::SetCoinEffect(&tmp);
            }
            mVec3_c p(toTile(pos->x) + 8.0f, toTile(pos->y) - 8.0f, 0.0f);
            dAudio::SoundEffectID_t(SE_OBJ_GET_COIN).playMapSound(p, dAudio::getRemotePlayer(plrNo));

            if (p5) {
                int x = toTileInt(pos->x);
                int y = toTileInt(-pos->y);
                dBg_c::m_bg_p->BgUnitChange(x, y, mLayer, 0);
            }
            return true;
        }
        if (p3 & 0x20000) {
            if ((p2 & 0xff) == 2) {
                return true;
            }
            dBlockMng_c::BlockInfo_t info;
            info.mX = (int) pos->x & 0xfff0;
            info.mY = (int) -pos->y & 0xfff0;
            info.m_08 = (u8) (p2 & 0xff);
            info.m_09 = plrNo;
            dBlockMng_c::m_instance->fn_80088b30(info);
            return true;
        }
    }
    return false;
}

bool dBc_c::setWakuCoinAppear(ulong p1, ulong p2, ulong p3, mVec3_c *pos) {
    if (p1 == false) {
        u8 a = p2 & 0xff;
        if ((a == 40 || a == 41 || a == 42) && p3 & 0x80000) {
            dBg_c::m_bg_p->EntryWakuCoin(a, (s16) pos->x & 0xfff0, (s16) -pos->y & 0xfff0, mLayer);
            return true;
        }
    }
    return false;
}

void dBc_c::clearDamage() {
    m_e1 = -1;
    m_e0 = 0;
}

bool dBc_c::setBgDamageFlag(int kind, float x, float y) {
    u32 t = dBc_c::getUnitType(x, y, mLayer);
    u32 k = dBc_c::getUnitKind(x, y, mLayer);
    u32 a = k & 0xff;
    bool smth = false;
    if (t & 0x1000) {
        if (a < 3) {
            if (a == kind && (a == 2 || a == 3 || a == 1 && mOwnerPosDelta.x < 0.0f || a == 0 && mOwnerPosDelta.x > 0.0f)) {
                smth = true;
            }
        } else if (a == 4 || a == 5 || a == 6) {
            float tmpx = x - mpOwnerPos->x;
            float tx = toTile(x) + 8.0f;
            float ty = std::fmodf(-y, 2.75f);
            float shift = 0.0f;
            switch (a) {
                case 4:
                    shift = (16.0f - ty) / 4.0f;
                    break;
                case 5:
                    shift = (16.0f - ty) / 4.0f + 2.0f;
                    break;
                case 6:
                    shift = (16.0f - ty) / 4.0f;
                    break;
                default:
                    break;
            }
            if (nw4r::math::FAbs(tx - mpOwnerPos->x) < nw4r::math::FAbs(tmpx) + shift - 1.0f) {
                if (tmpx < 0.0f) {
                    if (ty >= tx - shift) {
                        smth = true;
                    }
                } else {
                    if (ty <= tx + shift) {
                        smth = true;
                    }
                }
            }
        } else {
            smth = true;
        }
    }
    if (smth) {
        m_e0 |= 1 << kind;
        if (m_e1 < a) {
            m_e1 = a;
        }
        return true;
    } else {
        static const u32 someFlags[] = { 0x819, 0x819, 0x8899, 0x4899 };
        if (t & someFlags[kind]) {
            m_e0 |= 0x80;
        }
        return false;
    }
}

bool dBc_c::_checkBreakUnit(ulong p1, u8 p2, float x, float y) {
    s8 plrNo = getPlrNo(y);
    return checkBreakUnit(p1, p2, x, y, mLayer, plrNo, getActorKind());
}

bool dBc_c::checkBreakUnit(ulong p1, u8 p2, float x, float y, u8 layer, s8 plrNo, u8 actorKind) {
    if (!(p1 & 0x4000)) {
        return false;
    }
    u32 type = getUnitType(x, y, layer);
    u32 kind = getUnitKind(x, y, layer);

    if (type == 0 || (type & 2) || (type & 0x2000)) {
        return false;
    }

    int v = -1;
    if (type & 0x10) {
        v = 1;
    }
    if (type & 4) {
        v = 0;
    }
    if (type & 8) {
        v = 2;
    }

    if (v == -1) {
        return false;
    }

    dBlockMng_c::BlockInfo2_t info;
    info.mX = toTileInt(x);
    info.mY = -toTileInt(-y);
    info.m_08 = kind;
    info.m_0c = v;
    info.m_10 = p2;
    info.m_11 = plrNo;
    info.m_12 = actorKind;
    dBlockMng_c::m_instance->fn_80088970(info);
    return true;
}

bool dBc_c::setHatenaBlockHit(ulong p1, u8 p2, float x, float y) {
    s8 plrNo = getPlrNo(y);
    u32 type = getUnitType(x, y, mLayer);
    u32 kind = getUnitKind(x, y, mLayer);

    if (type & 4) {
        if (p2 != 2 && (type & 0x2000)) {
            return false;
        }

        if (p1 & 0x100000) {
            dBlockMng_c::BlockInfo3_t info;
            info.mX = toTileInt(x);
            info.mY = -toTileInt(-y);
            info.mKind = kind;
            info.m_0c = p2;
            info.m_0d = 0;
            info.m_0e = plrNo;
            info.m_0f = getActorKind();
            dBlockMng_c::m_instance->fn_800883e0(info);
            return true;
        }
    }
    return false;
}

u32 dBc_c::setBreakBlockHit(ulong p1, u8 p2, float x, float y) {
    s8 plrNo = getPlrNo(y);
    u32 type = getUnitType(x, y, mLayer);
    u32 kind = getUnitKind(x, y, mLayer);

    static const u32 resStuff[] = { 0x200, 0x200, 0x80000000, 0x800000 };
    static const u32 resStuff2[] = { 0x100, 0x100, 0x40000000, 0x400000 };

    int tx = toTileInt(x);
    int ty = toTileInt(-y);

    u32 res = 0;
    if (type & 0x10) {
        if (p1 & 0x800000) {
            dBlockMng_c::BlockInfo3_t info;
            info.mX = tx;
            info.mY = -ty;
            info.mKind = kind;
            info.m_0c = p2;
            info.m_0d = 0;
            info.m_0e = plrNo;
            info.m_0f = getActorKind();
            if (dBlockMng_c::m_instance->fn_800883e0(info)) {
                res = resStuff[p2 & 0b11];
            } else {
                res = resStuff2[p2 & 0b11];
            }
        } else if (p1 & 0x400000) {
            dBlockMng_c::BlockInfo3_t info;
            info.mX = tx;
            info.mY = -ty;
            info.mKind = kind;
            info.m_0c = p2;
            info.m_0d = 1;
            info.m_0e = plrNo;
            info.m_0f = getActorKind();
            dBlockMng_c::m_instance->fn_800883e0(info);
            res = resStuff2[p2 & 0b11];
        }
    }

    return res;
}

u32 dBc_c::checkFoot() {
    if (mpSensorFoot == nullptr) {
        return FLAG_NONE;
    }

    mFootPrevUnitType = mFootUnitType;
    mFootUnitType = 0x10;
    mFootAttr = 0;
    m_e0 &= ~BIT_FLAG(2);
    mLastUnitType = 0;
    mLastUnitKind = 0;
    mIceSpeed = 0.0f;
    mPlayerFlags &= ~BIT_FLAG(2);
    int footSensorFlags = mpSensorFoot->mFlags;
    u32 res = _checkFootSaka(footSensorFlags);
    if (!(res & (FLAG_13 | FLAG_14))) {
        mSlopeType = SAKA_NONE;
        mSlopeAngle = l_saka_angle[mSlopeType];
        m_e2 = 2;
        switch (footSensorFlags & 1) {
            case SENSOR_IS_POINT:
                res = checkFootPoint(mpSensorFoot->asPointSensor(), footSensorFlags);
                break;
            case SENSOR_IS_LINE:
                res = checkFootLine(mpSensorFoot->asLineSensor(), footSensorFlags);
                break;
        }
    }

    if (!(footSensorFlags & BIT_FLAG(2))) {
        u32 checkRes = (this->*checkObjFoot)(footSensorFlags, 0.0f);
        if (checkRes != 0) {
            res &= 0xffe01fff;
            res |= checkRes;
        }
    }

    if (m_e0 & BIT_FLAG(7)) {
        m_e0 &= ~(BIT_FLAG(2) | BIT_FLAG(7));
    }

    if (res & FLAG_16) {
        if (mFootUnitType != 1 || (footSensorFlags & BIT_FLAG(4))) {
            mIceSpeedParam = 1.0f;
        } else if (footSensorFlags & BIT_FLAG(5)) {
            sLib::chase(&mIceSpeedParam, 1.0f, 0.03f);
        } else {
            sLib::chase(&mIceSpeedParam, 1.0f, 0.01f);
        }
    } else {
        mIceSpeedParam = 0.0f;
    }

    res &= 0x3ffe000;
    mFlags = (mFlags & 0xfc001fff) | res;
    mDirection = DIR_LR_R;
    if (mpOwnerSpeed->x < 0.0f) {
        mDirection = DIR_LR_L;
    }
    mPushForce.set(0.0f, 0.0f, 0.0f);

    static AttrFunc scAttrFunc[] = {
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNuma,
        &dBc_c::_attrRight,
        &dBc_c::_attrLeft,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal,
        &dBc_c::_attrNormal
    };

    (this->*(scAttrFunc[mFootUnitType]))();

    return res;
}

u32 dBc_c::checkFootPoint(sBcSensorPoint *sensor, ulong p) {
    mVec2_c v(sensor->mX / 4096.0f, sensor->mY / 4096.0f);
    u32 res = _checkFoot(v.x, v.y, p) & 0xff7fffff;
    gUnitX = 0xffff;
    gUnitY = 0xffff;
    u32 res2 = _checkFootBlock(v.x, v.y, p);
    return res | res2;
}

u32 dBc_c::checkFootLine(sBcSensorLine *sensor, ulong p) {
    u32 res = 0;
    float dist = sensor->mOffset / 4096.0f;
    float x, y;
    if (sensor->mLineA < sensor->mLineB) {
        x = sensor->mLineA / 4096.0f;
        y = sensor->mLineB / 4096.0f;
    } else {
        x = sensor->mLineB / 4096.0f;
        y = sensor->mLineA / 4096.0f;
    }
    float currX = x;
    bool found = true;
    do {
        u32 tmpres = _checkFoot(currX, dist, p);
        if (!(tmpres & 0x1000000)) {
            found = false;
        }
        res |= tmpres;
    } while (!checkManyPoint(&currX, y));
    if (!found) {
        res &= 0xfeffffff;
    }
    gUnitX = 0xffff;
    gUnitY = 0xffff;
    if (res & 0x800000) {
        res &= 0xff7fffff;
        x -= 1.0f;
        y += 1.0f;
    }
    currX = x;
    do {
        res |= _checkFootBlock(currX, dist, p);
    } while (!checkManyPoint(&currX, y));
    return res;
}

u32 dBc_c::_checkFoot(float x, float y, ulong sensorFlags) {
    float footX = mpOwnerPos->x + x;
    float footY = mpOwnerPos->y + y;
    u32 type = getUnitType(footX, footY, mLayer);
    u32 kind = getUnitKind(footX, footY, mLayer);
    if (type == 0 && kind == 0) {
        return FLAG_NONE;
    }
    if (mLastUnitType == 0) {
        mLastUnitType = type;
        mLastUnitKind = kind;
    }
    mVec3_c pos(footX, footY, 0.0f);

    if (isBGCoin(type)) {
        getBGCoin(type, kind, sensorFlags, &pos, 1);
        return FLAG_NONE;
    }

    if (setWakuCoinAppear(type, kind, sensorFlags, &pos)) {
        return FLAG_NONE;
    }

    if (_checkBreakUnit(type, 3, footX, footY)) {
        u32 res = FLAG_22;
        if (!(sensorFlags & BIT_FLAG(28))) {
            res |= FLAG_13;
        }
        return res;
    }

    if ((type & 4) && (type & 0x2000)) {
        return FLAG_NONE;
    }

    if (type & 0x20 && (kind & 0xff) == 10 && !(type & 0x8000)) {
        type = (type & ~BIT_FLAG(5)) | 1;
    }

    setBgDamageFlag(2, footX, footY);

    u32 res = 0;
    u32 mask = 0x8899;
    u32 mask2 = 0x8000;
    if (sensorFlags & mask2) {
        mask |= BIT_FLAG(19);
        mask2 |= BIT_FLAG(19);
    }
    if (type & mask) {
        if ((mask & mask2) && (type & 0x20)) {
            return FLAG_NONE;
        }
        if ((type & 0x400) && (sensorFlags & 0x2000)) {
            return FLAG_NONE;
        }

        if (type & 0x14) {
            res |= BIT_FLAG(23);
        }

        if ((type & 0x10000) && (type & 1)) {
            int k = kind & 0xff;
            if (k == 8 || k == 10 || k == 12) {
                res |= BIT_FLAG(24);
            }
        }

        bool smth;
        if (type & 0x10 && sensorFlags & 0x800000) {
            smth = true;
        } else {
            smth = false;
        }

        if (smth && sensorFlags & FLAG_28) {
            return FLAG_NONE;
        }

        smth = false;
        u32 uVar13 = 0b1111;
        switch (checkHitMameBlock(footX, footY, mLayer)) {
            case 0:
                uVar13 = 0b111;
                smth = true;
                break;
            case 2:
                uVar13 = 0b1111;
                smth = true;
                break;
        }
        if (type & mask2 && ((kind >> 16) & 0xff) != 3) {
            int ty = toTile(-y);
            float tmp = -(mpOwnerPos->y + y);
            if (tmp > ty + 1.0f && !(mFlags & 0x200000 && mFlags & 0x4000 && mFlags & 0x20000000)) {
                smth = false;
            }
        }
        if (smth) {
            if (type & 0x2000000 && !mGrounded) {
                u32 newX = toTileU(footX);
                u32 tmp = (int) -footY & ~0xf;
                u16 newY = toTileU(tmp - 0.1f);
                u32 type = _getUnitType(newX, newY, mLayer);
                u32 kind = _getUnitKind(newX, newY, mLayer);
                if (type & 0x20 && (kind & 0xff) != 10 && ((kind >> 16) & 0xff) != 10) {
                    return FLAG_NONE;
                }
            }
            if (type & mask2) {
                res |= FLAG_21;
            }
            if (_checkFootAttr2(kind) && !(sensorFlags & BIT_FLAG(31))) {
                u16 tmp = (((int) -y) & ~uVar13);
                float shiftY = -(tmp - -y);
                if (mOwnerPosDelta.y != 0.0f) {
                    float tmp = -(mOwnerPosDelta.y - 1.0f);
                    if (tmp < 2.0f) {
                        tmp = 2.0f;
                    }
                    if (shiftY > tmp) {
                        shiftY = tmp;
                    } else if (shiftY < 0.0f) {
                        shiftY = 0.0f;
                    }
                }
                mpOwnerPos->y += shiftY;
            }
            res |= 0x2000;
        }
    }

    return res;
}

u32 dBc_c::_checkFootBlock(float x, float y, ulong sensorFlags) {
    float footX = mpOwnerPos->x + x;
    float footY = mpOwnerPos->y + y;
    if (!_isDifferentPos(footX, footY)) {
        return FLAG_NONE;
    }

    u32 type = getUnitType(footX, footY, mLayer);
    u32 kind = getUnitKind(footX, footY, mLayer);

    u16 ix = footX;
    u16 iy = -footY;

    if (type & 0x20000 && mpOwnerSpeed->y <= 0.0f && sensorFlags & BIT_FLAG(21)) {
        mVec2_c pos(ix & ~0xf, iy & ~0xf);
        if (((kind >> 16) & 0xff) != 8) {
            dBlockMng_c::m_instance->chikuwaCreate_set(&pos);
        }
    }
    u32 res = 0;
    if (setHatenaBlockHit(sensorFlags, 3, footX, footY)) {
        res |= FLAG_23;
    }
    res |= setBreakBlockHit(sensorFlags, 3, footX, footY);

    return res;
}

int dBc_c::_checkFootSakaType(float x, float y, ulong *type, ulong *kind) {
    u16 tx = toTile(x);
    *type = _getUnitType(tx, toTile(y), mLayer);
    *kind = _getUnitKind(tx, toTile(y), mLayer);

    int res = 0;
    if (*type & 0x20) {
        res = 2;
        if ((*kind & 0xff) == 10) {
            res = 1;
            if (*type & 0x8000) {
                res = 3;
            }
        }
    } else if (*type & 0x8899) {
        res = 1;
        if (*type & 0x100000) {
            res = 3;
        }
    }

    return res;
}

u32 dBc_c::_checkFootSaka(ulong sensorFlags) {
    mGrounded = false;
    u32 uVar13 = mFlags & 0x6000;
    float yOffset;
    switch (mpSensorFoot->mFlags & SENSOR_TYPE_MASK) {
        case SENSOR_IS_POINT:
            yOffset = mpSensorFoot->asPointSensor()->mY / 4096.0f;
            break;
        case SENSOR_IS_LINE:
            yOffset = mpSensorFoot->asLineSensor()->mOffset / 4096.0f;
            break;
    }
    ulong type;
    ulong kind;
    float x = mpOwnerPos->x;
    float y = -(mpOwnerPos->y + yOffset);
    int sakaType = _checkFootSakaType(x, y, &type, &kind);
    bool bVar9 = false;
    ulong type2 = 0;
    ulong kind2 = 0;
    float y2;
    if (sakaType == 0) {
        u32 iy = (int) y & ~0xf;
        y2 = iy + 16;
        _checkFootSakaType(x, y2, &type2, &kind2);
        if ((type2 & 0x8899) && (mFlags & 0x4000) && !(sensorFlags & 0x10000) && m_e2 == 1) {
            float shift = std::fmodf(y, 2.75f) - 16.0f;
            if (shift > -2.0f) {
                mpOwnerPos->y += shift;
            }
        }
    } else if (sakaType == 1 || sakaType == 3) {
        u32 iy = (int) y & ~0xf;
        y2 = iy - 0.1f;
        _checkFootSakaType(x, y2, &type2, &kind2);
        if ((type2 & 0x20) && (type2 & 0x8000) && (sakaType != 3)) {
            type2 &= ~BIT_FLAG(5);
        }
        if ((type2 & 0x20) && (type & 0x2000000)) {
            bVar9 = true;
        }
    }
    if (type2 & 0x20) {
        type = type2;
        kind = kind2;
        y = y2;
        sakaType = 2;
    }

    if (sakaType != 2 && sakaType != 3) {
        return FLAG_NONE;
    }

    u32 kindSub = kind & 0xff;
    float ym = std::fmod(x, 2.75f);
    u32 ty = toTileInt2(y);
    float dVar17 = l_saka_data[kindSub][0] + l_saka_data[kindSub][1] * ym + ty;
    float fVar7 = -(mpOwnerPos->y + yOffset);
    if (type & 0x8000) {
        float speed = std::fabs(mpOwnerSpeed->x) + 0.5f;
        if (speed < 1.0f) {
            speed = 1.0f;
        }
        float sy = mpOwnerSpeed->y;
        if (sy > speed) {
            return FLAG_NONE;
        }
        if (uVar13) {
            if (sy <= 0.0f) {
                fVar7 += 4.0f;
            }
            if (fVar7 < dVar17 - 1.0f) {
                return FLAG_NONE;
            }
        } else {
            if (fVar7 < dVar17 - 1.0f) {
                return false;
            }
            if (sy < -4.0f) {
                sy = -4.0f;
            }
            if (sy > 0.5f) {
                sy = 0.5f;
            }
            if (fVar7 > std::fabs(mpOwnerSpeed->x) + 1.0f + -sy + dVar17) {
                return FLAG_NONE;
            }
        }
    } else {
        if (mpOwnerSpeed->y <= 0.0f && uVar13) {
            fVar7 += 4.0f;
        }
        if (fVar7 < dVar17 - 0.6f) {
            return FLAG_NONE;
        }
    }

    if (!uVar13 && !bVar9) {
        if (sakaType == 3 && fVar7 > dVar17 + 8.0f) {
            return FLAG_NONE;
        }
        mVec3_c pos(mpOwnerLastPos->x, mpOwnerLastPos->y + yOffset, 0.0f);
        if (!checkGround(&pos, &pos.y, mLayer, 1, -1)) {
            return FLAG_NONE;
        }
        u32 unitType = getUnitType(pos.x, pos.y, mLayer);
        if (!(unitType & 0x8021)) {
            return FLAG_NONE;
        }
        if (pos.y < -(dVar17 + 16.0f)) {
            return FLAG_NONE;
        }
        int y = mpOwnerLastPos->y + yOffset;
        if (y < (int) pos.y || y > (int) (pos.y + 16.0f)) {
            return FLAG_NONE;
        }
    }

    int dir = getSakaDir();
    s16 prevAngle = mSlopeAngle;
    if (_checkSakaFly(kindSub, sensorFlags)) {
        return FLAG_NONE;
    }

    u32 res = 0x4000;
    if (kindSub == 10) {
        res = 0x2000;
    }
    if (type & 0x8000) {
        res |= 0x200000;
    }
    mSlopeType = 10;
    mSlopeAngle = l_saka_angle[mSlopeType];
    m_e2 = 2;
    if (!(sensorFlags & BIT_FLAG(31))) {
        mpOwnerPos->y = -dVar17 - yOffset;
        mSlopeType = kindSub;
        mSlopeAngle = l_saka_angle[kindSub & 0xff];
        if (kindSub != 10) {
            u8 dir = DIR_LR_R;
            if (mpOwnerSpeed->x < 0.0f) {
                dir = DIR_LR_L;
            }
            m_e2 = getSakaUpDown(dir);
        }
    }
    _checkFootAttr(kind);
    if (prevAngle != 0 && dir != getSakaDir()) {
        mPlayerFlags |= BIT_FLAG(2);
    }
    if (mFootUnitType == 7) {
        mpOwnerSpeed->y = 0.0f;
    }
    mGrounded = true;
    mLastX = x;
    mLastY = y;
    if (mLastUnitType == 0) {
        mLastUnitType = type;
    }

    return res;
}

u32 dBc_c::_checkSakaFly(ulong subkind, ulong sensorFlags) {
    u8 dir = DIR_LR_R;
    if (mpOwnerSpeed->x < 0.0f) {
        dir = DIR_LR_L;
    }
    if (mFlags & 0x4000) {
        if (sensorFlags & 0x10000 && m_e2 == 1 && dir == mDirection && getSakaUpDown(dir, subkind) != 1) {
            return 0x2000;
        } else {
            return 0x0;
        }
    } else {
        if (l_saka_type[subkind] == 4 && nw4r::math::FAbs(mpOwnerSpeed->x) > 1.5f && l_saka_dir[subkind] == dir) {
            return 0x2000;
        } else {
            return 0x0;
        }
    }
}

void dBc_c::_attrNormal() {}

void dBc_c::_attrNuma() {
    if (!(mpSensorFoot->mFlags & BIT_FLAG(12)) && mpOwnerSpeed->y <= 0.0f) {
        mPushForce.y = -0.21f;
    }
}

void dBc_c::_attrRight() {
    if (mLastUnitKind & 1) {
        mPushForce.x = 1.0f;
    } else {
        mPushForce.x = 0.5f;
    }
}

void dBc_c::_attrLeft() {
    if (mLastUnitKind & 1) {
        mPushForce.x = -1.0f;
    } else {
        mPushForce.x = -0.5f;
    }
}

u32 dBc_c::checkHead(ulong flags) {
    if (mpSensorHead == nullptr) {
        return FLAG_NONE;
    }

    gUnitX = 0xffff;
    gUnitY = 0xffff;

    u32 res = 0;
    mHeadPrevUnitType = mHeadUnitType;
    mHeadUnitType = 0x10;
    mHeadAttr = 0;
    m_e0 &= ~BIT_FLAG(3);

    if (mpSensorHead != nullptr) {
        int sensorFlags = mpSensorHead->mFlags;
        if (flags & 0x1fe000) {
            if ((flags & 0x20000)) {
                sensorFlags |= 0x80000000;
            } else if (flags & 0x10000) {
                sensorFlags |= 0x20000000;
            } else {
                u32 tmp = sensorFlags | 0x80000000;
                if ((flags & 0x8000) != 0) {
                    tmp = sensorFlags | 0x40000000;
                }
                sensorFlags = tmp;
            }
        }

        res = _checkHeadSaka(sensorFlags);
        if (res == 0) {
            mFlippedSlopeType = SAKA_NONE;
            mFlippedSlopeAngle = l_saka_angle[mFlippedSlopeType];
            switch (sensorFlags & 1) {
                case SENSOR_IS_POINT:
                    res = checkHeadPoint(mpSensorHead->asPointSensor(), sensorFlags);
                    break;
                case SENSOR_IS_LINE:
                    res = checkHeadLine(mpSensorHead->asLineSensor(), sensorFlags);
                    break;
            }
            if (!(sensorFlags & BIT_FLAG(2))) {
                u32 checkRes = (this->*checkObjHead)(sensorFlags, 0.0f);
                if (checkRes != 0) {
                    res &= 0xc3ffffff;
                    res |= checkRes;
                }
            }
        }
    }

    if (m_e0 & 0x80) {
        m_e0 &= ~(BIT_FLAG(3) | BIT_FLAG(7));
    }
    res &= 0xfc000000;
    mFlags = (mFlags & 0x3ffffff) | res;

    return res;
}


u32 dBc_c::checkHeadPoint(sBcSensorPoint *sensor, ulong p) {
    mVec2_c v(mpOwnerPos->x + sensor->mX / 4096.0f, mpOwnerPos->y + sensor->mY / 4096.0f);
    u32 res = _checkHead(v.x, v.y, p) & 0x7fffffff;
    gUnitX = 0xffff;
    gUnitY = 0xffff;
    res |= _checkHeadBlock(v.x, v.y, p);
    _checkHeadRev(res, v.y, p);
    return res;
}

u32 dBc_c::checkHeadLine(sBcSensorLine *sensor, ulong p) {
    u32 res = 0;
    float dist = sensor->mOffset / 4096.0f;
    float x, y;
    if (sensor->mLineA < sensor->mLineB) {
        x = sensor->mLineA / 4096.0f;
        y = sensor->mLineB / 4096.0f;
    } else {
        x = sensor->mLineB / 4096.0f;
        y = sensor->mLineA / 4096.0f;
    }
    float currX = x;
    int count1 = 0;
    int count2 = 0;
    mVec2_c v;
    do {
        v.x = mpOwnerPos->x + currX;
        v.y = mpOwnerPos->y + dist;
        res |= _checkHead(v.x, v.y, p);
        if (res & 0x40000000) {
            count2++;
        }
        if (res & 0x3c000000) {
            count1++;
        }
    } while (!checkManyPoint(&currX, y));
    gUnitX = 0xffff;
    gUnitY = 0xffff;
    if (res & 0x80000000) {
        res &= 0x7fffffff;
        x -= 1.0f;
        y += 1.0f;
    }
    currX = x;
    do {
        v.x = mpOwnerPos->x + currX;
        v.y = mpOwnerPos->y + dist;
        res |= _checkHeadBlock(v.x, v.y, p);
        if (res & 0x40000000) {
            count2++;
        }
    } while (!checkManyPoint(&currX, y));
    _checkHeadRev(res, mpOwnerPos->y + dist, p);
    if (count1 != count2) {
        res &= ~BIT_FLAG(30);
    }
    return res;
}

u32 dBc_c::_checkHead(float x, float y, ulong sensorFlags) {
    if (!_isDifferentPos(x, y)) {
        return FLAG_NONE;
    }

    u32 type = getUnitType(x, y, mLayer);
    u32 kind = getUnitKind(x, y, mLayer);
    if (type == 0 && kind == 0) {
        return FLAG_NONE;
    }
    mVec3_c pos(x, y, 0.0f);

    if (isBGCoin(type)) {
        getBGCoin(type, kind, sensorFlags, &pos, 1);
        return FLAG_NONE;
    }

    if (setWakuCoinAppear(type, kind, sensorFlags, &pos)) {
        return FLAG_NONE;
    }

    if (mpOwnerSpeed->y > 0.0f && _checkBreakUnit(sensorFlags, 2, x, y)) {
        return 0x40000000;
    }

    setBgDamageFlag(3, x, y);

    u32 res = 0;
    if (type & 0x4899) {
        if ((type & 0x14)) {
            res |= 0x80000000;
        }
        bool a;
        if ((type & 0x10) && (sensorFlags & 0x800000)) {
            a = true;
        } else {
            a = false;
        }

        if (a && (sensorFlags & 0x10000000)) {
            return FLAG_NONE;
        }

        if (!(type & 0x2000) || mpOwnerSpeed->y > 0.0f) {
            switch (checkHitMameBlock(x, y, mLayer)) {
                case 0:
                    res |= BIT_FLAG(28);
                    _checkHeadAttr(kind);
                    break;
                case 2:
                    res |= BIT_FLAG(26);
                    _checkHeadAttr(kind);
                    break;
            }
        }
    }

    return res;
}

u32 dBc_c::_checkHeadBlock(float x, float y, ulong sensorFlags) {
    if (!_isDifferentPos(x, y)) {
        return FLAG_NONE;
    }

    u32 type = getUnitType(x, y, mLayer);
    u32 kind = getUnitKind(x, y, mLayer);

    int iy = -y;
    if ((type & 0x2000) && !(mpOwnerSpeed->y > 0.0f && (iy & 0xf) >= 8)) {
        return FLAG_NONE;
    }

    if (mpOwnerSpeed->y < 0.0f && !(sensorFlags & 0x4000000)) {
        return FLAG_NONE;
    }

    u32 res = 0;
    if (setHatenaBlockHit(sensorFlags, 2, x, y)) {
        res |= FLAG_31 | FLAG_26;
    }
    res |= setBreakBlockHit(sensorFlags, 2, x, y);

    return res;
}

void dBc_c::_checkHeadRev(ulong flags, float y, ulong sensorFlags) {
    if (sensorFlags & BIT_FLAG(31)) {
        return;
    }

    if (flags & 0x14000000) {
        int mask = 0xf;
        if (flags & 0x10000000) {
            mask = 0x7;
        }
        float shift = ((s16) y & ~mask) + 0.1f - y;
        if (shift < 0.0f) {
            mpOwnerPos->y += shift;
        }
    }
}

u32 dBc_c::_checkHeadSaka(ulong sensorFlags) {
    float yOffset;
    switch (mpSensorHead->mFlags & SENSOR_TYPE_MASK) {
        case SENSOR_IS_POINT:
            yOffset = mpSensorHead->asPointSensor()->mY / 4096.0f;
            break;
        case SENSOR_IS_LINE:
            yOffset = mpSensorHead->asLineSensor()->mOffset / 4096.0f;
            break;
    }
    float x = mpOwnerPos->x;
    float y = mpOwnerPos->y + yOffset;
    u32 type = getUnitType(x, y, mLayer);
    if (type & 1) {
        u16 iy = ((int) -y & 0xfff0) + 16;
        u32 type2 = _getUnitType(x, iy, mLayer);
        if (type2 & 0x40) {
            y = -(s16) iy;
            type = type2;
        }
    }

    if (type & 0x40) {
        u32 kind = getUnitKind(x, y, mLayer);
        u8 kindSub = kind & 0xff;
        float ym = std::fmod(x, 2.75f);
        float ty = (int) (y / 16.0f) * 16.0f;
        ty += -(l_head_saka_data[kindSub][0] + l_head_saka_data[kindSub][1] * ym);
        if (y >= ty - 0.1f) {
            if (!(sensorFlags & BIT_FLAG(31))) {
                mpOwnerPos->y = ty - yOffset;
                mFlippedSlopeType = kindSub;
                mFlippedSlopeAngle = l_head_saka_angle[kindSub];
            }
            u32 res = 0x8000000;
            if (kindSub == 10) {
                res = 0x4000000;
            }
            _checkHeadAttr(kind);
            return res;
        }
    }

    return FLAG_NONE;
}

u32 dBc_c::checkWall(float *speed) {
    if (mpSensorWall == nullptr) {
        return FLAG_NONE;
    }

    m_e0 &= ~(BIT_FLAG(0) | BIT_FLAG(1));
    float xSpeed;
    if (speed != nullptr) {
        xSpeed = *speed;
    } else {
        xSpeed = mpOwnerSpeed->x;
    }

    int dir = 0;
    if (xSpeed < 0.0f) {
        dir = 1;
    }

    return checkWallCommon(dir, 0);
}

u32 dBc_c::checkWallCommon(int dir, int sensorFlags) {
    if (mpSensorWall == nullptr) {
        return FLAG_NONE;
    }

    int flags = mpSensorWall->mFlags | sensorFlags;
    mWallPrevUnitType[dir] = mWallAttr[dir];
    mWallUnitType[dir] = 0x10;
    mWallAttr[dir] = 0;
    mAdjacentSlopeAngle = 0;
    u32 res = 0;
    switch (flags & 1) {
        case SENSOR_IS_POINT:
            res = checkWallPoint(mpSensorWall->asPointSensor(), dir, flags);
            break;
        case SENSOR_IS_LINE:
            res = checkWallLine(mpSensorWall->asLineSensor(), dir, flags);
            break;
    }
    if (!(flags & BIT_FLAG(2))) {
        u32 checkRes = (this->*checkObjWall)(flags, dir, 0.0f);
        if (checkRes != 0) {
            res = checkRes;
        }
    }
    static const u8 sc_BgDamageFlag[] = { 0x1, 0x2 };
    u8 flag = sc_BgDamageFlag[dir];
    if (m_e0 & 0x80) {
        m_e0 &= ~(flag | 0x80);
    }
    if (m_e0 & flag) {
        res = FLAG_NONE;
    }
    res &= 0x1fff;
    mFlags &= 0xffffe000;
    mFlags |= res;

    return res;
}

u32 dBc_c::checkWallPoint(sBcSensorPoint *sensor, int dir, ulong p) {
    float xOffset;
    if (dir == DIR_LR_L) {
        xOffset = -(sensor->mX / 4096.0f + 1.0f);
    } else {
        xOffset = sensor->mX / 4096.0f;
    }
    mVec2_c pos(mpOwnerPos->x + xOffset, mpOwnerPos->y + sensor->mY / 4096.0f);
    u32 res = _checkWall(pos.x, pos.y, p, dir);
    if (res & 0xc) {
        res |= 0x10 << dir;
    }
    return res;
}

u32 dBc_c::checkWallLine(sBcSensorLine *sensor, int dir, ulong p) {
    float y, x;
    if (sensor->mLineA < sensor->mLineB) {
        y = sensor->mLineA / 4096.0f;
        x = sensor->mLineB / 4096.0f;
    } else {
        y = sensor->mLineB / 4096.0f;
        x = sensor->mLineA / 4096.0f;
    }
    float dist;
    if (dir == DIR_LR_R) {
        dist = sensor->mOffset / 4096.0f;
    } else {
        dist = -(sensor->mOffset + 0x1000) / 4096.0f;
    }
    u32 res = 0;
    int bVar1 = 1;
    do {
        mVec2_c v;
        v.x = mpOwnerPos->x + dist;
        v.y = mpOwnerPos->y + y;
        u32 r = _checkWall(v.x, v.y, p, dir);
        res |= r;
        if (!(r & 0xc)) {
            bVar1 = 0;
        }
    } while (!checkManyPoint(&y, x));
    if (bVar1 == 1) {
        res |= 0x10 << dir;
    }
    return res;
}
