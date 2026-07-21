#include <game/bases/d_kinopio_model.hpp>

const dPlayerMdl_c::ModelInfo_s scArcDt[1] = {
    {
        "Kinopio", "K_rcha",
        "KB_model", "SKB_model", "PLKB_model", "PKB_model",
        "KH_model", "SKH_model", "PLKH_model", "PKH_model",
        18.0f, 13.0f, 18.0f, 18.0f
    }
};

///< @unofficial
const char * scAnmPat1[] = { "KH_switch", "KH_switch", "PLKH_switch", "PKH_switch" };

///< @unofficial
const char * scAnmPat2[] = { "KB_switch", "KB_switch", "PLKB_switch", "PKB_switch" };

dKinopioMdl_c::dKinopioMdl_c(u8 modelType) : dPlayerMdl_c(modelType)
{
    mpArcNames = &scArcDt[0];
    m_77c = 15;
}

dKinopioMdl_c::~dKinopioMdl_c() {}

void dKinopioMdl_c::initialize() {
    dPlayerMdl_c::initialize();
}

void dKinopioMdl_c::createPlayerModel() {
    dPlayerMdl_c::createPlayerModel();

    nw4r::g3d::ResAnmTexPat anmPatHeadNormal = m_20c.GetResAnmTexPat("PH_wait");
    nw4r::g3d::ResMdl resMdlHeadNormal = m_20c.GetResMdl(mpArcNames->mModelNameHead[MODEL_NAME_NORMAL]);
    mTexAnm2.create(resMdlHeadNormal, anmPatHeadNormal, &mAllocator, nullptr, 2);

    nw4r::g3d::ResMdl resMdlBodyNormal = m_20c.GetResMdl(mpArcNames->mModelNameBody[MODEL_NAME_NORMAL]);
    nw4r::g3d::ResAnmTexPat anmPatBodyNormal = m_20c.GetResAnmTexPat("KB_switch");
    mTexAnm1.create(resMdlBodyNormal, anmPatBodyNormal, &mAllocator, nullptr, 1);

    nw4r::g3d::ResAnmTexPat anmPatHeadProp = m_20c.GetResAnmTexPat("PH_wait");
    nw4r::g3d::ResMdl resMdlHeadProp = m_20c.GetResMdl(mpArcNames->mModelNameHead[MODEL_NAME_PROPELLER]);
    mTexAnmPropeller.create(resMdlHeadProp, anmPatHeadProp, &mAllocator, nullptr, 2);

    nw4r::g3d::ResMdl resMdlBodyProp = m_20c.GetResMdl(mpArcNames->mModelNameBody[MODEL_NAME_PROPELLER]);
    nw4r::g3d::ResAnmTexPat anmPatBodyProp = m_20c.GetResAnmTexPat("PLKB_switch");
    mAnimPropeller.create(resMdlBodyProp, anmPatBodyProp, &mAllocator, nullptr, 1);

    nw4r::g3d::ResAnmTexPat anmPatHeadPeng = m_20c.GetResAnmTexPat("PH_wait");
    nw4r::g3d::ResMdl resMdlHeadPeng = m_20c.GetResMdl(mpArcNames->mModelNameHead[MODEL_NAME_PENGUIN]);
    mTexAnmPenguin.create(resMdlHeadPeng, anmPatHeadPeng, &mAllocator, nullptr, 2);

    nw4r::g3d::ResMdl resMdlBodyPeng = m_20c.GetResMdl(mpArcNames->mModelNameBody[MODEL_NAME_PENGUIN]);
    nw4r::g3d::ResAnmTexPat anmPatBodyPeng = m_20c.GetResAnmTexPat("PKB_switch");
    mAnimPenguin.create(resMdlBodyPeng, anmPatBodyPeng, &mAllocator, nullptr, 1);
}

void dKinopioMdl_c::_calc() {
    dPlayerMdl_c::_calc();
}

void dKinopioMdl_c::play() {
    dPlayerMdl_c::play();
}

