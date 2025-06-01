#include <game/bases/d_bg.hpp>
#include <game/bases/d_bg_global.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_rail.hpp>
#include <game/bases/d_p_switch_manager.hpp>
#include <game/mLib/m_heap.hpp>
#include <game/mLib/m_video.hpp>
#include <game/cLib/c_math.hpp>
#include <game/sLib/s_math.hpp>
#include <constants/sound_list.h>
#include <constants/game_constants.h>
#include <lib/MSL/string.h>

dHeapSize_c dBg_c::m_HeapSize(0x10000);
EGG::FrmHeap *dBg_c::m_FrmHeap_p = nullptr;
dBg_c *dBg_c::m_bg_p = nullptr;

void dBg_c::bg_createHeap() {
    if (m_FrmHeap_p != nullptr) {
        return;
    }
    m_FrmHeap_p = mHeap::createFrmHeap(
        m_HeapSize.mSize, EGG::Heap::sCurrentHeap,
        nullptr, 0x20, mHeap::OPT_NONE
    );
}

void dBg_c::bg_destroyHeap() {
    if (m_FrmHeap_p == nullptr) {
        return;
    }

    m_FrmHeap_p->destroy();
    m_FrmHeap_p = nullptr;
}

dBg_c::dBg_c() {
    m_bg_p = this;
    mBgCheckBuffer = nullptr;

    mBounds4[0] = 0.0f;
    mBounds4[1] = 0.0f;
    mBounds4[2] = 0.0f;
    mBounds4[3] = 0.0f;

    mBounds3[0] = 0.0f;
    mBounds3[1] = 0.0f;
    mBounds3[2] = 0.0f;

    mMoreFloats[1] = 0.0f;
    mMoreFloats[2] = 0.0f;
    mMoreFloats[3] = 0.0f;
    mMoreFloats[4] = 0.0f;
    mMoreFloats[5] = 0.0f;
    mMoreFloats[6] = 0.0f;
    mMoreFloats[7] = 0.0f;
    mMoreFloats[8] = 0.0f;
    mMoreFloats[9] = 0.0f;
    mMoreFloats[10] = 0.0f;
    mLoopOffsetX = 0.0f;
    mLoopOffsetY = 0.0f;
    mSomeParameterPos.set(0.0f, 0.0f);
    mMoreFloats7[1] = 0.0f;
    mMoreFloats7[2] = 0.0f;
    mMoreFloats[0] = 0.0f;
    mMoreFloats7[3] = 0.0f;
    mMoreFloats7[4] = 0.0f;
    mMoreFloats7[5] = 0.0f;
    mMoreFloats7[6] = 0.0f;

    for (int i = 0; i < (int) ARRAY_SIZE(mWaveRelated); i++) {
        mWaveRelated[i] = -999999.0f;
    }

    mDispScale = 1.0f;
    mMoreFloats3[0] = 1.0f;
    mMoreFloats3[1] = 1.0f;
    mMoreFloats3[2] = 0.0f;
    mZoomDenom = 1.0f;
    mMoreFloats3[3] = -999999.0f;
    mU8s[0] = 0;
    mU8s[1] = 0;
    mU8s[2] = 0;
    m_8ffbc = 0;
    mLLimit = 0.0f;
    mRLimit = 0.0f;
    mULimit = 0.0f;
    mDLimit = 0.0f;
    mLLimitSingle = 0.0f;
    mRLimitSingle = 0.0f;
    mULimitSingle = 0.0f;
    mDLimitSingle = 0.0f;
    mLLimitMulti = 0.0f;
    mRLimitMulti = 0.0f;
    mULimitMulti = 0.0f;
    mDLimitMulti = 0.0f;
    m_90008 = 0;
    m_90009 = 0;
    m_9000a = 0;
    mMoreFloats7[0] = 0.0f;
    m_90018 = 1.0f;
    m_9001c = 0;
    m_90020 = 0.0f;
    m_90024 = 0;
    mMoreFloats5[0] = 1.0f;
    mMoreFloats5[3] = 0.0f;
    mMoreFloats5[4] = 0.0f;
    m_9007c = 0;
    m_90080 = 0;
    m_90084 = 0.0f;
    m_90088 = 0;
    m_9008a = 0;
    mLimitRelated = 0;
    mLimitRelated2 = 0;
    m_90090 = 0;
    mScrollID = 0;
    m_90092 = 0;
    m_90093 = 0;
    m_900b6 = 0;
    m_900b4 = 0;

    mCallbackFunc = &dBg_c::fn_8007ac40;
    mTex[0] = nullptr;
    mTex[1] = nullptr;
    mTex[2] = nullptr;
    mTexProc = nullptr;
    initAutoScroll();
    m_9095c = 0;
    InitWmSwitchPushed();
}

dBg_c::~dBg_c() {
    m_bg_p = nullptr;
}

unsigned long long dBg_c::getBgCheckBuff(u16 idx) {
    return mBgCheckBuffer[idx];
}

unsigned long long *dBg_c::getpBgCheckBuff(u16 idx) {
    return &mBgCheckBuffer[idx];
}

unsigned long long dBg_c::fn_800774A0(u16 idx) {
    u32 idk = idx / 1024U;
    u32 bufIdx = idx % 1024U;
    unsigned long long cu = getBgCheckBuff(bufIdx);
    cu = fn_80081960(cu, idx);
    unsigned long long mask = (1ULL << 42);
    if (bufIdx > 0x100U) {
        u32 top = cu >> 32;
        u32 bottom = cu & 0xFFFFFFFF;
        // unsigned long long n = ((unsigned long long) top << 32) | (bottom & ~(0x400));
        if ((cu & mask) != 0 && idk != 0) {
            return ((unsigned long long) (top & ~(0x400)) << 32) | bottom;
        }
    }
    return cu;
}

dBgUnit_c *dBg_c::fn_80077520(u16 x, u16 y, u8 layer, int *param_5, bool b) {
    int file = dScStage_c::m_instance->mCurrFile;
    dBgUnit_c *bgUnit = dBgGlobal_c::ms_pInstance->GetBgUnitP(file, layer);
    return bgUnit->fn_80083b40(x, y, param_5, b);
}

u16 fn_80081ab0(u16);

u16 dBg_c::GetUnitNumber(u16 x, u16 y, u8 layer, bool p4) {
    dBgUnit_c *unit = fn_80077520(x, y, layer, 0, false);
    u16 res = -1;
    if (unit != nullptr) {
        if (p4) {
            res = fn_80081ab0(unit->mUnitNumber);
        } else {
            res = unit->mUnitNumber;
        }
    }
    return res;
}

u16 dBg_c::GetMaskedUnitNumber(u16 p1, u16 p2, u8 p3) {
    u16 data = GetUnitNumber(p1, p2, p3, true);
    if (data != (u16) -1) {
        data &= 0x3ff;
    }
    return data;
}

unsigned long long dBg_c::GetBgCheckData(u16 p1, u16 p2, u8 p3) {
    int loopPos = dScStage_c::getLoopPosX(p1);
    dBgUnit_c *data = fn_80077520(loopPos, p2, p3, 0, false);
    return fn_800774A0(data->mUnitNumber);
}

u32 dBg_c::GetUnitTypeInfo(u16 p1, u16 p2, u8 p3) {
    return GetBgCheckData(p1, p2, p3) >> 32;
}

u32 dBg_c::GetUnitKindInfo(u16 p1, u16 p2, u8 p3) {
    return GetBgCheckData(p1, p2, p3) & 0xFFFFFFFF;
}

u16 fn_800834b0(u16);

void dBg_c::CoinGetCommon(u16 p1, u16 p2, int p3, u16 *p4, u16 *p5, u16 *p6) {
    int currCourse = dScStage_c::m_instance->mCurrCourse;
    u16 var1 = (p2 & 0xf00) * 4;
    u16 var3 = p1 / 16 & 0x1f0;
    u8 var4 = p1 / 16;
    u8 var5 = p2 / 16 & 0xf;
    *p4 = dBgParameter_c::ms_Instance_p->getOneGetBuff(currCourse, var1 + var3 + var5, p3);
    *p5 = fn_800834b0(var4);
    *p6 = var1 + var3 + var5;
}

void dBg_c::CoinGetBitSet(u16 p1, u16 p2, int p3) {
    u16 c;
    u16 a;
    u16 b;
    CoinGetCommon(p1, p2, p3, &a, &b, &c);
    a |= b;
    dBgParameter_c::ms_Instance_p->setOneGetBuff(dScStage_c::m_instance->mCurrFile, c, p3, a);
}

bool dBg_c::CoinGetBitCheck(u16 p1, u16 p2, int p3) {
    u16 c;
    u16 a;
    u16 b;
    CoinGetCommon(p1, p2, p3, &a, &b, &c);
    return (a & b) != 0;
}

void dBg_c::BgUnitChange(u16 p1, u16 p2, int p3, u16 p4) {
    if (p4 == 2 && dPSwManager_c::ms_instance->checkSwitch(dPSwManager_c::SW_TYPE_0)) {
        p4 = 4;
    }
    int idk;
    dBgUnit_c *unit = fn_80077520(p1, p2, p3, &idk, true);
    if (idk != 0) {
        u16 smth = 0;
        if (dBgUnit_c::fn_80081900(unit->mUnitNumber, &smth)) {
            CoinGetBitSet(p1, p2, p3);
        }
        u16 n = dBgUnit_c::cvtNum(p4);
        if (n == 0xffff) {
            if (p4 == 138) {
                unit->mUnitNumber &= 0x3ff;
            } else if (p4 == 139) {
                unit->mUnitNumber |= 0x400;
            }
        } else {
            unit->mUnitNumber = n;
            mTex[p3]->setUnitChange(p1 / 16, p2 / 16, n);
        }
    }
}

