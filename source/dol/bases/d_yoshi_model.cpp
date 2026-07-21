#include <game/bases/d_yoshi_model.hpp>
#include <game/bases/d_3d.hpp>
#include <game/bases/d_a_yoshi.hpp>
#include <game/bases/d_bc.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/cLib/c_lib.hpp>

const float dYoshiMdl_c::scTongueLengthMax = 70.0f;

dYoshiMdl_c::dYoshiMdl_c(u8 modelType) : dPyMdlBase_c(modelType),
    mColor(0), mNodeCallback(this),
    m_294(0), m_298(0.0f, 0.0f), m_2a0(0),
    m_3f4(0.0f, 0.0f)
{}

dYoshiMdl_c::~dYoshiMdl_c() {}

void dYoshiMdl_c::createModel() {
    mColor = (YoshiColor_e) mAnotherPlayerID;
    mAnotherPlayerID = -1;
    getPlayerObjectRes();
    createPlayerModel();
}

void dYoshiMdl_c::initialize() {
    getBodyMdl()->setCallback(&mNodeCallback);
}

void dYoshiMdl_c::getPlayerObjectRes() {
    m_20c = dResMng_c::m_instance->getRes("Yoshi", "g3d/model.brres");
    m_210 = dResMng_c::m_instance->getRes("P_rcha", "g3d/model.brres");
    m_214 = dResMng_c::m_instance->getRes("Y_rcha", "g3d/model.brres");

    static const char *scYoshiTexRes[] = {
        "Y_TexGreen", "Y_TexRed", "Y_TexYellow", "Y_TexBlue"
    };
    m_250 = dResMng_c::m_instance->getRes(scYoshiTexRes[mColor], "g3d/model.brres");

    nw4r::g3d::ResMdl mdl = m_20c.GetResMdl("YB_model");
    for (ulong i = 0; i < mdl.GetResMatNumEntries(); i++) {
        nw4r::g3d::ResMat resMat = mdl.GetResMat(i);
        nw4r::g3d::ResTexPlttInfo plttInfo = resMat.GetResTexPlttInfo(0);
        plttInfo.Release(resMat.GetResTexObj(), resMat.GetResTlutObj());
        plttInfo.Bind(m_250, resMat.GetResTexObj(), resMat.GetResTlutObj());
        resMat.DCStore(false);
    }
}

void dYoshiMdl_c::createPlayerModel() {
    nw4r::g3d::ResMdl mdl = m_20c.GetResMdl("YB_model");
    getBodyMdl()->create(mdl, &mAllocator,
        nw4r::g3d::ScnMdl::BUFFER_RESTEV | nw4r::g3d::ScnMdl::ANM_TEXPAT |
        nw4r::g3d::ScnMdl::BUFFER_RESGENMODE | nw4r::g3d::ScnMdl::BUFFER_RESMATMISC | nw4r::g3d::ScnMdl::BUFFER_RESANMVIS
    );
    setSoftLight(*getBodyMdl());

    nw4r::g3d::ResAnmChr anm = m_210.GetResAnmChr("wait");
    getFootAnm().create(mdl, anm, &mAllocator);
    getBodyAnm().create(mdl, anm, &mAllocator);

    mBlendAnm.create(mdl, &mAllocator, 2);
    mBlendAnm.attach(0, &getFootAnm(), 1.0f);
    mBlendAnm.attach(1, &getBodyAnm(), 1.0f);

    nw4r::g3d::ResAnmTexPat pat = m_250.GetResAnmTexPat("YB_wait");
    pat.Release();
    pat.Bind(m_250);
    mAnmTexPat.create(mdl, pat, &mAllocator);
    mAnmTexPat.setAnm(*getBodyMdl(), pat, 0, m3d::PLAYMODE_INHERIT);

    getBodyMdl()->setAnm(mAnmTexPat);

    mNextPatSwitchTimer = dGameCom::rndF(60.0f) + 3.0f;
}

void dYoshiMdl_c::_calc() {
    mMtx_c mtx;
    getJointMtx(&mtx, 15);
    mtx.multVecZero(mHatPosMaybe);
    mtx.concat(mMtx_c::createTrans(0.0f, -16.0f, 0.0f));
    mtx.multVecZero(mHeadPosMaybe);
    m3d::mdl_c *mdl = getBodyMdl();
    mdl->setLocalMtx(&mBaseMtx);
    m_208 = 1;
    mdl->calc(false);
    setTongueBendCalc();
    setTongueBgCheck();
}

