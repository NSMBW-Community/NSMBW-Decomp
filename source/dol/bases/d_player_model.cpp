#include <game/bases/d_player_model.hpp>
#include <game/bases/d_3d.hpp>
#include <game/bases/d_ef.hpp>
#include <game/bases/d_player_model_manager.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/cLib/c_lib.hpp>
#include <nw4r/ut.h>

const int dPlayerMdl_c::scPropelRollBaseSpeed = 0x400;

dPlayerMdl_c::dPlayerMdl_c(u8 val) : dPyMdlBase_c(val),
    mPyPlayerMode(PLAYER_MODE_NORMAL), mOtherPlayerMode(PLAYER_MODE_NORMAL), m_77c(0),
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
        nodePos = resNode.GetTranslate();
        mOtherInf1[i].len1 = nodePos.Len();
        mOtherInf1[i].lensq1 = nodePos.LenSq();

        resNode = mdlMain.GetResNode(4);
        nodePos = resNode.GetTranslate();
        mOtherInf1[i].len2 = nodePos.Len();
        mOtherInf1[i].lensq2 = nodePos.LenSq();

        resNode = mdlMain.GetResNode(6);
        nodePos = resNode.GetTranslate();
        mOtherInf2[i].len1 = nodePos.Len();
        mOtherInf2[i].lensq1 = nodePos.LenSq();

        resNode = mdlMain.GetResNode(7);
        nodePos = resNode.GetTranslate();
        mOtherInf2[i].len2 = nodePos.Len();
        mOtherInf2[i].lensq2 = nodePos.LenSq();
    }

    nw4r::g3d::ResAnmChr anm = m_210.GetResAnmChr("wait");
    getFootAnm().create(mdlMain, anm, &mAllocator, nullptr);
    getBodyAnm().create(mdlMain, anm, &mAllocator, nullptr);

    mBlendAnm.create(mdlMain, 2, &mAllocator, nullptr);
    mBlendAnm.attach(0, &getFootAnm(), 1.0f);
    mBlendAnm.attach(1, &getBodyAnm(), 1.0f);

    static const char *starAnmNameB[] = {
        "PB_star_color",
        "PB_star_color2",
        "PB_star_color2"
    };

    mdlMain = m_20c.GetResMdl(mpArcNames[2]);
    nw4r::g3d::ResAnmClr starAnm1 = m_210.GetResAnmClr(starAnmNameB[m_180]);
    mMatClrAnm1.create(mdlMain, starAnm1, &mAllocator);

    static const char *starAnmNameH[] = {
        "PH_star_color",
        "PH_star_color2",
        "PH_star_color2"
    };

    nw4r::g3d::ResMdl starMdl2 = m_20c.GetResMdl(mpArcNames[6]);
    nw4r::g3d::ResAnmClr starAnm2 = m_210.GetResAnmClr(starAnmNameH[m_180]);
    mMatClrAnm2.create(starMdl2, starAnm2, &mAllocator);
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
    mtx2.concat(mMtx_c::createTrans(0.0f, -(*(((float *) &mpArcNames[mPyPlayerMode]) + 10)), 0.0f));
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

const dPyMdlBase_c::TexAnmData_s dPlayerMdl_c::scTexAnmData[TEX_ANM_COUNT] = {
    { "PH_wait", m3d::FORWARD_LOOP, 0.0f, 0.0f },
    { "PH_wait", m3d::FORWARD_LOOP, 1.0f, 0.0f },
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
    if (mPyPlayerMode == PLAYER_MODE_PROPELLER) {
        return mTexAnmPropeller;
    } else if (mPyPlayerMode == PLAYER_MODE_PENGUIN) {
        return mTexAnmPenguin;
    } else {
        return mTexAnm2;
    }
}

