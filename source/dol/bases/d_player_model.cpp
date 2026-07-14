#include <game/bases/d_player_model.hpp>
#include <game/bases/d_res_mng.hpp>

dPlayerMdl_c::dPlayerMdl_c(u8 val) : dPyMdlBase_c(val),
    m_770(0), m_774(0), m_77c(0),
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
    mInfo[m_770].mMdl2.setLocalMtx(&mtx);
    mInfo[m_770].mMdl2.calc(false);

    mMtx_c mtx2;
    getJointMtx(&mtx2, m_77c);
    mtx2.multVecZero(mHatPosMaybe);
    mtx2.concat(mMtx_c::createTrans(0.0f, -(*(((float *) &mpArcNames[m_770]) + 10)), 0.0f));
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

    mInfo[m_770].mMdl2.play();
    if (mCurrHeadPatID == 1) {
        if (mNextPatSwitchTimer != 0) {
            getHeadTexAnm()->setRate(0.0f, 1);
            mNextPatSwitchTimer--;
        } else {
            getHeadTexAnm()->setRate(1.0f, 1);
            if (getHeadTexAnm()->checkFrame(0.0f, 1)) {
                mNextPatSwitchTimer = cM::rndF(60.0f) + 3;
            }
        }
    }
    getHeadTexAnm()->play(1);
    mTexAnm1.play();
    m_780 -= m_782;
}