void dYoshiMdl_c::calc2() {
    m_208 = 2;
    getBodyMdl()->calc(false);
    if (isFlag20()) {
        m_17c &= ~0x20;
    } else {
        sLib::chase(&m_3f4.x, 0.0f, 0.1f);
    }
    float target = 0.0f;
    if (m_294 == 2 && mFlags & FLAG_0000_2000 && mPrevAnmID != PLAYER_ANIM_R_EAT_OUT && mPrevAnmID != PLAYER_ANIM_R_EAT_SUCCESSB) {
        target = 1.0f;
    }
    sLib::chase(&m_3f4.y, target, 0.2f);
}

void dYoshiMdl_c::play() {
    getFootAnm().play();
    getBodyAnm().play();
    getBodyMdl()->play();
    if (mNextPatSwitchTimer != 0) {
        mNextPatSwitchTimer--;
    } else if (mAnmTexPat.isStop(0)) {
        mNextPatSwitchTimer = dGameCom::rndF(60.0f) + 3.0f;
        mAnmTexPat.setFrame(0.0f, 0);
    }
    mAnmTexPat.play();
}

void dYoshiMdl_c::draw() {
    int v = 0;
    if (m_294 == 2) {
        v = 1;
    }
    d3d::setNodeVisibility(getBodyMdl(), 24, v);
    getBodyMdl()->entry();
    setSoftLight(*getBodyMdl());
}

void dYoshiMdl_c::setAnm(int animID, float rate, float c, float frame) {
    dPyMdlBase_c::setAnm(animID, rate, c, frame);
}

bool dYoshiMdl_c::setPersonalAnm(int anmID, nw4r::g3d::ResAnmChr *outAnmChr, int p4) {
    if (scPyAnmData[anmID].mPropellerName != nullptr) {
        *outAnmChr = m_214.GetResAnmChr(scPyAnmData[anmID].mPropellerName);
        if (!p4) {
            mFlags |= FLAG_0010_0000;
        }
        m_164 |= FLAG_0010_0000;
        return true;
    }
    return false;
}

void dYoshiMdl_c::_setFootAnm(nw4r::g3d::ResAnmChr &anmChr, m3d::playMode_e playMode, float rate, float frame, float blendFrame) {
    m3d::anmChr_c &anm = getFootAnm();
    m_218 = anmChr;
    anm.setAnm(*getBodyMdl(), anmChr, playMode);
    anm.setRate(rate);
    anm.setFrame(frame);
    getBodyMdl()->setAnm(mBlendAnm, blendFrame);
}

void dYoshiMdl_c::_setBodyAnm(nw4r::g3d::ResAnmChr &anmChr, m3d::playMode_e playMode, float rate, float frame, float blendFrame) {
    m_21c = anmChr;

    int anmID = getPrevAnmID();
    int newAnmID = PLAYER_ANIM_NONE;
    if (m_164 & FLAG_0000_0020 && mFlags & FLAG_0000_0400) {
        // [Possibly an inline (because of the mPrevAnmID access)]
        switch (mPrevAnmID) {
            case PLAYER_ANIM_R_EAT:
                newAnmID = PLAYER_ANIM_RS_EAT;
                break;
            case PLAYER_ANIM_R_EAT_OUT:
                newAnmID = PLAYER_ANIM_RS_EAT_OUT;
                break;
            case PLAYER_ANIM_R_EAT_SUCCESS:
                newAnmID = PLAYER_ANIM_RS_EAT_SUCCESS;
                break;
            case PLAYER_ANIM_R_EAT_FAIL:
                newAnmID = PLAYER_ANIM_RS_EAT_FAIL;
                break;
            case PLAYER_ANIM_R_EAT_SUCCESSB:
                newAnmID = PLAYER_ANIM_RS_EAT_SUCCESSB;
                break;
            case PLAYER_ANIM_R_EAT_SUCCESSB_DUPLICATE:
                newAnmID = PLAYER_ANIM_RS_EAT_SUCCESSB_DUPLICATE;
                break;
        }
        if (newAnmID != PLAYER_ANIM_NONE) {
            m_21c = m_214.GetResAnmChr(scPyAnmData[newAnmID].mPropellerName);
            anmID = newAnmID;
        }
    }

    m3d::anmChr_c &anm = getBodyAnm();
    anm.setAnm(*getBodyMdl(), m_21c, playMode);
    anm.setRate(rate);
    anm.setFrame(frame);
    getBodyMdl()->setAnm(mBlendAnm, blendFrame);
    if (mpSpinLiftParentMdl != nullptr) {
        if (anmID == PLAYER_ANIM_NONE) {
            anmID = mCurrAnmID;
        }
        mpSpinLiftParentMdl->setRideAnm(anmID, rate, blendFrame, frame);
    }
}