void dKinopioMdl_c::draw() {
    dPlayerMdl_c::draw();
}

void dKinopioMdl_c::setColorType(u8 type) {
    mPowerupTex = type;

    float f1 = 0.0f;
    if (!(mPyPlayerMode != PLAYER_MODE_PROPELLER && mPyPlayerMode != PLAYER_MODE_PENGUIN)) {
        if (m_151 == 3) {
            f1 = 1.0f;
        }
    } else {
        switch (m_151) {
            case 2:
                f1 = 0.0f;
                break;
            case 3:
                f1 = 3.0f;
                break;
            case 4:
                f1 = 6.0f;
                break;
        }

        if (type == POWERUP_MUSHROOM) {
            f1 += 1.0f;
        } else if (type == POWERUP_FIRE_FLOWER) {
            f1 += 2.0f;
        }
    }

    nw4r::g3d::ResAnmTexPat anmPatHead = m_20c.GetResAnmTexPat(scAnmPat1[mPyPlayerMode]);
    getHeadTexAnm().setAnm(getMdl2(), anmPatHead, 0, m3d::PLAYMODE_INHERIT);
    getHeadTexAnm().setRate(0.0f, 0);
    getHeadTexAnm().setFrame(f1, 0);
    getMdl2().setAnm(getHeadTexAnm());

    nw4r::g3d::ResAnmTexPat anmPatBody = m_20c.GetResAnmTexPat(scAnmPat2[mPyPlayerMode]);
    getBodyTexAnm().setAnm(*getBodyMdl(), anmPatBody, 0, m3d::PLAYMODE_INHERIT);
    getBodyTexAnm().setRate(0.0f, 0);
    getBodyTexAnm().setFrame(f1, 0);
    getBodyMdl()->setAnm(getBodyTexAnm());
}

