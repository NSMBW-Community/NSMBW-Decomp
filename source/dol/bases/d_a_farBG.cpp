
#include <game/bases/d_a_farBG.hpp>
#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_bg.hpp>
#include <game/framework/f_profile_name.hpp>
#include <game/framework/f_manager.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/cLib/c_math.hpp>
#include <game/bases/d_audio.hpp>
#include <constants/sound_list.h>
#include <game/bases/d_actor_manager.hpp>

daFarBG_HIO_c::daFarBG_HIO_c() : m_00(1.0f), m_04(1.0f), m_08(1.0f), m_0c(0.0f), m_10(0.0f), m_14(1.0f), m_18(0), m_19(0), m_1a(0), m_1b(0), m_1c(0), m_1d(0), m_1e(0), mShouldHideModel(false), m_20(0), m_21(0), mShouldNotSetCullingInfo(false), m_23(0), m_24(0xFFFF), m_26(0), m_27(0), m_28(0), m_29(0), m_2c(0), m_30(0), m_3c(0), m_3d(0), m_3e(0) {
    GXColor col1 = (GXColor){0xFF, 0xC8, 0x96, 0};
    GXColor col2 = (GXColor){0x05, 0x3C, 0x32, 0};

    m_34 = col1;
    m_38 = col2;

    for (int i = 0; i < (int)ARRAY_SIZE(m_3f); i++) {
        for (int j = 0; j < (int)ARRAY_SIZE(m_3f[0]); j++) {
            for (int k = 0; k < (int)ARRAY_SIZE(m_3f[0][0]); k++) {
                m_3f[i][j][k] = 1;
            }
        }
    }
}

BASE_PROFILE(FAR_BG, daFarBG_c)

int daFarBG_c::create() {
    c_PIC_WIDTH = 720.0f;
    c_PIC_HEIGHT = 768.0f;
    c_PIC_WIDTH_HALF = c_PIC_WIDTH / 2.0f;
    c_PIC_HEIGHT_HALF = c_PIC_HEIGHT / 2.0f;

    if (!initialize()) {
        return FAILED;
    }

    mAllocator.createFrmHeapToCurrent(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 32, mHeap::OPT_NONE);
    CreateHeap();
    mAllocator.adjustFrmHeapRestoreCurrent();

    CalcScale();
    CalcScrollBG();
    mExecStopMask = 0;

    return SUCCEEDED;
}

int daFarBG_c::execute() {
    UpdateAnim();
    CalcScale();
    CalcScrollBG();
    CalcBasePosAndMtx();
    ReserveModel();
    effectExecute();

    return SUCCEEDED;
}

int daFarBG_c::draw() {
    InitFrustum();

    for (int i = 0; i < (int)ARRAY_SIZE(mpBackgrounds); i++) {
        for (int j = 0; j < (int)ARRAY_SIZE(mpBackgrounds[0]); j++) {
            entryModel(mpBackgrounds[i][j]);
        }
    }

    if ((mpStaticBackground != nullptr) && (mpStaticBackground->mIsEnabled)) {
        entryModel(mpStaticBackground);
    }

    return SUCCEEDED;
}

int daFarBG_c::doDelete() {
    if (mAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 9; j++) {
                if (mpBackgrounds[i][j]->mModel != nullptr) {
                    mpBackgrounds[i][j]->mModel->remove();

                    if (mpBackgrounds[i][j]->mAnmChr != nullptr) {
                        mpBackgrounds[i][j]->mAnmChr->remove();
                    }

                    if (mpBackgrounds[i][j]->mAnmClr != nullptr) {
                        mpBackgrounds[i][j]->mAnmClr->remove();
                    }

                    if (mpBackgrounds[i][j]->mAnmSrt != nullptr) {
                        mpBackgrounds[i][j]->mAnmSrt->remove();
                    }
                }
            }
        }

        if (mpStaticBackground != nullptr) {
            if (mpStaticBackground->mModel != nullptr) {
                mpStaticBackground->mModel->remove();

                if (mpStaticBackground->mAnmChr != nullptr) {
                    mpStaticBackground->mAnmChr->remove();
                }

                if (mpStaticBackground->mAnmClr != nullptr) {
                    mpStaticBackground->mAnmClr->remove();
                }

                if (mpStaticBackground->mAnmSrt != nullptr) {
                    mpStaticBackground->mAnmSrt->remove();
                }
            }
        }

        mAllocator.destroyHeap();
    }

    return SUCCEEDED;
}

bool daFarBG_c::entryModel(mdlData_t *model) {
    if (!model->mIsEnabled) {
        return false;
    }

    nw4r::math::MTX34 mtx;

    model->mModel->getLocalMtx(&mtx);

    if (!m_HIO[mIsBgB].mShouldNotSetCullingInfo) {
        SetCullingInfo((m3d::smdl_c &) model->mModel);
    }

    if (!m_HIO[mIsBgB].mShouldHideModel) {
        model->mModel->entry();
    }

    return true;
}

bool daFarBG_c::initialize() {
    mIsBgB = mParam & 0xF;

    SetBgDataP();
    mRepeatType = GetRepeatType(
        GetMyBgDataP()->mFile1, GetMyBgDataP()->mFile2, GetMyBgDataP()->mFile3
    );

    if (mRepeatType == REPEAT_0) {
        return false;
    }

    RepeatType_e other_repeat = GetRepeatType(
        GetOtherBgDataP()->mFile1, GetOtherBgDataP()->mFile2, GetOtherBgDataP()->mFile3
    );

    if (other_repeat == REPEAT_0) {
        if (mIsBgB == 0) {
            mpBgEntries[1] = nullptr;
        } else {
            mpBgEntries[0] = nullptr;
        }
    }

    mOffset.x = GetMyBgDataP()->mXOffset;
    mOffset.y = GetMyBgDataP()->mYOffset;

    dActor_c *bg_center = (dActor_c *)fManager_c::searchBaseByProfName(fProf::BG_CENTER, nullptr);

    if (bg_center != nullptr) {
        mCenterXPos = bg_center->mPos.x;
        u32 mode = bg_center->mParam & 0xF;

        switch (mode) {
            case 0:
                mCenterMode = 0;
                break;
            case 1:
                mCenterMode = 1;
                break;
            case 2:
                mCenterMode = 2;
                break;
        }
    } else {
        mCenterMode = 2;
        mCenterXPos = 0.0f;
    }

    if (! checkResource()) {
        return false;
    }

    mScale.x = 1.0f;
    mScale.y = 1.0f;
    mScale.z = 1.0f;
    mBgScroll = 0.0f;
    m_4ec = 0.0f;
    m_4f0 = 0.0f;
    m_62c = 0;
    m_5f4[0] = 0xFFFF;
    m_5f4[1] = 0xFFFF;

    for (int i = 0; i < 100; i++) {
        m_658[i] = cM::rndInt(1 << 16);
    }

    return true;
}