bool dBg_c::CheckExistLayer(u8 layer) {
    dScStage_c *stage = dScStage_c::m_instance;
    dBgGlobal_c *g = dBgGlobal_c::ms_pInstance;
    switch (layer) {
        case 0:
            return true;
        case 1:
            if (g->mData[stage->mCurrFile].mLayer1 != 0) {
                return true;
            }
            break;
        case 2:
            if (g->mData[stage->mCurrFile].mLayer2 != 0) {
                return true;
            }
            break;
    }
    return false;
}

void dBg_c::CreateBgCheckBuffer() {
    mBgCheckBuffer = new(m_FrmHeap_p, 4) unsigned long long[0x400];

    int file = dScStage_c::m_instance->mCurrFile;

    char path[64];
    char resName[64];

    for (int i = 0; i < 4; i++) {
        char *env = dBgGlobal_c::ms_pInstance->getEnvironment(file, i);

        if (env[0] != '\0') {
            sprintf(path, "BG_chk/d_bgchk_%s.bin", env);
            sprintf(resName, "%s", env);

            nw4r::g3d::ResFile data = dResMng_c::m_instance->mRes.getRes(resName, path);
            for (int j = 0; j < 0x100; j++) {
                u64 *buff = getpBgCheckBuff(i * 0x100 + j);
                *buff = data.ofs_to_ptr_raw<u64>(0)[j];
            }
        }
    }
}

void dBg_c::DispScaleCalc() {
    float scale = mDispScale;
    if (scale == 0.0f) {
        scale = 1.0f;
    }
    dBgParameter_c::ms_Instance_p->mSize.x = mVideo::l_rayoutWidthF / scale;
    dBgParameter_c::ms_Instance_p->mSize.y = mVideo::l_rayoutHeightF / scale;
}

void dBg_c::CreateBgTex() {
    mTexMng.create(mHeap::g_gameHeaps[2]);

    u16 w = mL - mR;

    u16 h = mU - mD;

    u16 texWidthMaybe = mL / 16;
    u16 texHeightMaybe = -mU / 16;

    u16 iMu = mU;
    u16 iMd = mD;

    if ((w & 0xf) != 0) {
        w /= 16;
        w++;
    } else {
        w /= 16;
    }

    if ((h & 0xf) != 0) {
        h /= 16;
        h++;
    } else {
        h /= 16;
        if ((iMu & 0xF) != 0) {
            h++;
        }
        if ((iMd & 0xF) != 0) {
            h++;
        }
    }

    if (dScStage_c::m_loopType == 0) {
        w += 2;
        texWidthMaybe--;
    }
    h += 2;
    texHeightMaybe--;
    mTex[0] = __createBgTex(0, texWidthMaybe, texHeightMaybe, w, h, 0, 4);
    if (CheckExistLayer(1)) {
        mTex[1] = __createBgTex(1, texWidthMaybe, texHeightMaybe, w, h, 127, 3);
    }
    if (CheckExistLayer(2)) {
        mTex[2] = __createBgTex(2, texWidthMaybe, texHeightMaybe, w, h, 3, -1);
        mTexProc = new(m_FrmHeap_p, 4) dShareBgTexProc_c();
        mTexProc->create(mTex[2], m_FrmHeap_p);
    }
}

void dBg_c::RemoveBgTex() {
    delete mTexProc;
    mTexProc = nullptr;
    for (int i = 0; i < 3; i++) {
        delete mTex[i];
        mTex[i] = nullptr;
    }
}

bgTex_c *dBg_c::__createBgTex(int layer, u16 tw, u16 th, u16 w, u16 h, int i1, int i2) {
    bgTex_c *tex = nullptr;
    dScStage_c *stage = dScStage_c::m_instance;
    dBgGlobal_c *bgGlobal = dBgGlobal_c::ms_pInstance;
    dBgUnit_c *bgUnit = bgGlobal->GetBgUnitP(stage->mCurrFile, layer);
    if (bgUnit != nullptr) {
        tex = new(m_FrmHeap_p, 4) bgTex_c();
        tex->create(tw, th, w, h, i1, i2, mHeap::g_gameHeaps[0], layer);
        bool check = false;

        char *env = bgGlobal->getEnvironment(stage->mCurrFile, 1);
        if (env[0] != '\0' &&
               (strcmp(env, "Pa1_nohara") == 0
            || strcmp(env, "Pa1_nohara2") == 0
            || strcmp(env, "Pa1_daishizen") == 0)
        ) {
            check = true;
        }

        for (u16 y = 0; y < tex->mCount; y++) {
            u16 x = 0;
            if (tex->mSmth == 0) {
                continue;
            }
            unsigned int unitX, unitY;
            unitY = (y + th) * 16;
            while (x < tex->mSmth) {
                unitX = (x + tw) * 16;
                u16 unitNum = GetUnitNumber(unitX, unitY, layer, false);
                if (unitNum == 0xffff) {
                    unitNum = UNIT_0;
                }
                if (check && (UNIT_IS_GRASS(unitNum) || UNIT_IS_FLOWER(unitNum) || UNIT_IS_GRASS_FLOWER(unitNum))) {
                    int grassIdx = mGrassCount;
                    int flowerIdx = mFlowerCount;
                    dBgAnimObj_c animObj;
                    animObj.mX = unitX + 8;
                    animObj.mY = -(float) (unitY + 16);
                    if (UNIT_IS_GRASS(unitNum) || UNIT_IS_GRASS_FLOWER(unitNum)) {
                        if (UNIT_IS_GRASS(unitNum)) {
                            animObj.mType = unitNum - UNIT_GRASS_L;
                        } else {
                            animObj.mType = unitNum - UNIT_GRASS_FLOWER_1 + 1; // use center grass style
                        }
                        dBgAnimObj_c animObjGrass = animObj;
                        mGrass[grassIdx] = animObjGrass;
                        if (grassIdx < 100) {
                            grassIdx++;
                        }
                        mGrassCount = grassIdx;
                    }
                    if (UNIT_IS_FLOWER(unitNum) || UNIT_IS_GRASS_FLOWER(unitNum)) {
                        if (unitNum == 432 || unitNum == 433 || unitNum == 434) {
                            animObj.mType = unitNum - UNIT_FLOWER_1;
                        } else {
                            animObj.mType = unitNum - UNIT_GRASS_FLOWER_1;
                        }
                        dBgAnimObj_c animObjFlower = animObj;
                        mFlowers[flowerIdx] = animObjFlower;
                        // [BUG: should use flowerIdx here]
                        if (grassIdx < 100) {
                            flowerIdx++;
                        }
                        mFlowerCount = flowerIdx;
                    }
                    unitNum = UNIT_0;
                }
                tex->setOpaBuffer(x, y, unitNum);
                x++;
            }
        }
    }
    return tex;
}

u16 dBg_c::dBg_getUpLimitScroll(u8 layer) {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    u8 id = file->getAreaID(layer);
    int scrollID = file->getAreaScroll(id);
    return file->getScrollDataP(scrollID)->mUpLimit;
}

int dBg_c::dBg_getScrlAreaDataSize(u8 layer) {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    return file->mScrlAreaCount;
}

sScrollAreaData *dBg_c::dBg_getScrlAreaDataP(u8 layer, u8 idx) {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    return file->getScrlAreaDataP(idx);
}

void dBg_c::fn_80078250(u16 p1, u16 p2, u16 p3, u32 p4) {
    if (mBgThingsRelated > 255) {
        return;
    }
    sBgThing *thing = nullptr;
    for (int i = 0; i < 256; i++) {
        if (mBgThings[i].m_08 == 0) {
            thing = &mBgThings[i];
            break;
        }
    }
    thing->m_00 = p2;
    thing->m_02 = p3;
    thing->m_04 = p4;
    if (p1 == 42) {
        thing->m_08 = 28;
        thing->m_0c = 4;
    } else {
        thing->m_0c = 4;
        if (p1 == 40) {
            thing->m_08 = 32;
        } else {
            thing->m_08 = 56;
        }
    }
    fn_80077860(p2, p3, p4, 0);
}

void dBg_c::fn_80078300() {
    for (int i = 0; i < 256; i++) {
        sBgThing *thing = &mBgThings[i];
        if (thing->m_08 == 0) {
            continue;
        }
        // thing->m_08--;
        if (--thing->m_08 != 0) {
            continue;
        }
        fn_80077860(thing->m_00, thing->m_02, thing->m_04, thing->m_0c);
        mBgThingsRelated--;

        mVec2_c pos;
        pos.x = thing->m_00;
        pos.y = -thing->m_02;
        dAudio::SoundEffectID_t(SE_OBJ_COIN_APP).playMapSound(pos, 0);
    }
}

void dBg_c::setWaterInWave(float x, float y, u8 type) {
    for (int i = 0; i < 20; i++) {
        if (!mWaveActive[i]) {
            mWaveActive[i] = true;
            mWaveX[i] = x;
            mWaveY[i] = y;
            mWaveType[i] = type;
            return;
        }
    }
}

