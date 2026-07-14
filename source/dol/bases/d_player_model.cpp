#include <game/bases/d_player_model.hpp>
#include <game/bases/d_res_mng.hpp>

const dPyMdlBase_c::TexAnmData_s dPlayerMdl_c::scTexAnmData[TEX_ANM_COUNT] = {
    { "PH_wait", m3d::FORWARD_LOOP, 0.0f, 0.0f },
    { "PH_wait", m3d::FORWARD_LOOP, 0.0f, 0.0f },
    { "PH_goal_puton_cap", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_PL_goal_puton_cap", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_P_goal_puton_cap", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_P_Rgoal_puton_cap", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_dam", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_jump", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_jumped", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_throw", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_courese_in", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_coin_comp", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_dm_escort", m3d::FORWARD_ONCE, 1.0f, 0.0f },
    { "PH_dm_glad", m3d::FORWARD_LOOP, 1.0f, 0.0f }
};

dPlayerMdl_c::dPlayerMdl_c(u8 val) : dPyMdlBase_c(val),
    mPlayerMode(PLAYER_MODE_NORMAL), m_774(0), m_77c(0),
    m_780(0), m_782(0x400),
    m_784(1.0f), mMotionShareScale(1.0f, 1.0f, 1.0f),
    mCallback(this),
    mCallbackPLH(this) {}

dPlayerMdl_c::~dPlayerMdl_c() {}

void dPlayerMdl_c::createModel() {
    getPlayerObjectRes();
    createPlayerModel();
    mMotionShareScale.x = mMotionShareScale.y = mMotionShareScale.z = getAimMotionShareScale();
}

void dPlayerMdl_c::initialize() {
    for (int i = 0; i < 4; i++) {
        mInfo[i].mMdl1.setCallback(&mCallback);
    }
    mInfo[2].mMdl2.setCallback(&mCallbackPLH);
    setInitTexAnm();
}

void dPlayerMdl_c::getPlayerObjectRes() {
    m_20c = dResMng_c::m_instance->getRes(mpArcNames[0], "g3d/model.brres");
    m_210 = dResMng_c::m_instance->getRes("P_rcha", "g3d/model.brres");
    m_214 = dResMng_c::m_instance->getRes(mpArcNames[1], "g3d/model.brres");
}

void dPlayerMdl_c::createPlayerModel() {
    nw4r::g3d::ResMdl mdlMain;
    for (int i = 0; i < 4; i++) {
        nw4r::g3d::ResMdl mdl = m_20c.GetResMdl(mpArcNames[6 + i]);
        mInfo[i].mMdl2.create(mdl, &mAllocator,
            nw4r::g3d::ScnMdl::BUFFER_RESTEV | nw4r::g3d::ScnMdl::ANM_MATCLR | nw4r::g3d::ScnMdl::ANM_TEXPAT |
            nw4r::g3d::ScnMdl::ANM_VIS | nw4r::g3d::ScnMdl::BUFFER_RESMATMISC | nw4r::g3d::ScnMdl::BUFFER_RESGENMODE,
            1, nullptr
        );
        setSoftLight(mInfo[i].mMdl2);

        mdlMain = m_20c.GetResMdl(mpArcNames[2 + i]);
        mInfo[i].mMdl1.create(mdlMain, &mAllocator,
            nw4r::g3d::ScnMdl::BUFFER_RESTEV | nw4r::g3d::ScnMdl::ANM_MATCLR | nw4r::g3d::ScnMdl::ANM_TEXPAT |
            nw4r::g3d::ScnMdl::BUFFER_RESMATMISC | nw4r::g3d::ScnMdl::BUFFER_RESGENMODE,
            1, nullptr
        );
        setSoftLight(mInfo[i].mMdl1);

        nw4r::g3d::ResNode resNode;
        mVec3_c nodePos;

        resNode = mdlMain.GetResNode(3);
        nodePos.x = resNode.ref().translate.x;
        nodePos.y = resNode.ref().translate.y;
        nodePos.z = resNode.ref().translate.z;
        mOtherInf1[i].len1 = VEC3Len(&nodePos);
        mOtherInf1[i].lensq1 = VEC3LenSq(&nodePos);

        resNode = mdlMain.GetResNode(4);
        nodePos.x = resNode.ref().translate.x;
        nodePos.y = resNode.ref().translate.y;
        nodePos.z = resNode.ref().translate.z;
        mOtherInf1[i].len2 = VEC3Len(&nodePos);
        mOtherInf1[i].lensq2 = VEC3LenSq(&nodePos);

        resNode = mdlMain.GetResNode(6);
        nodePos.x = resNode.ref().translate.x;
        nodePos.y = resNode.ref().translate.y;
        nodePos.z = resNode.ref().translate.z;
        mOtherInf2[i].len1 = VEC3Len(&nodePos);
        mOtherInf2[i].lensq1 = VEC3LenSq(&nodePos);

        resNode = mdlMain.GetResNode(7);
        nodePos.x = resNode.ref().translate.x;
        nodePos.y = resNode.ref().translate.y;
        nodePos.z = resNode.ref().translate.z;
        mOtherInf2[i].len2 = VEC3Len(&nodePos);
        mOtherInf2[i].lensq2 = VEC3LenSq(&nodePos);
    }

    nw4r::g3d::ResAnmChr anm = m_210.GetResAnmChr("wait");
    getFootAnm().create(mdlMain, anm, &mAllocator, nullptr);
    getBodyAnm().create(mdlMain, anm, &mAllocator, nullptr);

    mBlendAnm.create(mdlMain, 2, &mAllocator, nullptr);
    mBlendAnm.attach(0, &getFootAnm(), 1.0f);
    mBlendAnm.attach(1, &getBodyAnm(), 1.0f);

    nw4r::g3d::ResMdl starMdl;
    nw4r::g3d::ResAnmClr starAnm;

    static const char *starAnmName1[] = {
        "PB_star_color",
        "PB_star_color2",
        "PB_star_color2"
    };
    static const char *starAnmName2[] = {
        "PH_star_color",
        "PH_star_color2",
        "PH_star_color2"
    };

    starMdl = m_20c.GetResMdl(mpArcNames[2]);
    starAnm = m_210.GetResAnmClr(starAnmName1[m_180]);
    mMatClrAnm1.create(starMdl, starAnm, &mAllocator, nullptr, 1);

    starMdl = m_20c.GetResMdl(mpArcNames[6]);
    starAnm = m_210.GetResAnmClr(starAnmName2[m_180]);
    mMatClrAnm2.create(starMdl, starAnm, &mAllocator, nullptr, 1);
}

