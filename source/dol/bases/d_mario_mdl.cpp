#include <game/bases/d_mario_mdl.hpp>
#include <game/bases/d_a_player_manager.hpp>

const dPlayerMdl_c::ModelInfo_s scArcDt[2] = {
    {
        "Mario", "L_rcha",
        "MB_model", "SMB_model", "PLMB_model", "PMB_model",
        "MH_model", "SMH_model", "PLMH_model", "PMH_model",
        13.0f, 10.0f, 13.0f, 13.0f
    },
    {
        "Luigi", "L_rcha",
        "LB_model", "SLB_model", "PLLB_model", "PLB_model",
        "LH_model", "SLH_model", "PLLH_model", "PLH_model",
        14.0f, 11.0f, 14.0f, 14.0f
    }
};

 const char * const scArcCapDt[2][2] = {
    {"MC_model", "SMC_model"},
    {"LC_model", "SLC_model"}
};

dMarioMdl_c::dMarioMdl_c(u8 modelType) : dPlayerMdl_c(modelType),
    m_824(0), m_870(0), m_86c(0)
{
    mpArcNames = &scArcDt[m_151];
    m_77c = 0x10;
}

dMarioMdl_c::~dMarioMdl_c() {}

void dMarioMdl_c::initialize() {
    dPlayerMdl_c::initialize();
    fn_800cab00(0);
}

void dMarioMdl_c::createPlayerModel() {
    dPlayerMdl_c::createPlayerModel();

    nw4r::g3d::ResAnmTexPat wait_tex_pat = m_20c.GetResAnmTexPat("PH_wait");
    nw4r::g3d::ResMdl res_mdl_head = m_20c.GetResMdl(mpArcNames->mModelNameHead[MODEL_NAME_NORMAL]);
    mTexAnm2.create(res_mdl_head, wait_tex_pat, &mAllocator, nullptr, 2);

    nw4r::g3d::ResMdl res_mdl_peng = m_20c.GetResMdl(mpArcNames->mModelNameHead[MODEL_NAME_PENGUIN]);
    mTexAnmPenguin.create(res_mdl_peng, wait_tex_pat, &mAllocator, nullptr, 2);

    nw4r::g3d::ResMdl res_mdl_prop = m_20c.GetResMdl(mpArcNames->mModelNameHead[MODEL_NAME_PROPELLER]);
    mTexAnmPropeller.create(res_mdl_prop, wait_tex_pat, &mAllocator, nullptr, 2);

    nw4r::g3d::ResMdl res_mdl_body = m_20c.GetResMdl(mpArcNames->mModelNameBody[MODEL_NAME_NORMAL]);
    nw4r::g3d::ResAnmTexPat switch_tex_pat = m_20c.GetResAnmTexPat("PB_switch");
    mTexAnm1.create(res_mdl_body, switch_tex_pat, &mAllocator);

    for (int i = 0; i < (int) ARRAY_SIZE(mCapModels); i++) {
        nw4r::g3d::ResMdl res_cap_mdl = m_20c.GetResMdl(scArcCapDt[m_151][i]);
        mCapModels[i].create(res_cap_mdl, &mAllocator, 0x863);
        setSoftLight(mCapModels[i]);
    }

    int idx = 0;
    if (mPyPlayerMode == 1) {
        idx = 1;
    }

    nw4r::g3d::ResMdl res_cap_mdl_2 = m_20c.GetResMdl(scArcCapDt[m_151][idx]);
    nw4r::g3d::ResAnmTexPat cap_switch_tex_pat = m_20c.GetResAnmTexPat("PC_switch");
    mCapPatAnm.create(res_cap_mdl_2, cap_switch_tex_pat, &mAllocator);
}

void dMarioMdl_c::_calc() {
    dPlayerMdl_c::_calc();

    // This feels very fake, but I couldn't get it to match another way...
    if (!((mPyPlayerMode != PLAYER_MODE_NORMAL) && (mPyPlayerMode != PLAYER_MODE_RIDE))) {
        if (m_86c == 0) {
            d3d::setMatCullMode(&mInfo[mPyPlayerMode].mMdl2, 2, GX_CULL_ALL);
            d3d::setMatCullMode(&mInfo[mPyPlayerMode].mMdl2, 3, GX_CULL_BACK);
        } else {
            d3d::setMatCullMode(&mInfo[mPyPlayerMode].mMdl2, 2, GX_CULL_BACK);
            d3d::setMatCullMode(&mInfo[mPyPlayerMode].mMdl2, 3, GX_CULL_ALL);
        }
    }

    mMtx_c mtx;
    getJointMtx(&mtx, 15);
    mCapModels[m_870].setLocalMtx(&mtx);
    mCapModels[m_870].calc(false);
}

void dMarioMdl_c::play() {
    dPlayerMdl_c::play();
    mCapModels[m_870].play();
}

void dMarioMdl_c::draw() {
    dPlayerMdl_c::draw();

    if (m_17c & 1) {
        mCapModels[m_870].entry();
        setSoftLight(mCapModels[m_870]);
    }
}