void dBg_c::setBubble(float x, float y, u8 type, u8 type2) {
    for (int i = 0; i < 200; i++) {
        if (mBubbleType[i] == 0) {
            mBubbleType[i] = type;
            mBubbleX[i] = x;
            mBubbleY[i] = y;
            mBubbleType2[i] = type2;
            return;
        }
    }
}

void dBg_c::InitWmSwitchPushed() {
    dInfo_c *info = dInfo_c::m_instance;
    mTexMng.mWmSwitchRelated = false;
    if (info->mIsWmSwitchHit) {
        mTexMng.mWmSwitchRelated = true;
    }
    if ((dInfo_c::mGameFlag & 0x10) == 0) {
        return;
    }
    mTexMng.mWmSwitchRelated = true;
}

bool dBg_c::IsWmSwitchPushed() {
    return mTexMng.mWmSwitchRelated;
}

dPanelObjMgr_c *dBg_c::getPanelObjMgr(int idx) {
    return &mTex[idx]->mPanelObjMgr;
}

bool dBg_c::dBg_isFlyPlayer() {
    bool res = false;
    u8 scrInfo = daPyMng_c::getActScrollInfo();
    for (int i = 0; i < 4; i++) {
        dAcPy_c *pl = daPyMng_c::getPlayer(i);
        if (pl == nullptr || ((scrInfo & (1 << i)) == 0)) {
            continue;
        }
        if (pl->mScrollMode == 2 || pl->mScrollMode == 6) {
            res = true;
        }
    }
    return res;
}

int dBg_c::dBg_isCloudFlyPlayer() {
    int res = false;
    u8 scrInfo = daPyMng_c::getActScrollInfo();
    for (int i = 0; i < 4; i++) {
        dAcPy_c *pl = daPyMng_c::getPlayer(i);
        if (pl == nullptr || ((scrInfo & (1 << i)) == 0)) {
            continue;
        }
        if (pl->mScrollMode == 6) {
            res = true;
        }
    }
    return res;
}

bool dBg_c::dBg_isCloudFlyPlayerMulti() {
    bool res = false;
    u8 scrInfo = daPyMng_c::getActScrollInfo();
    for (int i = 0; i < 4; i++) {
        dAcPy_c *pl = daPyMng_c::getPlayer(i);
        if (pl == nullptr || ((scrInfo & (1 << i)) == 0)) {
            continue;
        }
        if (pl->mScrollMode == 6) {
            dBgParameter_c *param = dBgParameter_c::ms_Instance_p;
            if (pl->mBgRelatedPos.y > param->mPos.y - 160.0f && mDispScale < 0.7f
                || mDispScale >= 0.7f && pl->mBgRelatedPos.y > param->mPos.y - 48.0f) {
                res = true;
            }
        }
    }
    return res;
}

float dBg_c::fn_80078860(float f) {
    float tmp = f;
    switch (mZoomRelated) {
        case  1: tmp = 19.0f / 14.0f; break;
        case 16: tmp = 10.0f / 7.0f; break;
        case  2: tmp = 12.0f / 7.0f; break;
        case  3: tmp = 2.0f; break;

        case  4: tmp = mZoomDenom; tmp = 1.0f / tmp; break;
        case  5: f = 1.0f; tmp = 0.7f; break;
        case  6:           tmp = 1.0f; break;
        case  7: f = 0.7f; tmp = 1.0f; break;
        case  8:           tmp = 1.0f; break;
        case  9:           tmp = 17.0f / 14.0f; break;

        case 10: f = 1.0f;        tmp = 1.0f; break;
        case 11: f = 1.0f;        tmp = 17.0f / 14.0f; break;
        case 12: f = 1.0f;        tmp = 19.0f / 14.0f; break;
        case 13: f = 1.0f;        tmp = 10.0f / 7.0f; break;
        case 14: f = 1.0f;        tmp = 12.0f / 7.0f; break;
        case 15: f = 1.0f;        tmp = 2.0f; break;

        case 17: f = 3.0f / 4.0f; tmp = 3.0f / 4.0f; break;
        case 18: f = 3.0f / 4.0f; tmp = 1.0f; break;
        case 19: f = 3.0f / 4.0f; tmp = 17.0f / 14.0f; break;
        case 20: f = 3.0f / 4.0f; tmp = 19.0f / 14.0f; break;
        case 21: f = 3.0f / 4.0f; tmp = 10.0f / 7.0f; break;
        case 22: f = 3.0f / 4.0f; tmp = 12.0f / 7.0f; break;
        case 23: f = 3.0f / 4.0f; tmp = 2.0f; break;
    }
    return f + mZoomWeight * (tmp - f);
}

float dBg_c::getMaxLeftPos() {
    float maxLeft = mL;
    if (mLimitRelated != 1 && mLimitRelated != 4 && 1.0f / getDispScale() < getZoomTargetMax_Common()) {
        maxLeft +=
            getZoomTargetMax_Common() * mVideo::getLayoutWidth() * 0.5f -
            1.0f / getDispScale()     * mVideo::getLayoutWidth() * 0.5f;
    }
    return maxLeft;
}

float dBg_c::getLeftLimit() {
    float maxLeft = mLLimit;
    if (mLimitRelated != 1 && mLimitRelated != 4 && 1.0f / getDispScale() < getZoomTargetMax_Common()) {
        maxLeft +=
            getZoomTargetMax_Common() * mVideo::getLayoutWidth() * 0.5f -
            1.0f / getDispScale()     * mVideo::getLayoutWidth() * 0.5f;
    }
    return maxLeft;
}

float dBg_c::getMaxRightPos() {
    float maxRight = mR;
    if (mLimitRelated != 1 && mLimitRelated != 4 && 1.0f / getDispScale() < getZoomTargetMax_Common()) {
        maxRight -=
            getZoomTargetMax_Common() * mVideo::getLayoutWidth() * 0.5f -
            1.0f / getDispScale()     * mVideo::getLayoutWidth() * 0.5f;
    }
    return maxRight;
}

float dBg_c::getRightLimit() {
    float maxRight = mRLimit;
    if (mLimitRelated != 1 && mLimitRelated != 4 && 1.0f / getDispScale() < getZoomTargetMax_Common()) {
        maxRight -=
            getZoomTargetMax_Common() * mVideo::getLayoutWidth() * 0.5f -
            1.0f / getDispScale()     * mVideo::getLayoutWidth() * 0.5f;
    }
    return maxRight;
}

float dBg_c::getLeftLimitSingle() {
    return mLLimitSingle;
}

float dBg_c::getLeftLimitMulti() {
    return mLLimitMulti;
}

float dBg_c::getZoomTargetMin_Common() {
    float zoomTargetMode1[] = {
        1.0f, 1.0f, 1.0f, 1.4285715f,
        1.3571428f, 1.2142857f, 1.2142857f, 1.2142857f,
        0.5f, 1.2142857f, 1.2142857f
    };
    float zoomTargetMode3[] = {
        1.0f, 1.3571428f, 1.7142857f, 2.0f,
        1.2142857f, 1.4285715f, 1.1428572f, 2.0f,
        0.5f, 0.75f
    };
    float zoomTargetMode2[] = {
        1.0f, 1.0f, 1.0f, 1.3571428f, 1.3571428f,
        1.3571428f, 1.2142857f, 1.2142857f
    };
    if (mLimitRelated == 0 || mLimitRelated == 1 || mLimitRelated == 6 || mLimitRelated == 7) {
        return zoomTargetMode1[mLimitRelated2];
    } else if (mLimitRelated == 2) {
        return zoomTargetMode2[mLimitRelated2];
    } else {
        return zoomTargetMode3[mLimitRelated2];
    }
}

float dBg_c::getZoomTargetMin() {
    return fn_80078860(getZoomTargetMin_Common());
}

float dBg_c::getZoomTargetMid_Common() {
    float zoomTargetMode1[] = {
        1.4285715f, 1.3571428f, 1.3571428f, 1.7142857f,
        1.7142857f, 1.7142857f, 1.7142857f, 1.4285715f,
        0.78571427f, 1.4642857f, 1.4285715f
    };
    float zoomTargetMode2[] = {
        1.3571428f, 1.3571428f, 1.3571428f, 1.3571428f,
        1.7142857f, 1.7142857f, 1.7142857f, 1.4642857f
    };
    if (mLimitRelated == 0 || mLimitRelated == 1 || mLimitRelated == 6 || mLimitRelated == 7) {
        return zoomTargetMode1[mLimitRelated2];
    } else if (mLimitRelated == 2) {
        return zoomTargetMode2[mLimitRelated2];
    } else {
        return getZoomTargetMin_Common();
    }
}

float dBg_c::getZoomTargetMid() {
    return fn_80078860(getZoomTargetMid_Common());
}