bool dPlayerMdl_c::setPersonalRideAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr) {
    if (mPyPlayerMode == PLAYER_MODE_PENGUIN) {
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

    if (mPyPlayerMode == PLAYER_MODE_PROPELLER) {
        bool found = false;
        switch (anmID) {
            case PLAYER_ANIM_LOW_WALK_START:
                *outAnmChr = m_210.GetResAnmChr("PL_low_walk_start");
                found = true;
                break;
            case PLAYER_ANIM_LOW_WALK:
                *outAnmChr = m_210.GetResAnmChr("PL_low_walk");
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
    } else if (mPyPlayerMode == PLAYER_MODE_PENGUIN && scPyAnmData[anmID].mPenguinName != nullptr) {
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
        if (mPyPlayerMode == PLAYER_MODE_PENGUIN && jumpType == 5 && mJumpAnmVariant == 0) {
            strncpy(anmNameBuf, "P_jump", 0x20);
        }
        return true;
    }
    return false;
}

void dPlayerMdl_c::copyAnm() {
    s8 otherPlayerMode = mOtherPlayerMode;
    s8 currPlayerMode = mPyPlayerMode;
    if (otherPlayerMode != currPlayerMode) {
        mInfo[otherPlayerMode].mMdl1.removeAnm(nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_CHR);
        mInfo[currPlayerMode].mMdl1.setAnm(mBlendAnm);
        mInfo[otherPlayerMode].mMdl2.removeAnm(nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_TEXPAT);
        mInfo[otherPlayerMode].mMdl1.removeAnm(nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_TEXPAT);

        if (m_17c & 0x100) {
            removeStarAnm(&mInfo[otherPlayerMode].mMdl2);
            removeStarAnm(&mInfo[otherPlayerMode].mMdl1);

            setStarAnm(&mInfo[currPlayerMode].mMdl2, &mMatClrAnm2);
            setStarAnm(&mInfo[currPlayerMode].mMdl1, &mMatClrAnm1);
        }
    }
    if (m_15c == PLAYER_ANIM_NONE) {
        int prevAnim = mCurrAnmID;
        if (mCurrAnmID < 0) {
            prevAnim = PLAYER_ANIM_WAIT;
        }
        mCurrAnmID = PLAYER_ANIM_NONE;
        dPyMdlBase_c::setAnm(prevAnim, getFootAnm().getRate(), 0.0f, getFootAnm().getFrame());
    }
}

void dPlayerMdl_c::_setFootAnm(nw4r::g3d::ResAnmChr &anmChr, m3d::playMode_e playMode, float rate, float frame, float p5) {
    nw4r::g3d::ResMdl mdl = m_20c.GetResMdl(*(&mpArcNames[mPyPlayerMode] + 2));
    m3d::anmChr_c &anm = getFootAnm();

    anm.setAnm(*getBodyMdl(), anmChr, playMode);
    anm.setRate(rate);
    anm.setFrame(frame);
    float f = 0.0f;
    if (frame >= anmChr.GetNumFrame()) {
        f = 1.0f;
    }
    getBodyMdl()->setAnm(mBlendAnm, p5);
    mStoopTimer = p5;
    if (mFlags & 1) {
        mStoopOffsetTarget = dPyMdlMng_c::m_hio.getOffsetTarget(m_151, mPyPlayerMode);
    } else {
        mStoopOffsetTarget = 0.0f;
    }
    calcStoopOffset();
}

void dPlayerMdl_c::_setBodyAnm(nw4r::g3d::ResAnmChr &anmChr, m3d::playMode_e playMode, float rate, float frame, float p5) {
    nw4r::g3d::ResMdl mdl = m_20c.GetResMdl(*(&mpArcNames[mPyPlayerMode] + 2));
    m3d::anmChr_c &anm = getBodyAnm();

    anm.setAnm(*getBodyMdl(), anmChr, playMode);
    anm.setRate(rate);
    anm.setFrame(frame);
    float f = 0.0f;
    if (frame >= anmChr.GetNumFrame()) {
        f = 1.0f;
    }
    getBodyMdl()->setAnm(mBlendAnm, p5);
}

void dPlayerMdl_c::setAnmBind() {
    nw4r::g3d::ResMdl mdl = m_20c.GetResMdl(*(&mpArcNames[mPyPlayerMode] + 2));

    nw4r::g3d::AnmObjChr *footObj = getFootAnm().getObj();
    footObj->Release();

    nw4r::g3d::AnmObjChr *bodyObj = getBodyAnm().getObj();
    bodyObj->Release();

    footObj->Bind(mdl, 5, nw4r::g3d::AnmObjChr::BIND_PARTIAL);
    footObj->Bind(mdl, 2, nw4r::g3d::AnmObjChr::BIND_PARTIAL);
    footObj->Bind(mdl, 1, nw4r::g3d::AnmObjChr::BIND_ONE);
    footObj->Bind(mdl, 0, nw4r::g3d::AnmObjChr::BIND_ONE);
    bodyObj->Bind(mdl, 8, nw4r::g3d::AnmObjChr::BIND_PARTIAL);
}

void dPlayerMdl_c::FUN_800d4750(int mode) {
    mOtherPlayerMode = mPyPlayerMode;
    mPyPlayerMode = (PlayerMode_e) mode;
}

void dPlayerMdl_c::setPlayerMode(int mode) {
    static const int l_player_model_dt[] = { 1, 0, 0, 1, 2, 3, 0 };

    mPlayerMode = mode;
    FUN_800d4750(l_player_model_dt[mode]);
    if (mOtherPlayerMode != mPyPlayerMode) {
        copyAnm();
        calcBlendRate();
        resetTexAnmType();
        mPowerupTex = -1;
    }
    switch (mPlayerMode) {
        case 2:
            setColorType(1);
            break;
        case 6:
            setColorType(2);
            break;
        default:
            setColorType(0);
            break;
    }
    _calc();
}

void dPlayerMdl_c::setColorType(u8 type) {}

void dPlayerMdl_c::setDark(int value) {
    float factor = 1.0f - value / 255.0f;
    int val = factor * 255;
    getMdl2().setTevColorAll(GX_TEVREG0, nw4r::ut::Color(val, val, val, val), false);
    getBodyMdl()->setTevColorAll(GX_TEVREG0, nw4r::ut::Color(val, val, val, val), false);

    nw4r::g3d::ResMdl mdl = getMdl2().getResMdl();
    nw4r::g3d::ResMatTevColor matTevColor;
    GXColor color;
    for (ulong i = 0; i < mdl.GetResMatNumEntries(); i++) {
        matTevColor = mdl.GetResMat(i).GetResMatTevColor();
        matTevColor.GXGetTevColor(GX_TEVREG1, &color);
        color.a *= factor;
        getMdl2().setTevColor(i, GX_TEVREG1, color, false);
    }
}

bool dPlayerMdl_c::getHeadPropelJointMtx(mMtx_c *mtx) {
    if (mPyPlayerMode == PLAYER_MODE_PROPELLER) {
        getMdl2().getNodeWorldMtx(1, mtx);
        return true;
    }
    return false;
}

void dPlayerMdl_c::onStarAnm() {
    if (!isStarAnm()) {
        dPyMdlBase_c::onStarAnm();
        setStarAnm(&getMdl2(), &mMatClrAnm2);
        setStarAnm(getBodyMdl(), &mMatClrAnm1);
    }
}

void dPlayerMdl_c::offStarAnm() {
    if (isStarAnm()) {
        dPyMdlBase_c::offStarAnm();
        removeStarAnm(&getMdl2());
        removeStarAnm(getBodyMdl());
    }
}

void dPlayerMdl_c::changeLightTexture(m3d::mdl_c *mdl, _GXTexMapID texMapID) {
    nw4r::g3d::ResMdl resMdl = mdl->getResMdl();
    nw4r::g3d::ScnMdl *scnMdl = nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mdl->getScn());
    if (scnMdl != nullptr) {
        for (int i = 0; i < resMdl.GetResMatNumEntries(); i++) {
            nw4r::g3d::ScnMdl::CopiedMatAccess cma(scnMdl, i);
            nw4r::g3d::ResTexObj tex = cma.GetResTexObj(false);
            if (tex.IsValidTexObj(texMapID)) {
                nw4r::g3d::ResTev resTev = cma.GetResTev(false);

                GXTexCoordID coordID;
                GXTexMapID mapID;
                GXChannelID channelID;
                resTev.GXGetTevOrder(GX_TEVSTAGE1, &coordID, &mapID, &channelID);
                resTev.GXSetTevOrder(GX_TEVSTAGE1, coordID, texMapID, channelID);
                resTev.DCStore(false);
            }
        }
    }
}

void dPlayerMdl_c::setStarAnm(m3d::mdl_c *mdl, m3d::anmMatClr_c *matClr) {
    changeLightTexture(mdl, GX_TEXMAP3);
    mdl->setAnm(*matClr);
}

void dPlayerMdl_c::removeStarAnm(m3d::mdl_c *mdl) {
    changeLightTexture(mdl, GX_TEXMAP2);
    mdl->removeAnm(nw4r::g3d::ScnMdlSimple::ANMOBJTYPE_MATCLR);
    d3d::resetTevColorAll(mdl, GX_TEVREG0, false);
    d3d::resetTevKColorAll(mdl, GX_KCOLOR3, false);
}

void dPlayerMdl_c::calcBlendRate() {
    float rate = getBodyMdl()->mCallback.mCalcRatio.getSlerpParam();
    float scale = getAimMotionShareScale();
    if (rate == 1.0f) {
        mMotionShareScale.x = mMotionShareScale.y = mMotionShareScale.z = scale;
        return;
    }
    if (mMotionShareScale.y != scale) {
        mMotionShareScale.z = mMotionShareScale.y;
        mMotionShareScale.y = scale;
    }
    mMotionShareScale.x = scale * rate + mMotionShareScale.z * (1.0f - rate);
}

void dPlayerMdl_c::callbackTimingA(nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResNode resNode) {
    int id = resNode.GetID();
    if (id != 0) {
        if (id == 1) {
            mVec3_c v;
            anmRes->GetTranslate(&v);
            v *= mMotionShareScale.x;
            anmRes->SetTranslate(v);
        } else {
            anmRes->SetTranslate(&resNode.GetTranslate());
            if (id == 11 || id == 14) {
                if (mPyPlayerMode == PLAYER_MODE_PENGUIN && !(m_164 & 0x800000)) {
                    anmRes->SetRotateDeg(&resNode.GetRot());
                    anmRes->SetScale(&resNode.GetScale());
                }
            }
        }
    }
}

void dPlayerMdl_c::nodeCallback_c::timingA(ulong nodeId, nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl) {
    mpOwner->callbackTimingA(anmRes, resMdl.GetResNode(nodeId));
}

void dPlayerMdl_c::nodeCallback_c::timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl) {
    nw4r::g3d::ResNode resNode = resMdl.GetResNode(nodeId);
    if (strcmp(resNode.GetName(), "face_1") != 0) {
        return;
    }

    int v = mpOwner->m_204;
    if (v == 1 || v == 2) {
        mMtx_c mtx;
        manip->GetMatrix(&mtx);
        mVec3_c vec;
        mtx.multVecZero(vec);
        mtx.concat(mMtx_c::createTrans(100.0f, 0.0f, 0.0f));
        mVec3_c vec2;
        mtx.multVecZero(vec2);
        s16 angX = cLib::targetAngleX(vec, vec2);
        s16 angY = cLib::targetAngleY(vec, vec2);

        manip->GetMatrix(&mtx);
        if (v == 2) {
            mAng3_c ang = mpOwner->getAng();
            // [Fake match...]
            mAng3_c ang2(ang.x, ang.y, ang.z);
            mtx.YrotM(angY - ang2.y);
        }
        mAng3_c ang = mpOwner->getAng();
        mtx.ZrotM(ang.z);
        manip->SetMatrix(mtx);
    }
}