void dPlayerMdl_c::_calc() {
    calcBlendRate();
    m_208 = 0;
    getBodyMdl()->setLocalMtx(&mBaseMtx);
    getBodyMdl()->calc(false);

    mMtx_c mtx;
    getJointMtx(&mtx, m_77c);
    mtx.concat(mMtx_c::createTrans(mHeadOffset));
    getMdl2().setLocalMtx(&mtx);
    getMdl2().calc(false);

    mMtx_c mtx2;
    getJointMtx(&mtx2, m_77c);
    mtx2.multVecZero(mHatPosMaybe);
    mtx2.concat(mMtx_c::createTrans(0.0f, -(*(((float *) &mpArcNames[mPlayerMode]) + 10)), 0.0f));
    mtx2.multVecZero(mHeadPosMaybe);
}

void dPlayerMdl_c::calc2() {
    m_208 = 2;
    getBodyMdl()->setLocalMtx(&mBaseMtx);
    getBodyMdl()->calc(false);
}

void dPlayerMdl_c::play() {
    setStarEffect();
    calcStoopOffset();

    getFootAnm().play();
    getBodyAnm().play();
    mMatClrAnm2.play();
    mMatClrAnm1.play();
    getBodyMdl()->play();

    getMdl2().play();
    if (mCurrTexAnmType == 1) {
        if (mNextPatSwitchTimer != 0) {
            getHeadTexAnm().setRate(0.0f, 1);
            mNextPatSwitchTimer--;
        } else {
            getHeadTexAnm().setRate(1.0f, 1);
            if (getHeadTexAnm().checkFrame(0.0f, 1)) {
                mNextPatSwitchTimer = cM::rndF(60.0f) + 3;
            }
        }
    }
    getHeadTexAnm().play(1);
    mTexAnm1.play();
    m_780 -= m_782;
}

void dPlayerMdl_c::draw() {
    getBodyMdl()->entry();
    getMdl2().entry();
    setSoftLight(*getBodyMdl());
    setSoftLight(getMdl2());
}

void dPlayerMdl_c::setInitTexAnm() {
    nw4r::g3d::ResAnmTexPat pat = m_20c.GetResAnmTexPat("PH_wait");
    _setHeadTexAnm(pat, m3d::FORWARD_LOOP, 0.0f, 0.0f);
}

void dPlayerMdl_c::_setHeadTexAnm(nw4r::g3d::ResAnmTexPat &anmTexPat, m3d::playMode_e playMode, float rate, float frame) {
    getHeadTexAnm().setAnm(getMdl2(), anmTexPat, 1, playMode);
    getHeadTexAnm().setRate(rate, 1);
    getHeadTexAnm().setFrame(frame, 1);
    getMdl2().setAnm(getHeadTexAnm());
}

void dPlayerMdl_c::setTexAnmType(TexAnmType_e anmType) {
    if (mCurrTexAnmType == anmType) {
        return;
    }

    mCurrTexAnmType = anmType;
    const TexAnmData_s &texAnmData = scTexAnmData[anmType];
    nw4r::g3d::ResAnmTexPat texPat = m_20c.GetResAnmTexPat(texAnmData.mName);
    _setHeadTexAnm(texPat, texAnmData.mPlayMode, texAnmData.mRate, texAnmData.mFrame);

    if (mCurrTexAnmType == TEX_ANM_NONE) {
        mNextPatSwitchTimer = 0;
    }
}