nw4r::g3d::ResFile daFarBG_c::GetRes(unsigned short bg_id, char* arcName, char* resPath, char* out_arcName) {
    dResMng_c * inst = dResMng_c::m_instance;

    if (mIsBgB == 0) {
        sprintf(arcName, "bgA_%04X", bg_id);
        sprintf(resPath, "g3d/bgA_%04X.brres", bg_id);
        sprintf(out_arcName, "bgA_%04X", bg_id);
    } else if (mIsBgB == 1) {
        sprintf(arcName, "bgB_%04X", bg_id);
        sprintf(resPath, "g3d/bgB_%04X.brres", bg_id);
        sprintf(out_arcName, "bgB_%04X", bg_id);
    }

    return inst->getRes(arcName, resPath);
}

void daFarBG_c::CreateModel(unsigned short bg_id, daFarBG_c::mdlData_t* mdl_data, bool c) {
    char effectName[16];
    char arcName_out[64];
    char resPath[64];
    char arcName[64];

    nw4r::g3d::ResFile res = GetRes(bg_id, arcName, resPath, arcName_out);
    nw4r::g3d::ResMdl resMdl = res.GetResMdl(arcName_out);

    if (fn_80081BE0(1, 0x2a, 1)) {
        d3d::SetResTevColorAll(&resMdl, GX_TEVREG0, m_HIO[mIsBgB].m_34);
        d3d::SetResTevColorAll(&resMdl, GX_TEVREG1, m_HIO[mIsBgB].m_38);
    }

    nw4r::g3d::ResAnmClr resAnmClr = res.GetResAnmClr(resPath);
    nw4r::g3d::ResAnmChr resAnmChr = res.GetResAnmChr(resPath);

    if (&resAnmChr != nullptr) {
        m3d::mdl_c * model = new m3d::mdl_c();
        mdl_data->mModel = model;
        model->create(resMdl, &mAllocator, 0x60);

        for (int i = 0; i < resMdl.GetResNodeNumEntries(); i++) {
            if (!memcmp(resMdl.GetResNode(i).GetName(), "Trans", 6)) {
                mdl_data->mIsTranslation = true;
                break;
            }
        }

        m3d::anmChr_c * anmchr = new m3d::anmChr_c();
        mdl_data->mAnmChr = anmchr;
        anmchr->create(resMdl, resAnmChr, &mAllocator);
        mdl_data->mModel->setAnm(*mdl_data->mAnmChr);

        if (resAnmChr.GetNumFrame() == 0) {
            mdl_data->mAnmChr->setRate(0.0f);
            mdl_data->mAnmChr->mPlayMode = 1;
            mdl_data->m_11 = true;

            int j = 0;
            for (int i = 0; i < resMdl.GetResMatNumEntries(); i++) {
                nw4r::g3d::ResNode resNode = resMdl.GetResNode(i);

                const char * name;
                if (resNode.GetName() == nullptr) {
                    name = nullptr;
                } else {
                    name = resNode.GetName();
                }

                snprintf(effectName, sizeof(effectName), "Effect0%d_", j);

                if (strstr(name, effectName)) {
                    m_5f4[j] = i;
                    j++;
                }
            }
        } else {
            mdl_data->mAnmChr->setRate(1.0f);
            mdl_data->mAnmChr->mPlayMode = 0;
        }
    }

    if (c) {
        nw4r::g3d::ResAnmTexSrt resAnmTexSrt = res.GetResAnmTexSrt(arcName_out);

        if (&resAnmTexSrt != nullptr) {
            m3d::anmTexSrt_c * anm_tex_srt = new m3d::anmTexSrt_c();
            mdl_data->mAnmSrt = anm_tex_srt;
            anm_tex_srt->create(resMdl, resAnmTexSrt, &mAllocator, nullptr, 1);

            if (m_62c == 0) {
                mdl_data->mAnmSrt->setPlayMode(m3d::FORWARD_LOOP, 0);
            } else {
                mdl_data->mAnmSrt->setFrameStart(0.0f, 0);
                mdl_data->mAnmSrt->setRate(0.0f, 0);
                mdl_data->mAnmSrt->setPlayMode(m3d::FORWARD_ONCE, 0);
            }

            mdl_data->mModel->setAnm(*mdl_data->mAnmSrt);
        }

        nw4r::g3d::ResAnmClr resAnmClr2 = res.GetResAnmClr(arcName_out);

        if (&resAnmClr != nullptr) {
            m3d::anmMatClr_c * anm_mat_clr = new m3d::anmMatClr_c();
            mdl_data->mAnmClr = anm_mat_clr;
            anm_mat_clr->create(resMdl, resAnmClr2, &mAllocator, nullptr, 1);
            anm_mat_clr->setPlayMode(m3d::FORWARD_LOOP, 0);
            mdl_data->mModel->setAnm(*mdl_data->mAnmClr);
        }

        if (fn_80089030() == 0) {
            if (mIsBgB == 0) {
                mdl_data->mModel->setPriorityDraw(0x15, 1);
            } else {
                mdl_data->mModel->setPriorityDraw(0x13, 0);
            }
        } else {
            if (mIsBgB == 0) {
                mdl_data->mModel->setPriorityDraw(0xF, -1);
            } else {
                mdl_data->mModel->setPriorityDraw(0xE, -1);
            }
        }

        mdl_data->mModel->setScale(mScale);
        dActor_c::setSoftLight_Map(*mdl_data->mModel);
        mdl_data->mModel->setCallback(&mCallback);
    }
}


daFarBG_c::RepeatType_e daFarBG_c::GetRepeatType(u16 idA, u16 idB, u16 idC) {
    if (((idA == 0) && (idC == 0)) || (idB == 0)) {
        return REPEAT_0;
    }
    if ((idA == idB) && (idB == idC)) {
        return REPEAT_5;
    }
    if ((idA == idB) && (idB != idC) && (idC != 0)) {
        return REPEAT_1;
    }
    if ((idB == idC) && (idA != idB) && (idA != 0)) {
        return REPEAT_2;
    }
    if ((idC == 0) && (idA != idB) && (idA != 0)) {
        return REPEAT_3;
    }
    if ((idA == 0) && (idC != idB) && (idC != 0)) {
        return REPEAT_4;
    }
    if ((idA == idC) && (idA != 0) && (idC != 0)) {
        return REPEAT_6;
    }
    if ((idA != 0) && (idB != 0) && (idC != 0)) {
        return REPEAT_7;
    }

    return REPEAT_0;
}

void daFarBG_c::SetBgDataP() {
    mpBgEntries[0] = fn_80081C40(0);
    mpBgEntries[1] = fn_80081C40(1);
}

float daFarBG_c::GetHScrollSpdRatio(int arg) {
    sBgData *bg_data;

    if (arg == 0) {
        bg_data = GetMyBgDataP();
    } else {
        bg_data = GetOtherBgDataP();
    }

    u16 rate = bg_data->mXScrollRate;
    if (rate == 0xFFFF) {
        rate = 0;
    }

    return l_speed_ratiodt[rate];
}