float dBg_c::getZoomTargetMax_Common() {
    float zoomTargetMode1[] = {
        1.3571428f, 1.7142857f, 2.0f, 1.7142857f,
        2.0f, 1.7142857f, 2.0f, 1.4285715f,
        1.0f, 1.7142857f, 2.0f
    };
    float zoomTargetMode2[] = {
        1.3571428f, 1.7142857f, 2.0f, 1.7142857f,
        2.0f, 2.0f, 2.0f, 1.7142857f
    };
    float res;
    if (mLimitRelated == 0 || mLimitRelated == 1 || mLimitRelated == 6 || mLimitRelated == 7) {
        res = zoomTargetMode1[mLimitRelated2];
    } else if (mLimitRelated == 2) {
        res = zoomTargetMode2[mLimitRelated2];
    } else {
        res = getZoomTargetMin_Common();
    }
    float lWidth = mVideo::l_rayoutWidthF;
    float ofsX = mLoopOffsetX;
    if (res * lWidth > ofsX) {
        res = mLoopOffsetX / mVideo::l_rayoutWidthF;
    }
    float lHeight = mVideo::l_rayoutHeightF;
    float ofsY = mLoopOffsetY;
    if (res * lHeight > ofsY) {
        res = mLoopOffsetY / mVideo::l_rayoutHeightF;
    }
    return res;
}

float dBg_c::getZoomTargetMax() {
    float res = fn_80078860(getZoomTargetMax_Common());
    float lWidth = mVideo::l_rayoutWidthF;
    float ofsX = mLoopOffsetX;
    if (res * lWidth > ofsX) {
        res = mLoopOffsetX / mVideo::l_rayoutWidthF;
    }
    float lHeight = mVideo::l_rayoutHeightF;
    float ofsY = mLoopOffsetY;
    if (res * lHeight > ofsY) {
        res = mLoopOffsetY / mVideo::l_rayoutHeightF;
    }
    mZoomDenom = 1.0 / res; // [1.0f was not used]
    return res;
}

float dBg_c::getZoomSpreadLine() {
    float zoomSpreadMode1[] = {
        0.55, 0.54, 0.54, 0.75, 0.72, 0.63, 0.63, 0.62, 0.26, 0.62, 0.62
    };
    float zoomSpreadMode2[] = {
        0.54, 0.54, 0.54, 0.72, 0.72, 0.72, 0.63, 0.62
    };
    float res = 0.0f;
    if (mLimitRelated == 0 || mLimitRelated == 1 || mLimitRelated == 6 || mLimitRelated == 7) {
        res = zoomSpreadMode1[mLimitRelated2];
    } else if (mLimitRelated == 2) {
        res = zoomSpreadMode2[mLimitRelated2];
    }
    return res;
}

float dBg_c::getZoomSpreadLine2() {
    float zoomSpreadMode1[] = {
        0.55, 0.72, 0.73, 0.75, 0.98, 0.63, 0.98, 0.62, 0.34, 0.78, 0.77
    };
    float zoomSpreadMode2[] = {
        0.54, 0.72, 0.73, 0.72, 0.98, 0.98, 0.98, 0.78
    };
    float res = 0.0f;
    if (mLimitRelated == 0 || mLimitRelated == 1 || mLimitRelated == 6 || mLimitRelated == 7) {
        res = zoomSpreadMode1[mLimitRelated2];
    } else if (mLimitRelated == 2) {
        res = zoomSpreadMode2[mLimitRelated2];
    }
    return res;
}

float dBg_c::getZoomShrinkLine() {
    float zoomSpreadMode1[] = {
        0.45, 0.39, 0.39, 0.6, 0.58, 0.49, 0.49, 0.48, 0.19, 0.48, 0.48
    };
    float zoomSpreadMode2[] = {
        0.39, 0.39, 0.39, 0.58, 0.58, 0.58, 0.49, 0.48
    };
    float res = 0.0f;
    if (mLimitRelated == 0 || mLimitRelated == 1 || mLimitRelated == 6 || mLimitRelated == 7) {
        res = zoomSpreadMode1[mLimitRelated2];
    } else if (mLimitRelated == 2) {
        res = zoomSpreadMode2[mLimitRelated2];
    }
    return res;
}

float dBg_c::getZoomShrinkLine2() {
    float zoomSpreadMode1[] = {
        0.45, 0.58, 0.59, 0.6, 0.83, 0.49, 0.83, 0.48, 0.26, 0.62, 0.64
    };
    float zoomSpreadMode2[] = {
        0.39, 0.58, 0.59, 0.58, 0.83, 0.83, 0.83, 0.62
    };
    float res = 0.0f;
    if (mLimitRelated == 0 || mLimitRelated == 1 || mLimitRelated == 6 || mLimitRelated == 7) {
        res = zoomSpreadMode1[mLimitRelated2];
    } else if (mLimitRelated == 2) {
        res = zoomSpreadMode2[mLimitRelated2];
    }
    return res;
}

void dBg_c::calcLookatOffs() {
    dAcPy_c *player = daPyMng_c::getPlayer(daPyMng_c::mCtrlPlrNo);
    if (player != nullptr) {
        DispScaleCalc();
        calcLookatOffsLimit();
        calcLookatOffsX(player);
        calcLookatOffsY(player);
    }
}

void dBg_c::calcLookatOffsLimit() {
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    float scale = mDispScale;
    if (scale == 0.0f) {
        scale = 1.0f;
    }
    mBounds4[2] = 32.0f / scale;
    mBounds4[3] = 32.0f / scale;
    float smth1 = 80.0f;
    float smth2 = 72.0f;
    if (dBg_isCloudFlyPlayer() == 1 && (!mU8s[4] || mAutoscrolls[0].mActive)) {
        smth1 = 88.0f;
        smth2 = 88.0f;
        smth1 += file->getScrollDataP(mScrollID)->mXCloud;
        smth2 -= file->getScrollDataP(mScrollID)->mYCloud;
    } else {
        smth1 += file->getScrollDataP(mScrollID)->mX;
        smth2 -= file->getScrollDataP(mScrollID)->mY;
    }
    if (mU8s[4] && !mAutoscrolls[0].mActive) {
        smth1 += (int) file->getScrollDataP(mScrollID)->m_16;
        smth2 -= (int) file->getScrollDataP(mScrollID)->m_18;
    }
    smth1 += mBounds3[3];
    smth2 -= mBounds3[3];
    sLib::addCalc(&mBounds3[0], smth1, 0.1f, 10.0f, 0.001f);
    sLib::addCalc(&mBounds3[1], smth2, 0.1f, 10.0f, 0.001f);
}

void dBg_c::calcLookatOffsX(dActor_c *actor) {
    float deltaX = actor->mPosDelta.x;
    deltaX = 0.333f * deltaX;

    float smth = m_8fda8;

    if (deltaX == 0.0f) {
        return;
    }

    if (deltaX - smth > 0.04f) {
        deltaX = smth + 0.04f;
    } else if (deltaX - smth < -0.04f) {
        deltaX = smth - 0.04f;
    }

    if (deltaX > 1.6f) {
        deltaX = 1.6f;
    } else if (deltaX < -1.6f) {
        deltaX = -1.6f;
    }

    float b40 = mBounds4[0];
    float b42 = mBounds4[2];
    float smth3 = b40 + deltaX;
    mBounds4[0] = smth3;
    if (smth3 > b42) {
        deltaX = b42 - b40;
        mBounds4[0] = b42;
    } else if (smth3 < -mBounds4[3]) {
        mBounds4[0] = -mBounds4[3];
        deltaX = mBounds4[0] - b40;
    }

    m_8fda8 = deltaX;
}

void dBg_c::calcLookatOffsY(dActor_c *actor) {
    dAcPy_c *pl = (dAcPy_c *) actor;
    float deltaY = pl->mBgRelatedPos.y - mPlayerPosY;
    float halfHeight2 = mSomeSize.y / 2 - mBounds3[0];
    float halfHeight = mSomeSize.y / 2 - mBounds3[1];
    if (deltaY > 0.0f) {
        deltaY += mBounds4[1];
        mBounds4[1] = deltaY;
        if (deltaY >= halfHeight2) {
            mBounds4[1] = halfHeight2;
        }
        return;
    }
    if (deltaY < 0.0f) {
        halfHeight = -halfHeight;
        deltaY += mBounds4[1];
        mBounds4[1] = deltaY;
        if (halfHeight >= deltaY) {
            mBounds4[1] = halfHeight;
        }
        return;
    }
    if (mBounds4[1] >= halfHeight2) {
        mBounds4[1] = halfHeight2;
    }
    halfHeight = -halfHeight;
    if (mBounds4[1] <= halfHeight) {
        mBounds4[1] = halfHeight;
    }
}

void dBg_c::initAutoScroll() {
    dBgParameter_c *bgParam = dBgParameter_c::getInstance();
    dScStage_c *stage = dScStage_c::m_instance;

    mAutoscrolls[0].mActive = false;
    mAutoscrolls[0].mPos.x = bgParam->mPos.x;
    mAutoscrolls[0].mPos.y = bgParam->mPos.y;
    mAutoscrolls[0].m_0c = 0.0f;
    mAutoscrolls[0].m_10 = 0.0f;
    mAutoscrolls[0].m_14 = 0;
    mAutoscrolls[0].m_15 = 0;
    mAutoscrolls[0].m_16 = 0;
    mAutoscrolls[0].m_17 = 0;
    mAutoscrolls[0].m_18 = false;
    mAutoscrolls[0].m_19 = 0;
    m_9007c = 0;
    m_90080 = 0;
    m_90084 = 0.0f;
    m_9008c = 0;
    m_9008d = 1;
    if (stage->mCurrWorld != WORLD_2) return;
    if (stage->mCurrCourse != STAGE_CASTLE) return;
    if (stage->mCurrFile != 0) return;
    mAutoscrolls[0].m_0c = 0.6f;
}