void dMarioMdl_c::setColorType(u8 type) {
    if (mPowerupTex == type) return;

    if (((mPyPlayerMode != PLAYER_MODE_NORMAL) && (mPyPlayerMode != PLAYER_MODE_RIDE)))
        return;

    mPowerupTex = type;

    float f1 = 0.0f;
    if (type == 1) {
        f1 = 1.0f;
    } else if (type == 2) {
        f1 = 2.0f;
    }

    nw4r::g3d::ResAnmTexPat ph_switch_anm = m_20c.GetResAnmTexPat("PH_switch");
    mTexAnm2.setAnm(mInfo[0].mMdl2, ph_switch_anm, 0, m3d::PLAYMODE_INHERIT);
    mTexAnm2.setRate(0.0f, 0);
    mTexAnm2.setFrame(f1, 0);
    mInfo[0].mMdl2.setAnm(mTexAnm2);

    nw4r::g3d::ResAnmTexPat pb_switch_anm = m_20c.GetResAnmTexPat("PB_switch");
    mTexAnm1.setAnm(*getBodyMdl(), pb_switch_anm, 0, m3d::PLAYMODE_INHERIT);
    mTexAnm1.setRate(0.0f, 0);
    mTexAnm1.setFrame(f1, 0);
    getBodyMdl()->setAnm(mTexAnm1);

    nw4r::g3d::ResAnmTexPat pc_switch_anm = m_20c.GetResAnmTexPat("PC_switch");
    mCapPatAnm.setAnm(mCapModels[0], pc_switch_anm, 0, m3d::PLAYMODE_INHERIT);
    mCapPatAnm.setRate(0.0f, 0);
    mCapPatAnm.setFrame(f1, 0);
    mCapModels[0].setAnm(mCapPatAnm);
}


void dMarioMdl_c::setTexAnmType(dPyMdlBase_c::TexAnmType_e type) {
    dPlayerMdl_c::setTexAnmType(type);
}

bool dMarioMdl_c::setPersonalAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr, int c) {
    if (dPlayerMdl_c::setPersonalAnm(anmID, outAnmChr, c)) {
        return true;
    }

    char anm_name[32];
    bool ret = false;
    if (m_151 == 1) {
        switch (anmID) {
            case 0x00:
                strncpy(anm_name, "LB_wait", ARRAY_SIZE(anm_name));
                ret = true;
                break;
            case 0x01:
                strncpy(anm_name, "LB_walk", ARRAY_SIZE(anm_name));
                ret = true;
                break;
            case 0x02:
                strncpy(anm_name, "LB_run", ARRAY_SIZE(anm_name));
                ret = true;
                break;
            case 0x87:
            case 0x91:
                strncpy(anm_name, "LB_waitL", ARRAY_SIZE(anm_name));
                ret = true;
                break;
            case 0x88:
            case 0x90:
                strncpy(anm_name, "LB_waitR", ARRAY_SIZE(anm_name));
                ret = true;
                break;
            case 0x92:
                strncpy(anm_name, "LB_waitR3", ARRAY_SIZE(anm_name));
                ret = true;
                break;
            case 0x93:
                strncpy(anm_name, "LB_waitL3", ARRAY_SIZE(anm_name));
                ret = true;
                break;
            case 0x98:
                strncpy(anm_name, "LB_slope_waitL2", ARRAY_SIZE(anm_name));
                ret = true;
                break;
            case 0x99:
                strncpy(anm_name, "LB_slope_waitR2", ARRAY_SIZE(anm_name));
                ret = true;
                break;
            case 0xAF:
                strncpy(anm_name, "LB_ending_wait", ARRAY_SIZE(anm_name));
                ret = true;
                break;
        }

        if (ret) {
            *outAnmChr = m_214.GetResAnmChr(anm_name);
            if (c == 0) {
                mFlags |= 0x200000;
            }
            m_164 |= 0x200000;
            return true;
        }
    }

    return false;
}

void dMarioMdl_c::FUN_800d4750(int mode) {
    dPlayerMdl_c::FUN_800d4750(mode);

    if (mPyPlayerMode == 1) {
        m_870 = 1;
    } else {
        m_870 = 0;
    }
}

void dMarioMdl_c::fn_800cab00(int isHatLess) {
    if (daPyMng_c::mBonusNoCap && !m_151) {
        m_86c = 1;
        m_17c &= ~1;
    } else {
        m_86c = isHatLess;
        if (isHatLess == 1) {
            m_17c |= 1;
        } else {
            m_17c &= ~1;
        }
    }
}

float dMarioMdl_c::getAimMotionShareScale() {
    static const float scMarioHipScale[4] = { 1.0f, 0.546f, 1.0f, 0.72f };
    static const float scMarioHangCliffScale[4] = { 0.0f, 0.0f, 0.0f, 0.55f };
    static const float scLuigiHipScale[4] = { 1.15f, 0.614f, 1.15f, 0.896f };
    static const float scLuigiOrgHipScale[4] = { 1.0f, 0.534f, 1.0f, 0.779f };
    static const float scLuigiHangCliffScale[4] = { -0.25f, -0.05f, -0.25f, 0.4f };

    dPyMdlBase_c::PlayerMode_e mode = mPyPlayerMode;

    if ((mFlags & 0x800000) || (mFlags & 0x1000000)) {
        mode = PLAYER_MODE_NORMAL;
    }

    if (m_151 == 0) {
        float f1 = scMarioHipScale[mode];

        if (!(mFlags & 8)) {
            return f1;
        }

        return f1 + scMarioHangCliffScale[mode];
    } else {
        float f1;

        if (mFlags & 0x200000) {
            f1 = scLuigiOrgHipScale[mode];
        } else {
            f1 = scLuigiHipScale[mode];
        }

        if (!(mFlags & 8)) {
            return f1;
        }

        return f1 + scLuigiHangCliffScale[mode];
    }
}

void dMarioMdl_c::updateBonusCap() {
    fn_800cab00(false);
}