float daFarBG_c::GetVScrollSpdRatio(int arg) {
    sBgData *bg_data;

    if (arg == 0) {
        bg_data = GetMyBgDataP();
    } else {
        bg_data = GetOtherBgDataP();
    }

    u16 rate = bg_data->mYScrollRate;
    if (rate == 0xFFFF) {
        rate = 0;
    }

    return l_speed_ratiodt[rate];
}

float daFarBG_c::GetScrollBaseY() {
    dBg_c * bg = dBg_c::m_bg_p;

    float ret = 0.0f;

    switch (mRepeatType) {
        case REPEAT_1:
            return bg->mZoneBottom;
        case REPEAT_2:
            return bg->mZoneTop;
        case REPEAT_3:
            return bg->mZoneTop;
        case REPEAT_4:
            return bg->mZoneBottom;
        case REPEAT_5:
            return bg->mZoneBottom;
        case REPEAT_6:
            return bg->mZoneBottom;
        case REPEAT_7:
            return bg->mZoneBottom;
        default:
            return ret;
    }
}

daFarBG_c::ScrollBaseType_e daFarBG_c::GetScrollBaseType() {
    switch (mRepeatType) {
        case REPEAT_4:
        case REPEAT_5:
        case REPEAT_6:
            return SCROLL_0;
        case REPEAT_2:
        case REPEAT_3:
            return SCROLL_1;
        default:
            break;
        case REPEAT_1:
            return SCROLL_0;
        case REPEAT_7:
            return SCROLL_2;
    }

    return SCROLL_0;
}

float daFarBG_c::GetModelBasePosX(int a, int b, float c) {
    float ret = 0.0f;

    switch ((int)mCenterMode) {
        case 0:
            ret = mCenterXPos + c * (float)(a - (b / 2));
            break;
        case 1:
            ret = mCenterXPos - c * (float)((int)(b - 1U) - a);
            break;
        case 2:
            ret = mCenterXPos + (float)(a - 1) * c;
            break;
    }

    switch ((int)mCenterMode) {
        case 0:
            m_5f8 = (b / 2);
            break;
        case 1:
            m_5f8 = (b - 1);
            break;
        case 2:
            m_5f8 = 0;
            break;
    }

    return ret;
}

const float l_offsetZ = 0.1f;

void daFarBG_c::fn_80116E60(int a, int b) {
    float f1 = c_PIC_HEIGHT * mScale.y;
    float f2 = c_PIC_WIDTH * mScale.x;

    int i4 = b - 1;

    for (int i = i4; i >= 0; i--) {
        int i6 = i4 - i;

        bgData_t * row = &mpBgData[i * 0x42];

        for (int j = 0; j < a; j++) {
            bgData_t * data = &row[j];
            data->m_0.x = GetModelBasePosX(j, b, f2);
            float scroll = GetScrollBaseY();
            data->m_0.y = (f1 * 0.5f - 256.0f) + f1 * (float)(i4 - i) + scroll;

            if (mIsBgB == 0) {
                data->m_0.z = -7000.0f;
            } else {
                data->m_0.z = -8000.0f;
            }

            data->m_0.z -= i6 * l_offsetZ;
            if (i == i4) {
                data->m_c = 2;
            } else {
                data->m_c = 1;
            }
        }
    }
}

void daFarBG_c::fn_80117030(int a, int b) {
    float f1 = c_PIC_HEIGHT * mScale.y;
    float f2 = c_PIC_WIDTH * mScale.x;

    for (int i = 0; i < b; i++) {
        bgData_t * row = &mpBgData[i * 0x42];

        for (int j = 0; j < a; j++) {
            bgData_t * data = &row[j];

            data->m_0.x = GetModelBasePosX(j, b, f2);
            float scroll = GetScrollBaseY();
            data->m_0.y = (256.0f - f1 * 0.5f) + (scroll - (float)i * f1);

            if (mIsBgB == 0) {
                data->m_0.z = -7000.0f;
            } else {
                data->m_0.z = -8000.0f;
            }

            data->m_0.z -= (i + j) * l_offsetZ;

            if (i == 0) {
                data->m_c = 0;
            } else {
                data->m_c = 1;
            }
        }
    }
}