void dBg_c::calcLoopAutoScroll() {
    dBgParameter_c *bgParam = dBgParameter_c::ms_Instance_p;
    float maxX = 6000.0f;

    mVec3_c unitVec;
    mVec3_c pos;
    unitVec.x = 1.0f;
    unitVec.y = 0.0f;

    float actualY = bgParam->mSize.y + mDLimit;

    pos = mAutoscrolls[0].mPos;
    mSomeParameterPos = bgParam->mPos;

    if (m_90958 == 1) {
        if (bgParam->mPos.x > 2290.0f) {
            pos.x -= mLoopOffsetX;
        }
    } else if (m_90958 == 2) {
        if (bgParam->mPos.x > 3790.0f) {
            pos.x -= mLoopOffsetX;
        }
    } else if (m_90958 == 3) {
        if (bgParam->mPos.x > 5420.0f) {
            pos.x -= mLoopOffsetX;
        }
    }

    pos.y = actualY;

    float f2 = 0.6f;
    if (
        (bgParam->mPos.x > 675.0 && bgParam->mPos.x < 1100.0) ||
        (bgParam->mPos.x > 2280.0 && bgParam->mPos.x < 2700.0) ||
        (bgParam->mPos.x > 3820.0 && bgParam->mPos.x < 4300.0) ||
        (bgParam->mPos.x > 5420.0 && bgParam->mPos.x < 5900.0)
    ) {
        f2 = 4.8f;
    }
    if ((dActor_c::mExecStop & 8) == 0) {
        if (f2 <= 0.6f) {
            sLib::addCalc(&mAutoscrolls[0].m_0c, f2, 0.02f, 0.03f, 0.001f);
        } else {
            sLib::addCalc(&mAutoscrolls[0].m_0c, f2, 0.01f, 0.01f, 0.0001f);
        }
        pos.x += unitVec.x * mAutoscrolls[0].m_0c;
        pos.y += unitVec.y * mAutoscrolls[0].m_0c;
        pos.z = 0.0f;
    }
    if (pos.x > maxX) {
        pos.x = maxX;
    }
    mAutoscrolls[0].mPos = pos;
    mSomePos.x = pos.x;
    mSomePos.y = pos.y;
    float lWidth = mVideo::l_rayoutWidthF;
    float lHeight = mVideo::l_rayoutHeightF;
    float target = m_bg_p->getZoomTargetMin();
    mDispScale = 1.0f / target;
    mSomeSize.x = lWidth * (1.0f / mDispScale);
    mSomeSize.y = lHeight * (1.0f / mDispScale);
}

void dBg_c::calcAutoScroll() {
    dScStage_c *stage = dScStage_c::m_instance;
    mVec2_c bgSize = dBgParameter_c::getInstance()->mSize;
    sRailInfoData *ri = dRail_c::getRailInfoP(mAutoscrolls[0].m_14);
    dCdFile_c *file = dCd_c::m_instance->getFileP(stage->mCurrCourse);

    float leftLim = getLeftLimit();
    float rightLim = getRightLimit() - bgSize.x;
    float upLim = mULimit;
    float downLim = bgSize.y - mDLimit;
    bool cond1 = true;
    bool cond2 = true;
    if (stage->mCurrWorld == WORLD_2 && stage->mCurrCourse == STAGE_CASTLE && stage->mCurrFile == 0) {
        calcLoopAutoScroll();
    } else if ((dActor_c::mExecStop & 8) == 0 && m_9008c == 0) {
        sBgData *base = &file->mpBg2Data[ri->mNodeIdx];
        if (m_9008d != 0) {
            for (int i = 0; i < 999; i++) {
                int idx = m_9007c;
                // dBgThing_c *tmp = &base[m_9007c];
                if (m_90009 == 0) {
                    if (mAutoscrolls[0].mPos.x > base[idx].mID) {
                        m_9007c++;
                    } else {
                        break;
                    }
                }
            }
        }
        m_9008d = 0;
        mVec3_c asPos;
        asPos.x = mAutoscrolls[0].mPos.x;
        asPos.y = mAutoscrolls[0].mPos.y;
        asPos.z = 0.0f;
        rightLim = asPos.y;
        int tmp = m_9007c;
        if (m_90080 < 999) {
            sBgData *base = &file->mpBg2Data[ri->mNodeIdx];
            mVec3_c bgThingVec;
            bgThingVec.x = base->mID;
            bgThingVec.y = base->m_04;
            bgThingVec.z = 0.0f;
            mVec3_c asCopy = asPos;
            if (tmp > 0) {
                asCopy.x = (base - 1)->mID;
                asCopy.y = (base - 1)->m_04;
            }
            mVec3_c someVec = bgThingVec - asCopy;
            float calcArg = 0.0f;
            someVec.normalize();
            mVec3_c someVec2 = asCopy - asPos;
            someVec2.normalize();
            float len = someVec2.len();
            short ang1 = cM::atan2s(someVec2.y, len);
            short ang2 = cM::atan2s(someVec2.x, someVec2.z);
            if (m_90080 == 0 && m_9007c <= 1 || mAutoscrolls[0].m_15 != 0) {
                m_90088 = ang1;
                m_9008a = ang2;
            } else {
                sLib::addCalcAngle(&m_90088, ang1, 60, 250, 1);
                sLib::addCalcAngle(&m_9008a, ang2, 60, 250, 1);
            }
            mVec3_c idk;
            idk.x = mAng(m_90088).cos() * mAng(m_9008a).sin();
            idk.y = mAng(m_90088).sin();
            idk.z = mAng(m_90088).cos() * mAng(m_9008a).cos();
            sLib::addCalc(&mAutoscrolls[0].m_0c, asPos.y, 1.0f, calcArg, 0.0001f);
            mAutoscrolls[0].mPos.z = 0.0f;
            mAutoscrolls[0].mPos.x += idk.y * mAutoscrolls[0].m_0c;
            mAutoscrolls[0].mPos.y += idk.z * mAutoscrolls[0].m_0c;
            float dist = mAutoscrolls[0].mPos.distTo(bgThingVec);
            bool cond3 = false;
            bool cond4 = false;
            if (mAutoscrolls[0].m_18 == 1) {
                cond3 = true;
            } else if (mAutoscrolls[0].m_18 == 2) {
                cond4 = true;
            }
            if ((
                cond4 || someVec.x == 0.0f ||
                (someVec.x > 0.0f && bgThingVec.x <= mAutoscrolls[0].mPos.x) ||
                (someVec.x <= 0.0f && (mAutoscrolls[0].mPos.x <= bgThingVec.x))
            ) && (
                cond3 || someVec.y == 0.0f ||
                (someVec.y > 0.0f && bgThingVec.y <= mAutoscrolls[0].mPos.y) ||
                (someVec.y <= 0.0f && (mAutoscrolls[0].mPos.y <= bgThingVec.y))
            )) {
                m_9007c++;
                if (m_9007c >= ri->mCount) {
                    switch (mAutoscrolls[0].m_17) {
                        case 0:
                            AutoScroll_stop();
                            break;
                        case 1:
                            m_9007c = mAutoscrolls[0].m_16;
                            m_90080++;
                            break;
                        default:
                            m_9007c = ri->mCount - 1;
                            m_90080 = 999;
                            break;
                    }
                    return;
                }
            }
            if (mAutoscrolls[0].mPos.x < leftLim) {
                leftLim = mAutoscrolls[0].mPos.x;
                if (rightLim < leftLim) {
                    leftLim = leftLim;
                }
            }
            if (mAutoscrolls[0].mPos.y < downLim) {
                downLim = mAutoscrolls[0].mPos.y;
                if (upLim < downLim) {
                    downLim = downLim;
                }
            }
            switch (mAutoscrolls[0].m_18) {
                case 0:
                    cond1 = true;
                    cond2 = true;
                    break;
                case 1:
                    cond1 = true;
                    cond2 = false;
                    break;
                case 2:
                    cond1 = false;
                    cond2 = true;
                    break;
            }
            if (cond1) {
                rightLim = -999999.0f;
                for (int i = 0; i < 4; i++) {
                    dAcPy_c *pl = daPyMng_c::getPlayer(i);
                    if (pl != nullptr) {
                        upLim = pl->m_68;
                        if (upLim > rightLim) {
                            rightLim = upLim;
                        }
                    }
                }
                mSomePos.x = leftLim;
            }
            if (cond2) {
                mSomePos.y = downLim;
            }

            float lWidth = mVideo::l_rayoutWidthF;
            float lHeight = mVideo::l_rayoutHeightF;
            mDispScale = 1.0f / m_bg_p->getZoomTargetMin();
            mSomeSize.x = lWidth * (1.0f / mDispScale);
            mSomeSize.y = lHeight * (1.0f / mDispScale);
        }
    }
}

void dBg_c::AutoScroll_start() {
    mAutoscrolls[0].mActive = true;
}

void dBg_c::AutoScroll_stop() {
    mAutoscrolls[0].mActive = false;
}

