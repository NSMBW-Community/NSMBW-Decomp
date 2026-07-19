#include <game/bases/d_mario_mdl.hpp>
#include <game/bases/d_a_player_manager.hpp>

const dPlayerMdl_c::ModelInfo_s scArcDt[2] = {
    { "Mario", "L_rcha", "Mb_model", "Smb_model", "Plmb_model", "Pmb_model", "Mh_model", "Smh_model", "Plmh_model", "Pmh_model", 13.0f, 10.0f, 13.0f, 13.0f },
    { "Luigi", "L_rcha", "Lb_model", "Slb_model", "Pllb_model", "Plb_model", "Lh_model", "Slh_model", "Pllh_model", "Plh_model", 14.0f, 11.0f, 14.0f, 14.0f },
};

char * scArcCapDt[2][2] = {
    {"a", "b"},
    {"c", "d"}
};

dMarioMdl_c::dMarioMdl_c(u8 modelType) : dPlayerMdl_c(modelType), m_820(0), m_868(0), m_86c(0) {
    mpArcNames = &scArcDt[modelType];
    m_77c = 0x10;
}

void dMarioMdl_c::initialize() {
    dPlayerMdl_c::initialize();
    fn_800cab00(0);
}

void dMarioMdl_c::createPlayerModel() {
    dPlayerMdl_c::createPlayerModel();

    nw4r::g3d::ResAnmTexPat wait_tex_pat = m_20c.GetResAnmTexPat("Ph_wait");
    nw4r::g3d::ResMdl res_mdl_head = m_20c.GetResMdl(mpArcNames->mModelNameHead[MODEL_NAME_NORMAL]);
    mTexAnm2.create(res_mdl_head, wait_tex_pat, &mAllocator, 2);

    nw4r::g3d::ResMdl res_mdl_peng = m_20c.GetResMdl(mpArcNames->mModelNameHead[MODEL_NAME_PENGUIN]);
    mTexAnmPenguin.create(res_mdl_peng, wait_tex_pat, &mAllocator, 2);

    nw4r::g3d::ResMdl res_mdl_prop = m_20c.GetResMdl(mpArcNames->mModelNameHead[MODEL_NAME_PROPELLER]);
    mTexAnmPropeller.create(res_mdl_prop, wait_tex_pat, &mAllocator, 2);

    nw4r::g3d::ResMdl res_mdl_body = m_20c.GetResMdl(mpArcNames->mModelNameBody[MODEL_NAME_NORMAL]);
    nw4r::g3d::ResAnmTexPat switch_tex_pat = m_20c.GetResAnmTexPat("PB_switch");
    mTexAnm1.create(res_mdl_body, switch_tex_pat, &mAllocator);

    for (int i = 0; i < (int)ARRAY_SIZE(mCapModels); i++) {
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
        if (m_868 == 0) {
            d3d::setMatCullMode(&mInfo[mPyPlayerMode].mMdl2, 2, GX_CULL_ALL);
            d3d::setMatCullMode(&mInfo[mPyPlayerMode].mMdl2, 3, GX_CULL_BACK);
        } else {
            d3d::setMatCullMode(&mInfo[mPyPlayerMode].mMdl2, 2, GX_CULL_BACK);
            d3d::setMatCullMode(&mInfo[mPyPlayerMode].mMdl2, 3, GX_CULL_ALL);
        }
    }

    mMtx_c mtx;
    getJointMtx(&mtx, 15);
    mCapModels[m_86c].setLocalMtx(&mtx);
    mCapModels[m_86c].calc(false);
}

void dMarioMdl_c::play() {
    dPlayerMdl_c::play();
    mCapModels[m_86c].play();
}

void dMarioMdl_c::draw() {
    dPlayerMdl_c::draw();

    if (m_178 & 1) {
        mCapModels[m_86c].entry();
        setSoftLight(mCapModels[m_86c]);
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
    bool ret = dPlayerMdl_c::setPersonalAnm(anmID, outAnmChr, c);
    if (ret) {
        return true;
    }

    char anm_name[32];
    ret = false;

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

        return false;
    }

    return ret;
}

void dMarioMdl_c::FUN_800d4750(int mode) {
    dPlayerMdl_c::FUN_800d4750(mode);

    if (mPyPlayerMode == 1) {
        m_86c = 1;
    } else {
        m_86c = 0;
    }
}

void dMarioMdl_c::fn_800cab00(int isHatLess) {
    if (daPyMng_c::mBonusNoCap && !m_151) {
        m_868 = 1;
        m_17c &= ~1;
    } else {
        m_868 = isHatLess;
        if (isHatLess == 1) {
            m_17c |= 1;
        } else {
            m_17c &= ~1;
        }
    }
}

float dMarioMdl_c::getAimMotionShareScale() {

    static float scMarioHipScale[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static float scMarioHangCliffScale[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static float scLuigiHipScale[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static float scLuigiOrgHipScale[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    static float scLuigiHangCliffScale[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

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
            f1 = scLuigiHipScale[mode];
        } else {
            f1 = scLuigiOrgHipScale[mode];
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