void daFarBG_c::fn_801171F0(int a, int b) {
    float f1 = c_PIC_HEIGHT * mScale.y;
    float f2 = c_PIC_WIDTH * mScale.x;

    for (int i = 0; i < 2; i++) {
        int row_idx = i * 0x42;

        for (int j = 0; j < a; j++) {
            bgData_t * data = &mpBgData[row_idx + j];

            data->m_0.x = GetModelBasePosX(j, b, f2);
            float scroll = GetScrollBaseY();
            data->m_0.y = (256.0f - f1 * 0.5f) + (scroll - (float)i * f1);

            if (mIsBgB == 0) {
                data->m_0.z = -7000.0f;
            } else {
                data->m_0.z = -8000.0f;
            }

            data->m_0.z -= (i + j) * l_offsetZ;

            if (i == 0) {
                data->m_c = 0;
            } else {
                data->m_c = 1;
            }
        }
    }
}
void daFarBG_c::fn_801173A0(int a, int b) {
    float f1 = c_PIC_HEIGHT * mScale.y;
    float f2 = c_PIC_WIDTH * mScale.x;

    for (int i = 2; i >= 0; i--) {
        int row_idx = i * 0x42;

        int i6 = 1 - i;
        for (int j = 0; j < a; j++) {
            bgData_t * data = &mpBgData[row_idx + j];

            data->m_0.x = GetModelBasePosX(j, b, f2);
            float scroll = GetScrollBaseY();
            data->m_0.y = (f1 * 0.5f - 256.0f) + (float)(1 - i) * f1 + scroll;

            if (mIsBgB == 0) {
                data->m_0.z = -7000.0f;
            } else {
                data->m_0.z = -8000.0f;
            }

            data->m_0.z -= i6 * l_offsetZ;

            if (i == 1) {
                data->m_c = 2;
            } else {
                data->m_c = 1;
            }
        }
    }
}
void daFarBG_c::fn_80117550(int a, int b) {
    float f1 = c_PIC_HEIGHT * mScale.y;
    float f2 = c_PIC_WIDTH * mScale.x;
    int i7 = b - 1;

    for (int i = i7; i >= 0; i--) {
        int row_idx = i * 0x42;

        int i4 = i7 - i;
        for (int j = 0; j < a; j++) {
            bgData_t * data = &mpBgData[row_idx + j];

            data->m_0.x = GetModelBasePosX(j, b, f2);
            float scroll = GetScrollBaseY();
            data->m_0.y = (f1 * 0.5f - 256.0f) + f1 * (float)((-i - 2u) + b) + scroll;

            if (mIsBgB == 0) {
                data->m_0.z = -7000.0f;
            } else {
                data->m_0.z = -8000.0f;
            }

            data->m_0.z -= i4 * l_offsetZ;
            data->m_c = 1;
        }
    }
}
void daFarBG_c::fn_80117710(int a, int b) {
    float f1 = c_PIC_HEIGHT * mScale.y;
    float f2 = c_PIC_WIDTH * mScale.x;
    int i6 = b - 1;
    u32 u7 = 0;

    for (int i = i6; i >= 0; i--, u7++) {
        int row_idx = i * 0x42;

        int i8 = i6 - i;
        for (int j = 0; j < a; j++, i8++) {
            bgData_t * data = &mpBgData[row_idx + j];

            data->m_0.x = GetModelBasePosX(j, b, f2);
            float scroll = GetScrollBaseY();
            data->m_0.y = (f1 * 0.5f - 256.0f) + f1 * (float)((b - 2u) - i) + scroll;

            if (mIsBgB == 0) {
                data->m_0.z = -7000.0f;
            } else {
                data->m_0.z = -8000.0f;
            }

            data->m_0.z -= i8 * l_offsetZ;

            // if (u7 >> 31) = 1:
            //      ((u7 & 1) ^ -1) + 1 == 1
            //      ~(u7 & 1) == 0
            //      (u7 & 1) == 0
            // else:
            //      ((u7 & 1) ^ -0) + 0 == 1
            //      ((u7 & 1) ^ 0) == 1
            //      (u7 & 1) == 1
            if ((u7 & 1) == 1 ^ (u7 >> 31)) {
                data->m_c = 1;
            } else {
                data->m_c = 0;
            }
        }
    }
}
void daFarBG_c::fn_801178F0(int a, int b) {
    float f1 = c_PIC_HEIGHT * mScale.y;
    float f2 = c_PIC_WIDTH * mScale.x;

    float f8 = GetScrollBaseY();
    float step = c_PIC_HEIGHT;
    float zero = 0.0f;
    f8 = f1 * 0.5f + mOffset.y + f8;
    for (float f9 = f8; f9 < zero; f9 += step) {}

    int i5 = b - 1;
    f8 -= mOffset.y;
    int i6 = i5 / 2;

    for (; i5 >= 0; i5--) {
        int row_idx = i5 * 0x42;

        int i7 = 0;
        for (int j = 0; j < a; j++, i7++) {
            bgData_t * data = &mpBgData[row_idx + i7];

            if (i5 == i6) {
                data->m_c = 1;
                data->m_0.y = f8;
            } else if (i5 > i6) {
                data->m_c = 2;
                data->m_0.y = f8 - f1 * (float)(i5 - i6);
            } else {
                data->m_c = 0;
                data->m_0.y = f8 + f1 * (float)(i6 - i5);
            }

            data->m_0.x = GetModelBasePosX(j, b, f2);

            if (mIsBgB == 0) {
                data->m_0.z = -7000.0f;
            } else {
                data->m_0.z = -8000.0f;
            }

            if (i5 == i6) {
                data->m_0.z -= j * l_offsetZ;
            } else {
                data->m_0.z -= (j + 1) * l_offsetZ;
            }

        }
    }
}

bool daFarBG_c::GetModelNumXY(int &a, int &b) {
    float width = dBg_c::m_bg_p->getZoneWidth();
    float height = dBg_c::m_bg_p->getZoneHeight();

    int x = width / daFarBG_c::c_PIC_WIDTH;
    int y = height / daFarBG_c::c_PIC_HEIGHT;

    if (x == 0) {
        x = 1;
    }

    if (y == 0) {
        y = 1;
    }

    if (x % 2 == 0) {
        x++;
    }
    if (y % 2 == 0) {
        y++;
    }

    a = x + 2;
    b = y + 2;

    return true;
}


void daFarBG_c::InitBgData() {
    bgData_t * array = new bgData_t[34 * 22 * 3];
    mpBgData = array;

    for (u32 i = 0; i < 34; i++) {
        for (int j = 22; j != 0; j--) {
            for (int k = 0; k < 3; k++, array++) {
                if (mIsBgB == 0) {
                    array[0].m_0.set(0.0f, 0.0f, -7000.0f);
                } else if (mIsBgB == 1) {
                    array[0].m_0.set(0.0f, 0.0f, -8000.0f);
                }
                array[0].m_c = 9;
            }
        }
    }

    CalcModelBasePos();
}


void daFarBG_c::SetScrollFlag(int idx, nw4r::g3d::ResMdl mdl) {
    for (int i = 0; i < mdl.GetResNodeNumEntries(); i++) {
        const char * name = mdl.GetResNode(i).GetName();

        if (memcmp(name, "Scroll", 6)) {
            continue;
        }

        switch (name[6]) {
            case 'A':
                m_5e8[idx] |= 1;
                m_5eb |= 1;
                break;
            case 'B':
                m_5e8[idx] |= 2;
                m_5eb |= 2;
                break;
            case 'C':
                m_5e8[idx] |= 4;
                m_5eb |= 4;
                break;
            case 'D':
                m_5e8[idx] |= 8;
                m_5eb |= 8;
                break;
            case 'E':
                m_5e8[idx] |= 16;
                m_5eb |= 16;
                break;
            case 'F':
                m_5e8[idx] |= 32;
                m_5eb |= 32;
                break;
        }
    }
}