void dBg_c::calcScroll(const mVec3_c &pos, int param_2) {
    dBgParameter_c *bgParam = dBgParameter_c::getInstance();
    float lWidth = mVideo::l_rayoutWidthF;
    float lHeight = mVideo::l_rayoutHeightF;
    if (m_bg_p->m_9095a == 0) {
        m_bg_p->getZoomTargetMax(); // unused return value
        if (mMoreFloats3[3] > 0.0f) {
            float bgVal = dBgParameter_c::getInstance()->m_30.y;
            float tmp2 = bgVal * (1.0f / mMoreFloats3[3] - 1.0f / m_bg_p->getZoomTargetMin());
            mDispScale = tmp2 + 1.0f / m_bg_p->getZoomTargetMin();
        }
        m_90018 = mDispScale;
        mMoreFloats5[1] = mDispScale;
        lWidth *= 1.0f / mDispScale;
        lHeight *= 1.0f / mDispScale;
        mSomeSize.x = lWidth;
        mSomeSize.y = lHeight;
        float leftLimit = getLeftLimit();
        float rightLimit = getRightLimit() - lWidth;
        float upLimit = mULimit;
        float downLimit = lHeight + mDLimit;
        bool cond1 = true;
        bool cond2 = true;
        float maxLeft = getMaxLeftPos();
        mSomeParameterPos = bgParam->mPos;
        mPlayerPosY = pos.y;
        float bounds40 = mBounds4[0];
        float bounds41 = mBounds4[1];
        float y = (pos.y + mSomeSize.y * 0.5f) - bounds41;
        float x = (pos.x + bounds40) - lHeight * 0.5f;
        float idkX = x;
        if (dScStage_c::m_loopType != 2) {
            if (y < idkX) {
                idkX = leftLimit;
            } else if (y > rightLimit) {
                idkX = rightLimit;
            }
        } else {
            if (dScStage_c::m_instance->mCurrWorld == WORLD_2 &&
                dScStage_c::m_instance->mCurrCourse == STAGE_CASTLE &&
                dScStage_c::m_instance->mCurrFile == 0 &&
                m_90958 == 0
            ) {
                if (y < idkX) {
                    idkX = leftLimit;
                } else if (y > rightLimit) {
                    idkX = rightLimit;
                }
            }
        }
        if (y < downLimit) {
            downLimit = y;
            if (upLimit > y) {
                downLimit = upLimit;
            }
        }
        if (mAutoscrolls[0].mActive) {
            switch (mAutoscrolls[0].m_18) {
                case 0:
                    cond1 = false;
                    cond2 = false;
                    break;
                case 1:
                    cond1 = false;
                    cond2 = true;
                    break;
                case 2:
                    cond1 = true;
                    cond2 = false;
                    break;
            }
        }
        if (cond1) {
            mSomePos.x = idkX;
        }
        if (cond2) {
            if (
                getAreaUpLimitScroll() > -999999.0f &&
                dBgParameter_c::getInstance()->mPos.y >= getAreaUpLimitScroll() &&
                downLimit > getAreaUpLimitScroll() &&
                dBg_isFlyPlayer() == 0
            ) {
                downLimit = getAreaUpLimitScroll();
            }
            if (param_2 != 0 || getAreaUpLimitScroll() <= -999999.0f) {
                mSomePos.y = downLimit;
            } else {
                sLib::chase(&mSomePos.y, downLimit, 16.0f);
            }
        }
        if (mLimitRelated == 1 || mLimitRelated == 4) {
            mSomePos.x = maxLeft + mLoopOffsetX * 0.5f - mSomeSize.x * 0.5f;
        }
        if (mLimitRelated == 6) {
            mSomePos.y = mD + mSomeSize.y;
        }
        mBounds4[0] = bounds40;
        mBounds4[1] = bounds41;
        if (!mAutoscrolls[0].mActive) {
            mAutoscrolls[0].mPos.x = mSomePos.x;
            mAutoscrolls[0].mPos.y = mSomePos.y;
        }
        if (m_900a7 != 0) {
            mSomePos.x = m_900ac - mSomeSize.x * 0.5f;
            mSomePos.y = m_900b0 + mSomeSize.y;
        }
    }
    fn_8007ca90(&mSomeInfo1, m_90009, 1);
    fn_8007ca90(&mSomeInfo1, m_9000a, 1);
    if (m_bg_p->m_9095a == 0) {
        fn_8007cd70(&mSomeInfo2, &mSomeInfo1, 1);
    }
}

void dBg_c::calcMultiScroll(int arg1) {
    int scrollNum = daPyMng_c::getScrollNum();
    fn_8007ca90(&mSomeInfo1, m_90009, scrollNum);
    fn_8007ca90(&mSomeInfo1, m_9000a, scrollNum);
    if (m_bg_p->m_9095a == 0) {
        fn_8007cd70(&mSomeInfo2, &mSomeInfo1, arg1);
        (this->*mCallbackFunc)(&mSomeInfo2, arg1);
    }
}