void dPlayerMdl_c::nodeCallbackPLH_c::timingA(ulong nodeId, nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl) {}

void dPlayerMdl_c::nodeCallbackPLH_c::timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl) {
    if (nodeId != 1) {
        return;
    }
    mMtx_c mtx;
    manip->GetMatrix(&mtx);
    mtx.YrotM(mpOwner->getPropelRollAngle());
    manip->SetMatrix(mtx);

    mVec3_c scale;
    manip->GetScale(&scale);
    scale *= mpOwner->m_784;
    manip->SetScale(scale);
}

void dPlayerMdl_c::setStarEffect() {
    if (!(m_17c & 0x200)) {
        return;
    }

    float scale = 1.0f;
    switch (mPlayerMode) {
        case 0:
            scale = 0.8f;
            break;
        case 3:
            scale = 0.5f;
            break;
    }
    mVec3_c sc = scale * mScale;
    mVec3_c v;
    mBaseMtx.multVecZero(v);
    switch (m_180) {
        case 0: {
            int idx = 0;
            switch (mPlayerMode) {
                case 0:
                case 3:
                    idx = 1;
                    break;
            }
            static const char *sc_starEffectID[] = { "Wm_mr_starkira", "Wm_mr_starkira_s" };
            dEf::createPlayerEffect_change(mAnotherPlayerID, &mLevelEf1, sc_starEffectID[idx], 0, &v, nullptr, &mScale);
            break;
        }
        case 1:
            mLevelEf1.createEffect("Wm_2d_mrstarkira", nullptr, &v, 0, &sc);
            break;
        case 2:
            dEf::createPlayerEffect(mAnotherPlayerID, &mLevelEf1, "Wm_2d_mrstarkira", 0, &v, nullptr, &sc);
            break;
    }
    if (m_180 == 0) {
        mVec3_c s = scale * mScale;
        mVec3_c jointPos;
        getJointPos(&jointPos, 8);
        dEf::createPlayerEffect(mAnotherPlayerID, &mLevelEf2, "Wm_mr_startail", 0, &jointPos, nullptr, &s);
    }
}