void daFarBG_c::CreateHeap() {
    InitBgData();
    m_5e4 = 0;

    for (int i = 0; i < (int)ARRAY_SIZE(mpBackgrounds); i++) {
        for (int j = 0; j < (int)ARRAY_SIZE(mpBackgrounds[0]); j++) {
            daFarBG_c::mdlData_t * mdl_data = new daFarBG_c::mdlData_t();
            mpBackgrounds[i][j] = mdl_data;
            mdl_data->mModel = nullptr;
            bool b3 = j == 0;
            mpBackgrounds[i][j]->mAnmChr = nullptr;
            mpBackgrounds[i][j]->mAnmClr = nullptr;
            mpBackgrounds[i][j]->mAnmSrt = nullptr;
            mpBackgrounds[i][j]->mIsTranslation = false;
            mpBackgrounds[i][j]->m_11 = false;
            mpBackgrounds[i][j]->m_12[0] = 0;
            mpBackgrounds[i][j]->m_12[1] = 0;
            mpBackgrounds[i][j]->mIsEnabled = false;

            switch (i) {
                case 0: {
                    daFarBG_c::sBgData * bgdata = GetMyBgDataP();
                    CreateModel(bgdata->mFile1, mpBackgrounds[i][j], b3);
                    break;
                }
                case 1: {
                    daFarBG_c::sBgData * bgdata = GetMyBgDataP();
                    CreateModel(bgdata->mFile2, mpBackgrounds[i][j], b3);
                    break;
                }
                case 2: {
                    daFarBG_c::sBgData * bgdata = GetMyBgDataP();
                    CreateModel(bgdata->mFile3, mpBackgrounds[i][j], b3);
                    break;
                }
            }
        }

        SetScrollFlag(i, mpBackgrounds[i][0]->mModel->getResMdl());
    }

    if (fn_80081BE0(1, 1, 1)) {
        mStaticBGIdx = 1;
    } else {
        mStaticBGIdx = 3;
    }

    if (mStaticBGIdx != 3) {
        daFarBG_c::mdlData_t * mdl_data = new daFarBG_c::mdlData_t();
        mpStaticBackground = mdl_data;
        mdl_data->mModel = nullptr;
        mpStaticBackground->mAnmChr = nullptr;
        mpStaticBackground->mAnmClr = nullptr;
        mpStaticBackground->mAnmSrt = nullptr;
        mpStaticBackground->mIsTranslation = false;
        mpStaticBackground->m_11 = false;
        mpStaticBackground->m_12[0] = 0;
        mpStaticBackground->m_12[1] = 0;
        mpStaticBackground->mIsEnabled = false;
        CreateModel(0x0104, mpStaticBackground, true);
    } else {
        mpStaticBackground = nullptr;
    }

    if (mpStaticBackground != nullptr) {
        SetScrollFlag(mStaticBGIdx, mpStaticBackground->mModel->getResMdl());
    }

    for (int i = 0; i < 6; i++) {
        if (m_5eb & (1 << i)) {
            m_5e4++;
        }
    }

    m_438[0].y = 0.0f;
    m_438[0].x = 0.0f;
    m_438[1].y = 0.0f;
    m_438[1].x = 0.0f;
    m_438[2].y = 0.0f;
    m_438[2].x = 0.0f;
    m_438[3].y = 0.0f;
    m_438[3].x = 0.0f;
    m_438[4].y = 0.0f;
    m_438[4].x = 0.0f;
    m_438[5].y = 0.0f;
    m_438[5].x = 0.0f;

    float f9 = getXScrollRate(0);
    float f10 = getYScrollRate(0);

    m_438[0].x = f9;
    m_438[0].y = f10;

    daFarBG_c::sBgData *other = GetOtherBgDataP();
    if (other == nullptr) {
        for (int i = 1; i < m_5e4; i++) {
            m_438[i].x = f9;
            m_438[i].y = f10;
        }
    } else {
        float f11 = std::fabs((f9 - getXScrollRate(1)) / (float)m_5e4);
        float f12 = std::fabs((f10 - getYScrollRate(1)) / (float)m_5e4);

        if ((m_438[0].x - f11 * (float)m_5e4 <= 0.0f) || (m_438[0].y - f12 * (float)(int)m_5e4 <= 0.0f)) {
            f11 = (m_438[0].x * 0.5f) / (float)m_5e4;
            f12 = (m_438[0].y * 0.5f) / (float)m_5e4;
        }

        int i7 = 0;
        u32 u5 = 0;
        for (int i6 = 3; i6 != 0; i6--) {
            for (int j = 0; j < 2; j++, u5++) {
                if (m_5eb & (1 << u5)) {
                    m_438[u5].x = f9 - f11 * (float)i7;
                    m_438[u5].y = f10 - f12 * (float)i7;
                    i7++;
                } else {
                    m_438[u5].y = 0.0f;
                    m_438[u5].x = 0.0f;
                }
            }
        }
    }
}


void daFarBG_c::CalcScale() {
    nw4r::math::MTX44 mtx;
    m3d::getCamera(0).GetProjectionMtx(&mtx);

    m_48c.x = 1.0f / mtx._00 * 0.0048935167f;
    m_48c.y = 1.0f / mtx._11 * 0.0089285718f;

    float zoom = GetZoomMagnif();

    mScale.x = m_48c.x - (m_48c.x - 1.0f) * zoom;
    mScale.y = m_48c.y - (m_48c.y - 1.0f) * zoom;
}

void daFarBG_c::CalcScrollBG() {
    dBgParameter_c * bg_param = dBgParameter_c::ms_Instance_p;
    dBg_c * bg_c = dBg_c::m_bg_p;
    mVec2_c screen_center = GetScreenCenterPos();
    mVec3_c zero = mVec3_c::Zero;

    switch (dScStage_c::m_loopType) {
        case 2: {
            float f1 = bg_c->mLoopOffset;
            float f2 = bg_param->mPos.x - bg_c->mPrevScreenLeft;

            if (f2 < -f1 * 0.9f) {
                f2 += f1;
            } else if (f2 > f1 * 0.9f) {
                f2 -= f1;
            }

            if (!dGameCom::isGameStop(dGameCom::GAME_STOP_ANY) && !(dActor_c::mExecStop & 0xF)) {
                mBgScroll += f2;
            }

            zero.x = mBgScroll;
            break;
        }
        case 0:
        case 1: {

            if (isZero(mCenterXPos)) {
                zero.x = screen_center.x - mCenterXPos;
            } else {
                zero.x = screen_center.x - bg_c->mZoneLeft;
            }
            break;
        }
    }

    switch (GetScrollBaseType()) {
        case SCROLL_0:
            zero.y = (screen_center.y - bg_param->mSize.y * 0.5f) - GetScrollBaseY();
            break;
        case SCROLL_1:
            zero.y = (screen_center.y + bg_param->mSize.y * 0.5f) - GetScrollBaseY();
            break;
        case SCROLL_2:
            zero.y = screen_center.y - GetScrollBaseY();
            break;
    }

    for (int i = 3; i != 0; i--) {

        m_498[i].x *= zero.x * mScale.x;
        m_498[i].y *= zero.y * mScale.y;
        m_498[i].z *= zero.z * mScale.z;

        switch (dScStage_c::m_loopType) {
            case 2: {
                while (c_PIC_WIDTH + 100.0f < std::fabs(m_498[i].x)) {
                    if (m_498[i].x > 0.0f) {
                        m_498[i].x -= c_PIC_WIDTH;
                    } else {
                        m_498[i].x += c_PIC_WIDTH;
                    }
                }
                break;
            }
        }

        m_498[i + 1].x *= zero.x * mScale.x;
        m_498[i + 1].y *= zero.y * mScale.y;
        m_498[i + 1].z *= zero.z * mScale.z;

        switch (dScStage_c::m_loopType) {
            case 2: {
                while (c_PIC_WIDTH + 100.0f < std::fabs(m_498[i + 1].x)) {
                    if (m_498[i + 1].x > 0.0f) {
                        m_498[i + 1].x -= c_PIC_WIDTH;
                    } else {
                        m_498[i + 1].x += c_PIC_WIDTH;
                    }
                }
                break;
            }
        }

    }
}

void daFarBG_c::InitFrustum() {
    mMtx_c mtx;

    float f1 = 0.5f * dBgParameter_c::ms_Instance_p->mSize.y;
    float neg_f1 = -f1;
    float f2 = 0.5f * -dBgParameter_c::ms_Instance_p->mSize.x;
    float neg_f2 = -f2;
    float f3 = 1.0f;
    float f4 = 20000.0f;

    m3d::getCamera(0).GetCameraMtx(&mtx);

    mFrustum.set(f1, neg_f1, f2, neg_f2, f3, f4, mtx, true);
}