void dYoshiMdl_c::releaseBodyAnm(float f) {
    mPrevAnmID = PLAYER_ANIM_NONE;
    const AnmData_s &anmData = scPyAnmData[mCurrAnmID];
    m_164 = anmData.mFlags;
    nw4r::g3d::ResAnmChr anmChr = getAnmResFile()->GetResAnmChr(anmData.mName);
    setPersonalAnm(mCurrAnmID, &anmChr, 1);
    float f2 = 5.0f;
    if (mFlags & FLAG_0000_0001) {
        f2 = 0.0f;
    }
    _setBodyAnm(anmChr, (m3d::playMode_e) getFootAnm().mPlayMode, getFootAnm().getRate(), getFootAnm().getFrame(), f2);
}

void dYoshiMdl_c::setAnmBind() {
    nw4r::g3d::ResMdl mdl = m_20c.GetResMdl("YB_model");

    nw4r::g3d::AnmObjChr *footObj = getFootAnm().getObj();
    footObj->Release();

    nw4r::g3d::AnmObjChr *bodyObj = getBodyAnm().getObj();
    bodyObj->Release();

    footObj->Bind(mdl, 5, nw4r::g3d::AnmObjChr::BIND_PARTIAL);
    footObj->Bind(mdl, 2, nw4r::g3d::AnmObjChr::BIND_PARTIAL);
    footObj->Bind(mdl, 0, nw4r::g3d::AnmObjChr::BIND_ONE);
    bodyObj->Bind(mdl, 1, nw4r::g3d::AnmObjChr::BIND_ONE);
    bodyObj->Bind(mdl, 8, nw4r::g3d::AnmObjChr::BIND_PARTIAL);
}

void dYoshiMdl_c::setFrame(float frame) {
    dPyMdlBase_c::setFrame(frame);
    if (mpSpinLiftParentMdl != nullptr) {
        mpSpinLiftParentMdl->setFrame(getBodyAnm().getFrame());
    }
}

void dYoshiMdl_c::setBodyFrame(float frame) {
    dPyMdlBase_c::setBodyFrame(frame);
    if (mpSpinLiftParentMdl != nullptr) {
        mpSpinLiftParentMdl->setFrame(getBodyAnm().getFrame());
    }
}

void dYoshiMdl_c::setRate(float rate) {
    dPyMdlBase_c::setRate(rate);
    if (mpSpinLiftParentMdl != nullptr) {
        mpSpinLiftParentMdl->setRate(getBodyAnm().getRate());
    }
}

void dYoshiMdl_c::setBodyRate(float rate) {
    dPyMdlBase_c::setBodyRate(rate);
    if (mpSpinLiftParentMdl != nullptr) {
        mpSpinLiftParentMdl->setRate(getBodyAnm().getRate());
    }
}

void dYoshiMdl_c::getFootAnmTrans(mVec3_c *vec, const char *name) {
    int index = m_218.GetNodeAnmIndex(name);
    nw4r::g3d::ChrAnmResult result;
    m_218.GetAnmResult(&result, index, getFootAnm().getFrame());
    result.GetTranslate(vec);
}

const nw4r::g3d::ResFile *dYoshiMdl_c::getAnmResFile() const {
    if (m_164 & FLAG_0010_0000) {
        return &m_214;
    }
    return &m_210;
}

void dYoshiMdl_c::nodeCallback_c::timingA(ulong nodeId, nw4r::g3d::ChrAnmResult *anmRes, nw4r::g3d::ResMdl resMdl) {
    nw4r::g3d::ResNode resNode = resMdl.GetResNode(nodeId);

    if (!(mpOwner->mFlags & FLAG_0010_0000) && nodeId != 0) {
        mVec3_c v;
        if (nodeId == 1) {
            anmRes->GetTranslate(&v);
            v.y *= 0.98f;
            anmRes->SetTranslate(v);
        } else {
            v = resNode.GetTranslate();
            anmRes->SetTranslate(&v);
        }
    }
    if ((mpOwner->m_164 & FLAG_0000_0020) && nodeId == 1) {
        mVec3_c vec;
        mpOwner->getFootAnmTrans(&vec, "skl_root");
        anmRes->SetTranslate(vec);
    }
}