void dBg_c::fn_8007ac40(dBgSomeInfo_c *info, int arg1) {
    dBgBound_c b = info->mBounds;

    float lWidth = mVideo::l_rayoutWidthF;
    float lHeight = mVideo::l_rayoutHeightF;

    float invScale = 1.0f / mDispScale;
    float zoom = 1.0f / mZoomDenom;

    float tmpUp = std::fabs(b.mDown - b.mUp) / lHeight;
    float tmpRight = std::fabs(b.mLeft - b.mRight) / lWidth;

    float spL = m_bg_p->getZoomSpreadLine();
    float spL2 = m_bg_p->getZoomSpreadLine2();
    float shL = m_bg_p->getZoomShrinkLine();
    float shL2 = m_bg_p->getZoomShrinkLine2();
    float tgMin = m_bg_p->getZoomTargetMin();
    float tgMid = m_bg_p->getZoomTargetMid();
    float tgMax = m_bg_p->getZoomTargetMax();

    dScStage_c *stage = dScStage_c::m_instance;

    bool bVar7 = false;

    float scale;
    if (mLimitRelated == 1) {
        scale = 1.2f;
    } else {
        scale = 0.9f;
    }
    b.mUp = tmpUp * scale;
    mVec2_c r = fn_8007bd40(info, zoom, scale, &mWaveRelated[0x4d]);
    b.mRight = tmpRight;
    if (mLimitRelated == 1) {
        r.x = r.y;
        b.mRight = b.mUp;
    }
    if (mLimitRelated == 6 || mLimitRelated == 7) {
        r.y = r.x - 1.0f;
        b.mUp = b.mRight;
    }
    if (mLimitRelated == 2) {
        if ((mLimitRelated2 == 5 || mLimitRelated2 == 6) || (mLimitRelated2 == 1 || mLimitRelated2 == 2)) {
            if (invScale < m_bg_p->getZoomTargetMax() && spL2 < b.mRight) {
                b.mRight = spL2;
            }
            if (!dBg_isFlyPlayer() && mU8s[2] == 0 && spL2 < b.mUp) {
                b.mUp = spL2;
            }
        } else {
            if (invScale <= m_bg_p->getZoomTargetMid() && spL < b.mRight) {
                b.mRight = spL;
            }
            if (!dBg_isFlyPlayer() && mU8s[2] == 0 && spL < b.mUp) {
                b.mUp = spL;
            }
        }
    }
    if (mLimitRelated == 0 && mLimitRelated2 == 10) {
        if (dBg_isCloudFlyPlayerMulti() == 1) {
            b.mRight = spL2 + 0.01f;
        } else if (shL2 - 0.01 < b.mRight || m_9095b != 0) {
            b.mRight = shL2 - 0.01f;
        }
        b.mUp = b.mRight - 1.0f;
    }
    if (b.mUp < b.mRight) {
        b.mUp = b.mRight;
    }
    if ((daPyDemoMng_c::mspInstance->mFlags & 1) == 0) {
        if (m_90024 == 0) {
            if (b.mUp <= spL) {
                mMoreFloats5[0] = tgMin;
            } else {
                mMoreFloats5[0] = tgMid;
                m_90024 = 1;
            }
        } else if (m_90024 == 1) {
            if (b.mUp <= spL2) {
                if (shL <= b.mUp) {
                    if (spL <= b.mUp) {
                        mMoreFloats5[0] = tgMid;
                    }
                } else {
                    bVar7 = true;
                    if (m_900b6 > 120) {
                        mMoreFloats5[0] = tgMin;
                        m_90024 = 0;
                    }
                }
            } else {
                mMoreFloats5[0] = tgMax;
                m_90024 = 2;
            }
        } else if (m_90024 == 2) {
            if (shL2 <= b.mUp) {
                if (spL2 <= b.mUp) {
                    mMoreFloats5[0] = tgMax;
                }
            } else {
                bVar7 = true;
                if (m_900b6 > 120) {
                    mMoreFloats5[0] = tgMid;
                    m_90024 = 1;
                }
            }
        }
        if (mMoreFloats5[0] == tgMax) {
            if (mMoreFloats5[2] != tgMid ) {
                b.mRight = m_90018;
                if (mMoreFloats5[1] <= b.mRight) {
                    mMoreFloats5[1] = b.mRight;
                }
                mMoreFloats5[2] = tgMid;
            }
        } else {
            mMoreFloats5[2] = tgMin;
        }
        if (bVar7) {
            m_900b6++;
        } else {
            m_900b6 = 0;
        }
    } else {
        mMoreFloats5[0] = m_90018;
    }
    float fVar28 = mMoreFloats5[0];
    float fVar25 = m_90018;
    if (mMoreFloats5[0] > m_90018) {
        fVar25 = 0.25f;
        float calcArg = 0.0015f;
        float scale = 0.003f;
        if (mZoomRelated != 0 && mZoomRelated != 8) {
            fVar25 = 1.0f;
            calcArg = 0.1;
            scale = 1.0f;
        }
        if (m_9001c == 0) {
            sLib::addCalc(&m_90018, fVar28, fVar25, scale * (b.mUp * b.mUp * b.mUp * 6.0f + 1.0f), calcArg);
        }
        fVar25 = m_90018;
        if (fVar25 < mMoreFloats5[1]) {
            if (std::fabs(fVar28 - fVar25) > 0.1f) {
                m_9001c = 90;
            }
            m_90018 = mMoreFloats5[1];
        }
    } else if (m_90018 <= mMoreFloats5[0]) {
        mMoreFloats5[1] = mMoreFloats5[2];
    } else {
        fVar25 = 0.5f;
        float calcArg = 0.0015f;
        float scale = 0.003f;
        if (mZoomRelated != 0 && mZoomRelated != 8) {
            fVar25 = 1.0f;
            calcArg = 0.1;
            scale = 1.0f;
        }
        if (m_9001c == 0) {
            sLib::addCalc(&m_90018, fVar28, fVar25, scale * (b.mUp * b.mUp * b.mUp * 6.0f + 1.0f), calcArg);
        }
        mMoreFloats5[1] = mMoreFloats5[2];
    }
    if (m_9001c != 0) {
        m_9001c--;
    }
    if (zoom < m_90018) {
        m_90018 = zoom;
    }
    if (zoom > m_90018) {
        m_90018 = zoom;
    } else if (tgMax < m_90018) {
        m_90018 = tgMax;
    }
    float copy_90018 = m_90018;
    float val = -(mDLimit - mULimit);
    if (val < m_90018 - mVideo::getLayoutHeight()) {
        m_90018 = m_90018 / mVideo::getLayoutHeight();
    }
    if (getAreaUpLimitScroll() > -999999.0f) {
        float upLimit = getAreaUpLimitScroll();
        if (mU8s[2] == 0) {
            m_90964 = 1.0f;
        } else {
            sLib::addCalc(&m_90964, 0.0f, 0.5f, 0.01f, 0.01f);
        }
        copy_90018 += m_90964 * (val / fVar28 - copy_90018);
    }
    mPrevSomeSize.x = lWidth * copy_90018;
    mPrevSomeSize.y = lHeight * copy_90018;
    mDispScale = 1.0f / copy_90018;
    fVar28 *= 0.5f;
    float mLP = getMaxLeftPos();
    float mRP = getMaxRightPos();
    float w = (info->mBounds.mRight + info->mBounds.mLeft) * 0.5f;
    float u = mU;
    float d = mD;
    if (dScStage_c::m_loopType != 2) {
        if (mLP > w - fVar28) {
            w += mLP - (w - fVar28);
        } else if (mRP < w + fVar28) {
            w -= (w + fVar28) - mRP;
        }
    }
    w -= fVar28;
    if (mLimitRelated == 1 || mLimitRelated == 4) {
        w = mLP + mLoopOffsetX * 0.5f - mPrevSomeSize.x * 0.5f;
        if (
            dScStage_c::m_instance->mCurrWorld == 11 &&
            dScStage_c::m_instance->mCurrCourse == 65 &&
            dScStage_c::m_instance->mCurrFile == 1
        ) {
            w -= 48.0f;
        }
    }
    bool check = false;
    if (mLimitRelated != 1 && mLimitRelated != 4 && fn_8007bc40(info, copy_90018)) {
        fn_8007ba70(info);
        check = true;
    }
    float newW = w;
    if (dScStage_c::m_loopType != 2) {
        newW = getLeftLimit();
        float d = getRightLimit() - newW;
        if (newW <= w) {
            newW = w;
            if (d < w) {
                newW = d;
            }
        }
    }
    if (!check && dScStage_c::m_loopType != 1) {
        mPrevSomePos.x = newW;
    }
    int iVar9 = 0;
    float fnres = fn_8007bbf0(info);
    float fVar12 = 0.5f;
    if (fnres < mPrevSomePos.y && (m_90009 == 3 || m_9000a == 3)) {
        iVar9 = 16;
        fVar12 = fnres;
    }
    float fnres2 = fn_8007bba0(info);
    if (mPrevSomePos.y < fnres2) {
        if (m_90009 == 1 || m_9000a == 2) {
            iVar9 = 1;
            fVar12 = fnres2;
        } else if (m_9000a == 3 && iVar9 == 0) {
            int rounded = fnres2 - mPrevSomePos.y;
            mBounds3[3] = rounded;
        }
    }
    if (iVar9 != 0 && iVar9 == 17 && (m_90009 == 1 || m_9000a == 1)) {
        fVar12 = fnres2;
    }
    if (iVar9 != 0) {
        if (fVar12 < (int) mPrevSomePos.y) {
            float diff = mPrevSomePos.y - fVar12;
            if (diff < 16.0f) {
                mMoreFloats5[2] -= diff;
            } else {
                mMoreFloats5[2] -= 16.0f;
            }
        } else if ((int) mPrevSomePos.y < fVar12) {
            float diff = fVar12 - mPrevSomePos.y;
            if (diff < 16.0f) {
                mMoreFloats5[2] += diff;
            } else {
                mMoreFloats5[2] += 16.0f;
            }
        }
    }
    if (mPrevSomePos.y + 32.0f < info->mBounds.mUp) {
        info->mBounds.mUp = mPrevSomePos.y + 32.0f;
    }
    if (info->mBounds.mDown < mPrevSomePos.y - mPrevSomeSize.y - 32.0f) {
        info->mBounds.mDown = mPrevSomePos.y - mPrevSomeSize.y - 32.0f;
    }
    calcLookatOffsLimit();
}

void dBg_c::fn_8007ba70(dBgSomeInfo_c *info) {
    float fVar1 = info->mBounds.mLeft;
    float w = mPrevSomeSize.x;
    float fVar4;
    if (m_90009 == 2 || m_9000a == 2) {
        fVar4 = 112.0f - dBgParameter_c::getInstance()->m_78;
        fVar1 = info->mBounds.mRight;
    } else {
        fVar4 = w - (112.0f - dBgParameter_c::getInstance()->m_78);
    }
    fVar1 -= fVar4;
    if (fVar1 - mPrevSomePos.x > 6.0f) {
        fVar1 = mPrevSomePos.x + 6.0f;
    } else if (fVar1 - mPrevSomePos.x < -6.0f) {
        fVar1 = mPrevSomePos.x - 6.0f;
    }
    float ll = getLeftLimit();
    float d = getRightLimit() - w;
    float newW = fVar1;
    if (dScStage_c::m_loopType != 2) {
        newW = ll;
        if (fVar1 < newW) {
            newW = fVar1;
            if (d > w) {
                newW = d;
            }
        }
    }
    mPrevSomePos.x = newW;
}

float dBg_c::fn_8007bba0(dBgSomeInfo_c *info) {
    float tmp = 64.0f + info->mBounds.mUp;
    if (tmp - mPrevSomePos.y < -6.0f) {
        tmp = mPrevSomePos.y - 6.0f;
    }
    if (!(tmp > mULimit)) {
        return tmp;
    }
    return mULimit;
}

float dBg_c::fn_8007bbf0(dBgSomeInfo_c *info) {
    float sizeY = dBgParameter_c::getInstance()->mSize.y;
    float tmp = info->mBounds.mDown + (sizeY - 32.0f);
    if (tmp - mPrevSomePos.y > 6.0f) {
        tmp = mPrevSomePos.y + 6.0f;
    }
    if (!(tmp < sizeY + mDLimit)) {
        return tmp;
    }
    return mDLimit;
}

bool dBg_c::fn_8007bc40(dBgSomeInfo_c *info, float f) {
    float fVar1 = info->mBounds.mLeft;
    float fVar2 = info->mBounds.mRight;
    float ll = getLeftLimit();
    float rl = getRightLimit();
    float fVar4 = dBgParameter_c::m_78_offset();
    float fVar3 = fVar4;
    if (fVar1 < ll + fVar4) {
        fVar3 = fVar1 - ll;
    }
    if (fVar2 > rl - dBgParameter_c::m_78_offset()) {
        fVar4 = rl - fVar2;
    }
    if (f * mVideo::getLayoutWidth() - (fVar3 + fVar4) > info->mBounds.mRight - info->mBounds.mLeft) {
        return true;
    } else {
        return false;
    }
}