void daFarBG_c::SetCullingInfo(m3d::smdl_c & model) {
    nw4r::g3d::ResMdl resMdl = model.getResMdl();
    mAABB_c local_50;
    mAABB_c local_38;

    for (unsigned long i = 0; i < resMdl.GetResNodeNumEntries(); i++) {
        nw4r::g3d::ResNode resNode = resMdl.GetResNode(i);

        local_50.min = resNode.GetVolumeMin();
        local_50.max = resNode.GetVolumeMax();
        bool b1 = false;

        if (local_50.min == local_50.max) {
            b1 = true;
        }

        if (!b1) {
            // TODO: Figure this stuff out...
            nw4r::g3d::ScnMdl * mdl = nw4r::g3d::ScnMdl::DynamicCast<nw4r::g3d::ScnMdl>(model.getScn());

            local_38.Set(&local_50, mdl->GetMtxPtr(mdl->MTX_LOCAL));

            if (mFrustum.intersectAABB(&local_38)) {
                d3d::setNodeVisibility(&model, i, 1);
            } else {
                d3d::setNodeVisibility(&model, i, 0);
            }
        }
    }
}

mVec3_c daFarBG_c::GetScrollDiff(int idx) {
    return m_498[0] - m_498[idx];
}

void daFarBG_c::nodeCallback_c::timingB(unsigned long nodeIdx, nw4r::g3d::WorldMtxManip* b, nw4r::g3d::ResMdl resMdl) {
    nw4r::g3d::ResNode resNode = resMdl.GetResNode(nodeIdx);
    nw4r::g3d::ResName resName = resNode.GetResName();
    mMtx_c mtx;
    mMtx_c mtx2;

    if (!memcmp(resName.GetName(), "Rotate", 6)) {
        b->GetMatrix(&mtx);

        float f2 = dBgParameter_c::ms_Instance_p->mSize.x * 0.5f;
        float f3 = dBgParameter_c::ms_Instance_p->mPos.x + f2;
        float f5 = 5000.0f;
        float f4 = 0.0f;

        if (nodeIdx < 100) {
            if (f3 <= mtx._03) {
                f4 = (mtx._03 - f3) / f2;
            } else {
                f4 = -((f3 - mtx._03) / f2);
            }
        }

        switch (resName.GetName()[6]) {
            case '0':
                f5 = 2000.0f;
                break;
            case '1':
                f5 = 3000.0f;
                break;
            case '2':
                f5 = 4000.0f;
                break;
            case '3':
                // [Why is this case empty? Maybe they accidentally commented out the assignment?]
                // f5 = 5000.0f;
                break;
            case '4':
                f5 = 6000.0f;
                break;
            case '5':
                f5 = 7000.0f;
                break;
            case '6':
                f5 = 8000.0f;
                break;
            case '7':
                f5 = 9000.0f;
                break;
            case '8':
                f5 = 10000.0f;
                break;
            case '9':
                f5 = 11000.0f;
                break;
        }

        mtx.YrotM(f4 * f5);
        b->SetMatrix(mtx);
    } else if (!memcmp(resName.GetName(), "Scroll", 6)) {
        b->GetMatrix(&mtx);
        mVec3_c scroll(0.0f, 0.0f, 0.0f);

        switch (resName.GetName()[6]) {
            case 'A':
                scroll = mpParent->GetScrollDiff(0);
                break;
            case 'B':
                scroll = mpParent->GetScrollDiff(1);
                break;
            case 'C':
                scroll = mpParent->GetScrollDiff(2);
                break;
            case 'D':
                scroll = mpParent->GetScrollDiff(3);
                break;
            case 'E':
                scroll = mpParent->GetScrollDiff(4);
                break;
            case 'F':
                scroll = mpParent->GetScrollDiff(5);
                break;
        }

        mtx.trans(scroll);

        b->GetMatrix(&mtx2);
        mtx.concat(mtx2);
        b->SetMatrix(mtx);
    }
}


daFarBG_c::sBgData * daFarBG_c::GetMyBgDataP() {
    return mpBgEntries[mIsBgB];
}

daFarBG_c::sBgData * daFarBG_c::GetOtherBgDataP() {
    if (mIsBgB == 0) {
        return mpBgEntries[1];
    } else {
        return mpBgEntries[0];
    }
}

void daFarBG_c::UpdateAnim() {
    if (!dGameCom::isGameStop(dGameCom::GAME_STOP_ANY) && !(dActor_c::mExecStop & 0xF)) {
        u32 r30 = 0;
        u32 r31 = 0;

        if (dActorMng_c::m_instance->mStartVolcanoEruptions != 0) {
            r31 = 1;
        }

        if (m_62d != 0) {
            m_62d = 0;
            r30 = 1;
        }

        if (m_HIO[mIsBgB].m_20 == 0) {
            for (int i = 0; i < (int)ARRAY_SIZE(mpBackgrounds); i++) {
                for (int j = 0; j < (int)ARRAY_SIZE(mpBackgrounds[0]); j++) {
                    if (mpBackgrounds[i][j]->mModel != nullptr) {
                        if (r31 && (mpBackgrounds[i][j]->mAnmChr != nullptr)) {
                            mpBackgrounds[i][j]->mAnmChr->setFrame(0.0f);
                            mpBackgrounds[i][j]->mAnmChr->setRate(1.0f);
                        }

                        if ((m_62c != 0) && (mpBackgrounds[i][j]->mAnmSrt != nullptr)) {
                            if (r30) {
                                m_638 = m_630;
                            }

                            if (m_634 < 0.0f) {
                                float f13 = m_638 - m_634;

                                if (f13 >= 0.0f) {
                                    m_638 = f13;
                                } else {
                                    m_638 = 0.0f;
                                }
                            } else {
                                float f13 = m_638 + m_634;

                                if ((f13 >= mpBackgrounds[i][j]->mAnmSrt->getFrameMax(0))) {
                                    m_638 = f13;
                                } else {
                                    m_638 = mpBackgrounds[i][j]->mAnmSrt->getFrameMax(0);
                                }
                            }

                            mpBackgrounds[i][j]->mAnmSrt->setFrame(m_638, 0);
                        }

                        mpBackgrounds[i][j]->mModel->play();
                    }

                    if ((m_62c == 0) && (mpBackgrounds[i][j]->mAnmSrt != nullptr)) {
                        mpBackgrounds[i][j]->mAnmSrt->play();
                    }

                    if (mpBackgrounds[i][j]->mAnmClr != nullptr) {
                        mpBackgrounds[i][j]->mAnmClr->play();
                    }
                }
            }
        }

        if (mpStaticBackground != nullptr) {
            if (mpStaticBackground->mModel) {
                mpStaticBackground->mModel->play();
            }

            if (mpStaticBackground->mAnmSrt) {
                mpStaticBackground->mAnmSrt->play();
            }

            if (mpStaticBackground->mAnmClr) {
                mpStaticBackground->mAnmClr->play();
            }
        }
    }
}