void dPlayerMdl_c::resetTexAnmType() {
    TexAnmType_e prevAnmType = mCurrTexAnmType;
    switch (prevAnmType) {
        case TEX_ANM_NONE:
            setInitTexAnm();
            break;
        default:
            mCurrTexAnmType = TEX_ANM_NONE;
            setTexAnmType(prevAnmType);
            break;
    }
}

m3d::anmTexPat_c &dPlayerMdl_c::getHeadTexAnm() {
    if (mPlayerMode == PLAYER_MODE_YOSHI) {
        return mTexAnmYoshi;
    } else if (mPlayerMode == PLAYER_MODE_PENGUIN) {
        return mTexAnmPenguin;
    } else {
        return mTexAnm2;
    }
}

bool dPlayerMdl_c::setPersonalRideAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr) {
    if (mPlayerMode == PLAYER_MODE_PENGUIN) {
        bool found = false;
        switch (anmID) {
            case PLAYER_ANIM_HIP_TO_STOOP:
                *outAnmChr = m_210.GetResAnmChr("P_Rhip_to_stoop");
                found = true;
                break;
            case PLAYER_ANIM_STOOP:
                *outAnmChr = m_210.GetResAnmChr("P_Rstoop");
                found = true;
                break;
            case PLAYER_ANIM_STOOP_START:
                *outAnmChr = m_210.GetResAnmChr("P_Rstoop_start");
                found = true;
                break;
            case PLAYER_ANIM_SLIP_TO_STOOP:
                *outAnmChr = m_210.GetResAnmChr("P_Rslip_to_stoop");
                found = true;
                break;
            case PLAYER_ANIM_RS_EAT:
                *outAnmChr = m_210.GetResAnmChr("P_RSeat");
                found = true;
                break;
            case PLAYER_ANIM_RS_EAT_OUT:
                *outAnmChr = m_210.GetResAnmChr("P_RSeat_out");
                found = true;
                break;
            case PLAYER_ANIM_RS_EAT_SUCCESS:
                *outAnmChr = m_210.GetResAnmChr("P_RSeat_success");
                found = true;
                break;
            case PLAYER_ANIM_RS_EAT_FAIL:
                *outAnmChr = m_210.GetResAnmChr("P_RSeat_fail");
                found = true;
                break;
            case PLAYER_ANIM_RS_EAT_SUCCESSB:
                *outAnmChr = m_210.GetResAnmChr("P_RSeat_successB");
                found = true;
                break;
            case PLAYER_ANIM_S_JUMP:
                *outAnmChr = m_210.GetResAnmChr("P_Rsjump");
                found = true;
                break;
            case PLAYER_ANIM_S_JUMP2:
                *outAnmChr = m_210.GetResAnmChr("P_RSjump2");
                found = true;
                break;
            case PLAYER_ANIM_S_JUMPED:
                *outAnmChr = m_210.GetResAnmChr("P_RSjumped");
                found = true;
                break;
        }

        if (found) {
            mFlags |= 0x800000;
            m_164 |= 0x800000;
            return true;
        }
    }

    return false;
}

bool dPlayerMdl_c::setPersonalAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr, int p3) {
    char anmNameBuf[32];
    if (getJumpAnmName(anmID, anmNameBuf, p3)) {
        *outAnmChr = m_210.GetResAnmChr(anmNameBuf);
        return true;
    }

    if (mPlayerMode == PLAYER_MODE_YOSHI) {
        bool found = false;
        switch (anmID) {
            case PLAYER_ANIM_LOW_WALK_START:
                *outAnmChr = m_210.GetResAnmChr("P_Plow_walk_start");
                found = true;
                break;
            case PLAYER_ANIM_LOW_WALK:
                *outAnmChr = m_210.GetResAnmChr("P_Plow_walk");
                found = true;
                break;
        }
        if (found) {
            if (!p3) {
                mFlags |= 0x1000000;
            }
            m_164 |= 0x1000000;
            return true;
        }
    } else if (mPlayerMode == PLAYER_MODE_PENGUIN && scPyAnmData[anmID].mPenguinName != nullptr) {
        *outAnmChr = m_210.GetResAnmChr(scPyAnmData[anmID].mPenguinName);
        if (!p3) {
            mFlags |= 0x800000;
        }
        m_164 |= 0x800000;
        return true;
    }

    return false;
}

bool dPlayerMdl_c::getJumpAnmName(int jumpType, char *anmNameBuf, int p4) {
    if (dPyMdlBase_c::getJumpAnmName(jumpType, anmNameBuf, p4)) {
        if (mPlayerMode == PLAYER_MODE_PENGUIN && jumpType == 5 && mJumpAnmVariant == 0) {
            strncpy(anmNameBuf, "P_jump", 0x20);
        }
        return true;
    }
    return false;
}