mVec2_c dBg_c::fn_8007bd40(dBgSomeInfo_c *info, float f1, float f2, float *f3) {
    mVec2_c result(0.0f, 0.0f);

    float fVar1;
    float fVar2;
    float fVar3;
    float fVar4;
    float fVar5;
    float fVar6;
    float fVar7;
    float fVar8;

    fVar5 = mVideo::l_rayoutHeightF;
    fVar4 = mVideo::l_rayoutWidthF;
    fVar7 = getLeftLimit();
    fVar8 = getRightLimit();
    fVar1 = 112.0f;
    fVar2 = mULimit;
    fVar3 = mDLimit;
    fVar6 = fVar1;
    if (f3[0] - 112.0f < fVar7) {
        fVar6 = f3[0] - fVar7;
    }
    if (fVar8 < f3[1] + 112.0f) {
        fVar1 = fVar8 - f3[1];
    }
    fVar7 = 1.0f;
    fVar4 = (fVar1 + fVar6 + (f3[1] - f3[0])) / fVar4;
    if ((fVar4 >= 1.0f) && (fVar7 = fVar4, f1 < fVar4)) {
        fVar7 = f1;
    }
    fVar1 = 88.0f;
    fVar4 = 80.0f;
    if (fVar2 < f3[2] + 88.0f) {
        fVar1 = fVar2 - f3[2];
    }
    if (f3[3] - 80.0f < fVar3) {
        fVar4 = f3[3] - fVar3;
    }
    fVar2 = 1.0f;
    fVar5 = ((fVar4 + fVar1 + (f3[2] - f3[3])) * f2) / fVar5;
    if ((fVar5 >= 1.0f) && (fVar2 = fVar5, f1 < fVar5)) {
        fVar2 = f1;
    }

    return result;
}

void dBg_c::initScroll() {
    dScStage_c *stage = dScStage_c::m_instance;
    dBgParameter_c *bgParam = dBgParameter_c::getInstance();

    float playerBoundL = -1e6f;
    float playerBoundR = 1e6f;
    float playerBoundU = 1e6f;
    float playerBoundD = -1e6f;

    for (int i = 0; i < 4; i++) {
        mVec3_c pos = daPyMng_c::getPlayerSetPos(stage->mCurrFile, stage->m_1211);
        float x = pos.x;
        float y = pos.y;
        if (x < playerBoundL) {
            playerBoundL = x;
        }
        if (x > playerBoundR) {
            playerBoundR = x;
        }
        if (y > playerBoundU) {
            playerBoundU = y;
        }
        if (y < playerBoundD) {
            playerBoundD = y;
        }
    }
    mSomeInfo2.mBounds.mLeft = playerBoundL;
    mSomeInfo2.mBounds.mRight = playerBoundR;
    mSomeInfo2.mBounds.mUp = playerBoundU;
    mSomeInfo2.mBounds.mDown = playerBoundD;
    mSomeInfo1.mBounds.mLeft = playerBoundL;
    mSomeInfo1.mBounds.mRight = playerBoundR;
    mSomeInfo1.mBounds.mUp = playerBoundU;
    mSomeInfo1.mBounds.mDown = playerBoundD;
    mIdkI = 0;
    mIdkF[0] = 0.0f;
    mIdkF[1] = 0.0f;
    mIdkF[2] = 0.0f;
    mIdkF[3] = 0.0f;
    mVec3_c plPos = daPyMng_c::getPlayerSetPos(stage->mCurrFile, stage->m_1211);
    mPlayerPosY = plPos.y;
    mBounds3[0] = 80.0f;
    mBounds3[1] = 72.0f;
    mBounds3[3] = 0.0f;
    calcLookatOffs();
    if (stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_7) {
        mBounds4[1] += 4.0f;
    }
    calcScroll(plPos, 1);

    mPrevSomePos = mSomePos;
    bgParam->mPos = mSomePos;

    m_90960 = mSomePos.y;

    mPrevSomeSize = mSomeSize;
    bgParam->mSize = mSomeSize;

    float halfWidth = bgParam->mSize.x * 0.5f;
    float halfHeight = bgParam->mSize.y * 0.5f;
    bgParam->mCenter.x = bgParam->mPos.x + halfWidth;
    bgParam->mCenter.y = bgParam->mPos.y - halfHeight;

    mU8s[3] = 0;
    mU8s[4] = 0;
    mU8s[5] = 0;

    if (!daPyMng_c::isScrollMode1()) {
        mMoreFloats5[3] = mSomePos.y;
    }
    m_9001c = 0;
    m_90018 = 1.0f;
    m_90024 = 0;
    mMoreFloats5[0] = -1.0f;
    mMoreFloats5[1] = 1.0f;
    mMoreFloats5[2] = 1.0f;
    mMoreFloats5[4] = 0.0f;
    mSomeParameterPos.x = dBgParameter_c::ms_Instance_p->mPos.x;
    mSomeParameterPos.y = dBgParameter_c::ms_Instance_p->mPos.y;

    dBgScrollLimit_c zeroLimit;
    zeroLimit.mL = 0.0f;
    zeroLimit.mR = 0.0f;
    zeroLimit.mU = 0.0f;
    zeroLimit.mD = 0.0f;
    zeroLimit.mL2 = 0.0f;
    zeroLimit.mR2 = 0.0f;
    zeroLimit.mU2 = 0.0f;
    zeroLimit.mD2 = 0.0f;
    zeroLimit.mL3 = 0.0f;
    zeroLimit.mR3 = 0.0f;
    zeroLimit.mU3 = 0.0f;
    zeroLimit.mD3 = 0.0f;
    zeroLimit.mL4 = 0.0f;
    zeroLimit.mR4 = 0.0f;
    zeroLimit.mU4 = 0.0f;
    zeroLimit.mD4 = 0.0f;
    zeroLimit.mFlags = 0;

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 8; j++) {
            for (int k = 0; k < 16; k++) {
                mScrLimit[i][j][k] = zeroLimit;
            }
        }
    }

    dBgSubstruct2_c emptySubstruct2;
    emptySubstruct2.mFloat1 = 0.0f;
    emptySubstruct2.mFloat2 = 0.0f;
    emptySubstruct2.mFloat3 = 0.0f;
    emptySubstruct2.mFloat4 = 0.0f;
    emptySubstruct2.mU8_1 = 99;
    emptySubstruct2.mU8_2 = 0;
    emptySubstruct2.mU8_3 = 0;

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 20; j++) {
            mData2[i][j] = emptySubstruct2;
        }
    }

    dBgAnimObj_c animObj;
    animObj.mX = 0.0f;
    animObj.mY = 0.0f;
    animObj.mType = -1;

    mGrassCount = 0;
    mFlowerCount = 0;

    for (int i = 0; i < 100; i++) {
        mGrass[i] = animObj;
    }

    for (int i = 0; i < 100; i++) {
        mFlowers[i] = animObj;
    }

    mZoomWeight = 0.0f;
    mZoomRelated = 0;
    m_900a5 = 0;
    m_900a6 = 0;
    m_900a7 = 0;
    m_900a8 = 0;
    m_900ac = 0.0f;
    m_900b0 = 0.0f;
    m_900b8 = 0.0f;
    m_900bc = 0.0f;

    for (int i = 0; i < 20; i++) {
        mWaveX[i] = 0.0f;
        mWaveType[i] = 0;
        mWaveActive[i] = false;
    }

    for (int i = 0; i < 200; i++) {
        mBubbleX[i] = 0.0f;
        mBubbleY[i] = 0.0f;
        mBubbleType[i] = 0;
        mBubbleType2[i] = 0;
    }

    m_90958 = 0;
    m_9095a = 0;
    m_9095b = 0;
    m_90964 = 1.0f;
}

float dBg_c::fn_8007c7d0(float x) {
    float res = x;
    float loopOffset = mLoopOffsetX;
    if (dScStage_c::m_loopType == 2) {
        if (dBgParameter_c::ms_Instance_p->posX() < getMaxLeftPos()) {
            float tmp1 = getMaxLeftPos() + (getMaxRightPos() - getMaxLeftPos()) * 0.5f;
            float bgW = dBgParameter_c::ms_Instance_p->width();
            float bgX = dBgParameter_c::ms_Instance_p->posX();
            bgW += getMaxLeftPos();
            if (bgX > bgW && tmp1 > x) {
                res -= loopOffset;
            }
        } else if (dBgParameter_c::ms_Instance_p->posX() > getMaxRightPos() - dBgParameter_c::ms_Instance_p->width()) {
            float tmp1 = getMaxLeftPos() + (getMaxRightPos() - getMaxLeftPos()) * 0.5f;
            float bgW = dBgParameter_c::ms_Instance_p->width();
            float bgX = dBgParameter_c::ms_Instance_p->posX();
            if (dBgParameter_c::ms_Instance_p->posX() < getMaxRightPos() && tmp1 < x) {
                res += loopOffset;
            }
        }
    }
    return res;
}

bool isRightPlayer(dAcPy_c *pl1, dAcPy_c *pl2) {
    float p1 = dBg_c::m_bg_p->fn_8007c7d0(pl1->mBgRelatedPos.x);
    float p2 = dBg_c::m_bg_p->fn_8007c7d0(pl2->mBgRelatedPos.x);
    return p1 > p2;
}

bool isLeftPlayer(dAcPy_c *pl1, dAcPy_c *pl2) {
    float p1 = dBg_c::m_bg_p->fn_8007c7d0(pl1->mBgRelatedPos.x);
    float p2 = dBg_c::m_bg_p->fn_8007c7d0(pl2->mBgRelatedPos.x);
    return p1 < p2;
}

bool isUpPlayer(dAcPy_c *pl1, dAcPy_c *pl2) {
    return pl1->mBgRelatedPos.y > pl2->mBgRelatedPos.y;
}

bool isDownPlayer(dAcPy_c *pl1, dAcPy_c *pl2) {
    return pl1->mBgRelatedPos.y < pl2->mBgRelatedPos.y;
}

void swap(dAcPy_c **pl1, dAcPy_c **pl2) {
    dAcPy_c *tmp = *pl1;
    *pl1 = *pl2;
    *pl2 = tmp;
}