bool daFarBG_c::checkResource() {
    char local_58[64];
    char local_98[64];
    char local_d8[64];
    char local_118[64];
    char local_158[64];
    nw4r::g3d::ResFile pnVar3;
    nw4r::g3d::ResFile pnVar2;

    daFarBG_c::sBgData *bg_data = GetMyBgDataP();
    bool ret = true;

    nw4r::g3d::ResFile pnVar4;

    pnVar2 = GetRes(bg_data->mFile1, local_d8, local_118, local_158);
    pnVar3 = GetRes(bg_data->mFile2, local_98, local_118, local_158);
    pnVar4 = GetRes(bg_data->mFile3, local_58, local_118, local_158);

    if ((bg_data->mFile1 != 0) && (!pnVar2.IsValid())) {
        ret = false;
    }
    if ((bg_data->mFile2 != 0) && (!pnVar3.IsValid())) {
        ret = false;
    }
    if ((bg_data->mFile3 != 0) && (!pnVar4.IsValid())) {
        ret = false;
    }

    return ret;
}

mVec2_c daFarBG_c::GetScreenCenterPos() {
    dBgParameter_c * bg_param = dBgParameter_c::ms_Instance_p;
    mVec2_c ret;
    ret.x = bg_param->mPos.x + (bg_param->mSize.x * 0.5f);
    ret.y = bg_param->mPos.y - (bg_param->mSize.y * 0.5f);
    return ret;
}

bool daFarBG_c::mdlData_t::createKazanEffect(mVec3_c param_2, int i) {
    m_12[i] = mFollowEffects[i].createEffect("Wm_bg_volcano", 0, &param_2, nullptr, nullptr);
    return m_12[i];
}

bool daFarBG_c::mdlData_t::updateKazanEffect(mVec3_c param_2, int i) {
    m_12[i] = mFollowEffects[i].follow(&param_2, nullptr, nullptr);
    return m_12[i];
}

bool daFarBG_c::fn_80119370(m3d::bmdl_c *mdl, ulong idx, nw4r::math::VEC3 &vec) {
    return mdl->getNodeWorldMtxMultVecZero(idx, vec);
}

void daFarBG_c::effectExecute() {
    if (dGameCom::isGameStop(dGameCom::GAME_STOP_ANY) || (dActor_c::mExecStop & 0xF)) {
        return;
    }

    bool r26 = false;
    if (mIsBgB == 1) {
        return;
    }

    if (dActorMng_c::m_instance->mStartVolcanoEruptions != 0) {
        dActorMng_c::m_instance->mStartVolcanoEruptions = 0;
        r26 = true;
    }

    mVec2_c local_90 = GetScreenCenterPos();
    mVec3_c local_70;
    mVec3_c local_64(local_90, 0.0f);
    mVec3_c local_58;
    float f1 = 1e6f;

    for (int i = 0; i < (int)ARRAY_SIZE(mpBackgrounds); i++) {
        for (int j = 0; j < (int)ARRAY_SIZE(mpBackgrounds[0]); j++) {
            mdlData_t * bg = mpBackgrounds[i][j];
            if ((bg->mModel != nullptr) && bg->m_11 && bg->mIsEnabled) {
                for (int k = 0; k < (int)ARRAY_SIZE(bg->m_12); k++) {
                    if (r26) {
                        fn_80119370(bg->mModel, m_5f4[k], local_58);
                        bg->createKazanEffect(local_58, k);

                        local_70.x = local_58.x - local_90.x;
                        local_70.y = local_58.y - local_90.y;
                        local_70.z = 0.0f;

                        if (local_70.LenSq() < f1) {
                            f1 = local_70.LenSq();
                            local_64 = local_58;
                        }
                    } else if (bg->m_12[k] != 0) {
                        fn_80119370(bg->mModel, m_5f4[k], local_58);
                        mpBackgrounds[i][j]->updateKazanEffect(local_58, k);
                    }
                }
            }
        }
    }

    if (r26) {
        dAudio::SndObjctCmnMap_c * tmp = dAudio::g_pSndObjMap;
        tmp->startSound(SE_OBJ_KAZAN_ERUPTION, dAudio::cvtSndObjctPos(local_64), 0);
    }
}

void daFarBG_c::CalcBasePosAndMtx() {
    dBgParameter_c * bg_param = dBgParameter_c::ms_Instance_p;
    daFarBG_c::bgData_t * cur;
    u16 u6 = 0;

    for (int i7 = ARRAY_SIZE(mpBgData), i = 0; i7 != 0; i7--, i++) {
        cur = &mpBgData[i];
        if ((cur->m_c != 9) && (m_5f8 == u6 % 66)) {
            m_5fa = u6 / 66;
            break;
        }
    }

    float f1 = cur->m_0.x + mOffset.x;
    float f2 = cur->m_0.y + mOffset.y;
    float f9 = bg_param->mSize.y;
    mVec3_c local_534 = GetCameraPos();

    if (isZero(GetZoomMagnif()) != false) {
        m_5fc.trans(mVec3_c(local_534));
        m_5fc.concat(mMtx_c::createTrans(-m_498[0].x, -m_498[0].y, 0.0f));
        m_5fc.concat(mMtx_c::createScale(mScale.x, mScale.y, 1.0f));
        m_5fc.concat(mMtx_c::createTrans(0.0f, -GetScrollBaseY(), 0.0f));

        if (GetScrollBaseType() == SCROLL_1) {
            m_5fc.concat(mMtx_c::createTrans(0.0f, f9, 0.0f));
        }

        m_5fc.concat(mMtx_c::createTrans(mVec3_c(-local_534)));
        m_5fc.concat(mMtx_c::createTrans(0.0f, local_534.y, local_534.z));
        m_5fc.concat(mMtx_c::createTrans(f1, f2, 0.0f));
    } else {
        local_534.x -= mCenterXPos;
        local_534.y -= 0.5f * bg_param->mSize.y;
        local_534.z = 0.0f;

        if (mCenterMode != 0) {
            m_5fc.trans(mVec3_c(local_534));
            m_5fc.concat(mMtx_c::createTrans(0.0f, -GetScrollBaseY(), 0.0f));

            if (GetScrollBaseType() == SCROLL_1) {
                m_5fc.concat(mMtx_c::createTrans(0.0f, f9, 0.0f));
            }

            m_5fc.concat(mMtx_c::createTrans(-m_498[0].x, -m_498[0].y, 0.0f));

            m_5fc.concat(mMtx_c::createTrans(0.0f, GetScrollBaseY(), 0.0f));
            m_5fc.concat(mMtx_c::createScale(mScale.x, mScale.y, 1.0f));
            m_5fc.concat(mMtx_c::createTrans(0.0f, -GetScrollBaseY(), 0.0f));

            m_5fc.concat(mMtx_c::createTrans(mVec3_c(-local_534)));
            m_5fc.concat(mMtx_c::createTrans(mVec3_c(local_534)));
            m_5fc.concat(mMtx_c::createTrans(f1, f2, 0.0f));
        } else {
            m_5fc.trans(mVec3_c(local_534));
            m_5fc.concat(mMtx_c::createTrans(0.0f, -GetScrollBaseY(), 0.0f));

            if (GetScrollBaseType() == SCROLL_1) {
                m_5fc.concat(mMtx_c::createTrans(0.0f, f9, 0.0f));
            }

            m_5fc.concat(mMtx_c::createTrans(-m_498[0].x, -m_498[0].y, 0.0f));
            m_5fc.concat(mMtx_c::createTrans(f1, 0.0f, 0.0f));

            m_5fc.concat(mMtx_c::createTrans(0.0f, GetScrollBaseY(), 0.0f));
            m_5fc.concat(mMtx_c::createScale(mScale.x, mScale.y, 1.0f));
            m_5fc.concat(mMtx_c::createTrans(0.0f, -GetScrollBaseY(), 0.0f));

            m_5fc.concat(mMtx_c::createTrans(mVec3_c(-local_534)));
            m_5fc.concat(mMtx_c::createTrans(mVec3_c(local_534)));
            m_5fc.concat(mMtx_c::createTrans(0.0f, f2, 0.0f));
        }
    }
}