void dYoshiMdl_c::nodeCallback_c::timingB(ulong nodeId, nw4r::g3d::WorldMtxManip *manip, nw4r::g3d::ResMdl resMdl) {
    if (!mpOwner->m_208) {
        return;
    }

    nw4r::g3d::ResNode resNode = resMdl.GetResNode(nodeId);
    mMtx_c mtx;
    switch (nodeId) {
        case 15: {
            mVec3_c trans(0.0f, 0.0f, 0.0f);
            s16 rot = 0;
            if (mpOwner->m_3f4.x > 0.0f && !(mpOwner->m_164 & FLAG_0000_0020)) {
                rot = mpOwner->m_3f4.x * -5000.0f;
            }
            if (mpOwner->m_3f4.y > 0.0f) {
                s16 rotY = mpOwner->m_3f4.y * -5000.0f;
                if (rot > rotY) {
                    rot = rotY;
                }
                trans.set(3.0f, 0.0f, 0.0f);
                trans *= mpOwner->m_3f4.y;
            }
            manip->GetMatrix(&mtx);
            mtx.ZXYrotM(0, 0, rot);
            mtx.concat(mMtx_c::createTrans(trans));
            manip->SetMatrix(mtx);
            break;
        }
        case 17:
            manip->GetMatrix(&mtx);
            if (mpOwner->m_208 == 1) {
                mpOwner->setTongueJointMtx(nodeId, &mtx);
            }
            if (mpOwner->m_294 != 1) {
                mtx.concat(mMtx_c::createScale(mVec3_c(0.0f, 0.0f, 0.0f)));
                manip->SetMatrix(mtx);
                mpOwner->setTongueJointMtx(23, &mtx);
            }
            break;
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23: {
            if (mpOwner->m_294 == 1) {
                manip->GetMatrix(&mtx);
                if (mpOwner->m_208 == 1) {
                    mpOwner->setTongueJointMtx(nodeId, &mtx);
                } else {
                    mMtx_c &mtx = mpOwner->getTongueJointMtx(nodeId);
                    manip->SetMatrix(&mtx);
                }
            }
            break;
        }
    }
}

void dYoshiMdl_c::getJointMtx(mMtx_c *mtx, int jointIdx) {
    switch (jointIdx) {
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
            *mtx = getTongueJointMtx(jointIdx);
            break;
        default:
            getBodyMdl()->getNodeWorldMtx(jointIdx, mtx);
            break;
    }
}

mVec3_c dYoshiMdl_c::getTonguePos() {
    mVec3_c pos;
    getJointPos(&pos, 17);
    return pos;
}

mMtx_c dYoshiMdl_c::getTongueTipMtx() {
    mMtx_c mtx;
    getJointMtx(&mtx, 23);
    return mtx;
}

void dYoshiMdl_c::setTongueJointMtx(int jointIdx, const mMtx_c *mtx) {
    setTongueJointMtxBuf(jointIdx - 17, mtx);
}

mMtx_c &dYoshiMdl_c::getTongueJointMtx(int jointIdx) {
    return getTongueJointMtxBuf(jointIdx - 17);
}

void dYoshiMdl_c::setTongueJointMtxBuf(int jointIdx, const mMtx_c *mtx) {
    mTongueJointMtxs[jointIdx] = *mtx;
}

mMtx_c &dYoshiMdl_c::getTongueJointMtxBuf(int jointIdx) {
    return mTongueJointMtxs[jointIdx];
}

void dYoshiMdl_c::setTongueBendCalc() {
    if (m_294 == 1) {
        mMtx_c tongueMtxs[7];
        mVec3_c tongueJointPos[2];
        mVec3_c baseJointPos;
        getJointPos(&baseJointPos, 0);
        if (!m_2a0) {
            m_2a0 = 1;
            m_298.y = baseJointPos.y;
        }
        float diff = m_298.y - baseJointPos.y;
        if (diff >= 30.0f) {
            diff = 30.0f;
        }
        if (diff <= -30.0f) {
            diff = -30.0f;
        }
        float tongueLength = 0.0f;
        for (int i = 1; i < (int) ARRAY_SIZE(mTongueJointMtxs); i++) {
            getTongueJointMtxBuf(i - 1).multVecZero(tongueJointPos[0]);
            getTongueJointMtxBuf(i).multVecZero(tongueJointPos[1]);
            tongueLength += (tongueJointPos[1] - tongueJointPos[0]).Len();

            float factor = tongueLength / 10.0f;
            float y = diff * (factor * factor * factor) / 1000.0f;
            mMtx_c mtx = mMtx_c::createTrans(0.0f, y, 0.0f);
            mtx.concat(mMtx_c::createScale(0.0f, 0.0f, 0.0f));
            tongueMtxs[i] = mtx;
        }
        s16 angle;
        for (int i = 1; i < (int) ARRAY_SIZE(mTongueJointMtxs); i++) {
            mMtx_c mtx = getTongueJointMtxBuf(i);
            getTongueJointMtxBuf(i - 1).multVecZero(tongueJointPos[0]);
            mtx.multVecZero(tongueJointPos[1]);
            angle = -cLib::targetAngleX(tongueJointPos[0], tongueJointPos[1]);
            mtx.Add(tongueMtxs[i]);
            mtx.ZrotM(angle);
            setTongueJointMtxBuf(i, &mtx);
        }
    } else {
        m_2a0 = 0;
    }
}