void dKinopioMdl_c::setTexAnmType(TexAnmType_e type) {
    char anmName[32];
    bool valid = false;
    switch (type) {
        case TEX_ANM_GOAL_PUTON_CAP:
            strncpy(anmName, "KH_goal_puton_cap", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case TEX_ANM_P_GOAL_PUTON_CAP:
            strncpy(anmName, "KH_P_goal_puton_cap", ARRAY_SIZE(anmName));
            valid = true;
            break;
        default:
            break;
    }
    if (valid) {
        mCurrTexAnmType = type;
        nw4r::g3d::ResAnmTexPat anm = m_20c.GetResAnmTexPat(anmName);
        _setHeadTexAnm(anm, m3d::FORWARD_ONCE, 1.0f, 0.0f);
    } else {
        dPlayerMdl_c::setTexAnmType(type);
    }
}

m3d::anmTexPat_c &dKinopioMdl_c::getBodyTexAnm() {
    if (mPyPlayerMode == PLAYER_MODE_PROPELLER) {
        return mAnimPropeller;
    }
    if (mPyPlayerMode == PLAYER_MODE_PENGUIN) {
        return mAnimPenguin;
    }
    return mTexAnm1;
}

bool dKinopioMdl_c::setPersonalAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr, int c) {
    char anmName[32];
    bool valid = false;

    if (mPyPlayerMode == PLAYER_MODE_PENGUIN) {
        switch (anmID) {
            case PLAYER_ANIM_WAIT:
                strncpy(anmName, "PKB_wait", ARRAY_SIZE(anmName));
                valid = true;
                break;
            case PLAYER_ANIM_STAR_ROLL_DUPLICATE:
                strncpy(anmName, "PKB_star_roll", ARRAY_SIZE(anmName));
                valid = true;
                break;
            case PLAYER_ANIM_P_GOAL_PUTON_CAP:
                strncpy(anmName, "PKB_goal_puton_cap", ARRAY_SIZE(anmName));
                valid = true;
                break;
            default:
                break;
        }
        if (valid) {
            *outAnmChr = m_214.GetResAnmChr(anmName);
            if (c == 0) {
                mFlags |= FLAG_0080_0000 | FLAG_0040_0000;
            }
            m_164 |= FLAG_0080_0000 | FLAG_0040_0000;
            return true;
        }
    }

    if (dPlayerMdl_c::setPersonalAnm(anmID, outAnmChr, c)) {
        return true;
    }

    switch (anmID) {
        case PLAYER_ANIM_WAIT:
            strncpy(anmName, "KB_wait", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_ROLL_JUMP:
            strncpy(anmName, "KB_roll_jump", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_STOOP:
            strncpy(anmName, "KB_stoop", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_STOOP_START:
            strncpy(anmName, "KB_stoop_start", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_MONKEY_START:
            strncpy(anmName, "KB_monkey_start", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_MONKEY_WAIT_R:
            strncpy(anmName, "KB_monkey_wait_r", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_MONKEY_WAIT_L:
            strncpy(anmName, "KB_monkey_wait_l", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_MONKEY_R_TO_L:
            strncpy(anmName, "KB_monkey_r_to_l", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_MONKEY_L_TO_R:
            strncpy(anmName, "KB_monkey_l_to_r", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_GOAL_PUTON_CAP:
            strncpy(anmName, "KB_goal_puton_cap", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_STAR_ROLL_DUPLICATE:
            strncpy(anmName, "KB_star_roll", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_S_JUMPED:
            strncpy(anmName, "KB_Sjumped", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_WAIT_DUPLICATE:
            strncpy(anmName, "KB_demo_talk", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_WAIT_DUPLICATE2:
            strncpy(anmName, "KB_busy_wait", ARRAY_SIZE(anmName));
            valid = true;
            break;
        case PLAYER_ANIM_ENDING_WAIT:
            strncpy(anmName, "KB_ending_wait", ARRAY_SIZE(anmName));
            valid = true;
            break;
    }

    if (valid) {
        *outAnmChr = m_214.GetResAnmChr(anmName);
        if (c == 0) {
            mFlags |= FLAG_0040_0000;
        }
        m_164 |= FLAG_0040_0000;
        return true;
    }

    return false;
}

bool dKinopioMdl_c::setPersonalRideAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr) {
    if (dPlayerMdl_c::setPersonalRideAnm(anmID, outAnmChr)) {
        return true;
    }
    if (anmID == PLAYER_ANIM_GOAL_PUTON_CAPF) {
        nw4r::g3d::ResAnmChr anm = m_214.GetResAnmChr("KB_Rgoal_puton_cap");
        *outAnmChr = anm;
        mFlags |= FLAG_0040_0000;
        m_164 |= FLAG_0040_0000;
        return true;
    }
    return false;
}

float dKinopioMdl_c::getAimMotionShareScale() {
    static const float scKinopioHipScale[4] = { 0.6f, 0.377f, 0.6f, 0.45f };
    static const float scKinopioOrgHipScale[4] = { 1.0f, 0.628f, 1.0f, 1.0f };
    static const float scKinopioHangCliffScale[4] = { 0.15f, 0.2f, 0.15f, 0.37f };

    PlayerMode_e playerMode = mPyPlayerMode;
    if (mFlags & FLAG_0080_0000 || mFlags & FLAG_0100_0000) {
        playerMode = PLAYER_MODE_NORMAL;
    }

    float scale;
    if (mFlags & FLAG_0040_0000) {
        scale = scKinopioOrgHipScale[playerMode];
    } else {
        scale = scKinopioHipScale[playerMode];
    }

    if (!(mFlags & FLAG_0000_0008)) {
        return scale;
    }

    scale += scKinopioHangCliffScale[playerMode];
    if (mPlayerMode != PLAYER_MODE_PENGUIN) {
        return scale;
    }
    return scale - 0.05f;
}

void dKinopioMdl_c::callbackTimingA(nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResNode resNode) {
    dPlayerMdl_c::callbackTimingA(anmRes, resNode);
}