void daFarBG_c::ReserveModel() {
    for (int i = 0; i < (int)ARRAY_SIZE(mpBackgrounds); i++) {
        for (int j = 0; j < (int)ARRAY_SIZE(mpBackgrounds[0]); j++) {
            mpBackgrounds[i][j]->mIsEnabled = false;
        }
    }

    if (mpStaticBackground != nullptr) {
        mpStaticBackground->mIsEnabled = false;
    }

    mVec3_c local_100;
    mVec3_c local_10c;
    int local_130[3] = { 0 };

    local_100.set(
        dBgParameter_c::ms_Instance_p->mPos.x,
        dBgParameter_c::ms_Instance_p->mPos.y - dBgParameter_c::ms_Instance_p->mSize.y,
        0.0f
    );
    local_10c.set(
        local_100.x + dBgParameter_c::ms_Instance_p->mSize.x,
        dBgParameter_c::ms_Instance_p->mPos.y,
        0.0f
    );

    for (int u9 = 0; u9 < 0x22; u9++) {
        for (int uVar8 = 0; uVar8 < 0x42; uVar8++) {
            daFarBG_c::bgData_t * bg = &mpBgData[u9 * 0x42 + uVar8];

            int u7 = bg->m_c;
            if (u7 == 9) {
                continue;
            }

            bool b3 = false;
            daFarBG_c::mdlData_t *model = mpBackgrounds[bg->m_c][0];
            if ((u7 == mStaticBGIdx) && ((uVar8 & 0xFFFF) == 1)) {
                model = mpStaticBackground;
            }

            mMtx_c local_c0 = m_5fc;
            local_c0._03 += mScale.x * c_PIC_WIDTH * (float)((uVar8 & 0xFFFF) - m_5f8);
            local_c0._13 += mScale.y * c_PIC_HEIGHT * (float)((u9 & 0xFFFF) - m_5fa);

            for (int u6 = 0; u6 < 6; u6++) {
                if ((m_5e8[u7] & (1 << u6)) != 0) {
                    float f2 = local_c0._03 + GetScrollDiff(u6).x;
                    float f3 = local_c0._13 + GetScrollDiff(u6).y;
                    float f1 = mScale.x;
                    mVec3_c local_118;
                    local_118.x = f2 - f1 * c_PIC_WIDTH_HALF;
                    local_118.y = f3 - f1 * c_PIC_HEIGHT_HALF;
                    local_118.z = 0.0f;
                    mVec3_c local_124;
                    local_124.x = f2 + f1 * c_PIC_WIDTH_HALF;
                    local_124.y = f3 + f1 * c_PIC_HEIGHT_HALF;
                    local_124.z = 0.0f;

                    if (model->mAnmChr != nullptr) {
                        local_118.x -= f1 * c_PIC_WIDTH;
                    }
                    if (model->mIsTranslation) {
                        // why not use c_PIC_WIDTH_HALF?
                        local_118.x -= mScale.x * c_PIC_WIDTH * 0.5f;
                    }

                    if (dGameCom::checkRectangleOverlap(&local_118, &local_124, &local_100, &local_10c, 8.0f)) {
                        b3 = true;
                        break;
                    }
                }
            }

            if (b3) {
                if (model == mpStaticBackground) {
                    mpStaticBackground->mIsEnabled = true;
                } else {
                    int i4 = local_130[u7];
                    if (i4 < 9) {
                        local_130[u7] = i4 + 1;
                        mpBackgrounds[u7][i4]->mIsEnabled = true;
                        model = mpBackgrounds[u7][i4];
                    } else {
                        b3 = false;
                    }
                }
            }

            if (b3) {
                local_c0._23 = bg->m_0.z;
                local_c0.concat(mMtx_c::createScale(1.0000012f, 1.0000012f, 1.0f));
                model->mModel->setLocalMtx((nw4r::math::MTX34 *)&local_c0);
                model->mModel->setScale(1.0f, 1.0f, 1.0f);
                model->mModel->calc(false);
            }
        }
    }
}


mVec3_c daFarBG_c::GetCameraPos() {
    dBgParameter_c *bg_param = dBgParameter_c::ms_Instance_p;
    mVec3_c ret;

    if (isZero(GetZoomMagnif())) {
        /// [actually unreachable - GetZoomMagnif() never returns a value < 0.25f]
        ret.x = bg_param->mPos.x;
        float size = bg_param->mSize.y;
        ret.y = bg_param->mPos.y - size;
        ret.z = 0.0f;
    } else {
        ret = dScStage_c::getCamera(0)->mCenterPos;
    }
    return ret;
}

float daFarBG_c::GetZoomMagnif() {
    static const float l_zoom_magnif_table[] = {
        1.0f, 0.75f, 0.5f, 0.25f
    };

    u16 idx = GetMyBgDataP()->mScale;
    if (idx >= ARRAY_SIZE(l_zoom_magnif_table)) {
        idx = 0;
    }

    return l_zoom_magnif_table[idx];
}

void daFarBG_c::CalcModelBasePos() {
    int num_x, num_y;
    GetModelNumXY(num_x, num_y);

    switch (mRepeatType) {
        case REPEAT_1:
            fn_80116E60(num_x, num_y);
            break;
        case REPEAT_2:
            fn_80117030(num_x, num_y);
            break;
        case REPEAT_3:
            fn_801171F0(num_x, num_y);
            break;
        case REPEAT_4:
            fn_801173A0(num_x, num_y);
            break;
        case REPEAT_5:
            fn_80117550(num_x, num_y);
            break;
        case REPEAT_6:
            fn_80117710(num_x, num_y);
            break;
        case REPEAT_7:
            fn_801178F0(num_x, num_y);
            break;
    }
}