void dYoshiMdl_c::updateTongueMtx() {
    mVec3_c tongueJointPos[2];
    mVec3_c pos = getTonguePos();
    for (int i = 1; i < (int) ARRAY_SIZE(mTongueJointMtxs); i++) {
        getTongueJointMtxBuf(i - 1).multVecZero(tongueJointPos[0]);
        getTongueJointMtxBuf(i).multVecZero(tongueJointPos[1]);
        if (std::fabs(pos.x - tongueJointPos[1].x) > m_298.x) {
            float v = m_298.x - std::fabs(pos.x - tongueJointPos[0].x);
            if (v < 0.0f) {
                v = 0.0f;
            }
            mVec3_c shift = tongueJointPos[1] - tongueJointPos[0];
            float scale = 0.0f;
            if (std::fabs(shift.x) > 0.1f) {
                scale = std::fabs(v / shift.x);
            }
            mMtx_c mtx = mMtx_c::createTrans(scale * shift);
            float factor = 0.0f;
            mtx.m[0][0] *= factor;
            mtx.m[0][1] *= factor;
            mtx.m[0][2] *= factor;
            mtx.m[1][0] *= factor;
            mtx.m[1][1] *= factor;
            mtx.m[1][2] *= factor;
            mtx.m[2][0] *= factor;
            mtx.m[2][1] *= factor;
            mtx.m[2][2] *= factor;
            mtx.Add(getTongueJointMtxBuf(i - 1));
            setTongueJointMtxBuf(i, &mtx);
        }
    }
}

bool dYoshiMdl_c::setTongueBgCheck() {
    if (m_294 != 1) {
        return false;
    }
    mVec3_c tongueJointPos[2];
    mVec3_c pos = getTonguePos();
    for (int i = 1; i < (int) ARRAY_SIZE(mTongueJointMtxs); i++) {
        getTongueJointMtxBuf(i - 1).multVecZero(tongueJointPos[0]);
        getTongueJointMtxBuf(i).multVecZero(tongueJointPos[1]);
        float wallX;
        if (std::fabs(pos.x - tongueJointPos[0].x) < m_298.x && dBc_c::checkWall(&tongueJointPos[0], &tongueJointPos[1], &wallX, 0, 1, nullptr)) {
            float offset = std::fabs(wallX - pos.x);
            if (offset < 4.0f) {
                offset = 4.0f;
                if (tongueJointPos[1].x < pos.x) {
                    wallX = pos.x - offset;
                } else {
                    wallX = offset + pos.x;
                }
            }
            if (m_298.x > offset) {
                m_298.x = offset;
                if (mpOwner != nullptr && mpOwner->mKind == dActor_c::STAGE_ACTOR_YOSHI) {
                    mVec3_c wallHitPos(wallX, tongueJointPos[0].y, tongueJointPos[0].z);
                    ((daYoshi_c *) mpOwner)->setTongueHitEffect(wallHitPos);
                }
            }
            break;
        }
    }
    updateTongueMtx();
    return false;
}

void dYoshiMdl_c::setTongueCcCheck(float f) {
    mVec3_c pos = getTonguePos();
    float diff = std::fabs(f - pos.x);
    if (m_298.x > diff) {
        m_298.x = diff;
    }
    updateTongueMtx();
}

void dYoshiMdl_c::setFunbariRate(float f) {
    if (f > 1.0f) {
        f = 1.0f;
    }
    if (f < 0.0f) {
        f = 0.0f;
    }
    m_3f4.x = f;
    m_17c |= 0x20;
}
