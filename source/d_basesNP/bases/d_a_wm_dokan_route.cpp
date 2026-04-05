#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_a_wm_dokan_route.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_wm_se_manager.hpp>

daWmDokanRoute_c::Settings_t daWmDokanRoute_c::sSettings[6] = {
    {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 60, 98, 30, 0},
    {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 30, 75, 15, 3},
    {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 90, 160, 15, 1},
    {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 120, 155, 15, 3},
    {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 110, 150, 15, 3},
    {mVec3_c(0.0f, 0.0f, 0.0f), 1.0f, 155, 175, 15, 3},
};

// I thought it was a bool, apparently not
u8 daWmDokanRoute_c::sIsNormalExit[6] = {
    true,
    true,
    true,
    true,
    false,
    true,
};

const char *daWmDokanRoute_c::sNodeNames[6] = {
    "W601",
    "W603",
    "W603",
    "W605",
    "W605",
    "W605"
};

ACTOR_PROFILE(WM_DOKAN, daWmDokanRoute_c, 0);

daWmDokanRoute_c::daWmDokanRoute_c() {}
daWmDokanRoute_c::~daWmDokanRoute_c() {}

int daWmDokanRoute_c::create() {
    mPosCopy = mPos;
    createModel();

    mClipSphere.mCenter = mPos;
    mClipSphere.mRadius = 250.0f;

    FUN_808d0270();
    calcModel(mModel);
    return SUCCEEDED;
}

int daWmDokanRoute_c::execute() {
    setCutEndSpecific(dCsSeqMng_c::ms_instance->GetCutName(), dCsSeqMng_c::ms_instance->m_164);

    // Could be a stubbed debug function
    static const ProcFunc Proc_tbl[PROC_STATE_COUNT] = {
        &daWmDokanRoute_c::FUN_808d02d0
    };

    (this->*Proc_tbl[mProcState])();
    FUN_808d0360();
    FUN_808d01a0();
    calcModel(mModel);
    return SUCCEEDED;
}

int daWmDokanRoute_c::draw() {
    mModel.entry();
    return SUCCEEDED;
}

int daWmDokanRoute_c::doDelete() {
    return SUCCEEDED;
}

void daWmDokanRoute_c::createModel() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);
    mResFile = dResMng_c::m_instance->getRes("cobDokanRoute", "g3d/model.brres");

    nw4r::g3d::ResMdl resMdl = mResFile.GetResMdl("cobDokanRoute");
    mModel.create(resMdl, &mAllocator, nw4r::g3d::ScnMdl::ANM_TEXSRT | nw4r::g3d::ScnMdl::BUFFER_RESMATMISC, 1, nullptr);

    static m3d::playMode_e playModes[1] = {
        m3d::FORWARD_ONCE
    };

    static const char * resAnmChrNames[1] = {
        "cobDokanRoute"
    };

    for (int i = 0; i < 1; i++) {
        nw4r::g3d::ResAnmChr resAnmChr = mResFile.GetResAnmChr(resAnmChrNames[i]);
        mChrAnim[i].create(resMdl, resAnmChr, &mAllocator, nullptr);
        mChrAnim[i].mPlayMode = playModes[i];
        mChrAnim[i].setRate(0.0f);
        mChrAnim[i].setFrame(0.0f);
    }

    static const char * resAnmSrtNames[1] = {
        "cobDokanRoute"
    };

    for (int i = 0; i < 1; i++) {
        nw4r::g3d::ResAnmTexSrt resAnmTexSrt = mResFile.GetResAnmTexSrt(resAnmSrtNames[i]);
        mSrtAnim[i].create(resMdl, resAnmTexSrt, &mAllocator, nullptr, 1);
        mSrtAnim[i].setPlayMode(playModes[i], 0);
        mSrtAnim[i].setRate(0.0f, 0);
        mSrtAnim[i].setFrame(0.0f, 0);
    }

    mModel.setAnm(mChrAnim[0]);
    mModel.setAnm(mSrtAnim[0]);
    dWmActor_c::setSoftLight_Map(mModel);
    mAllocator.adjustFrmHeap();
}

void daWmDokanRoute_c::FUN_808d01a0() {
    mModel.play();
}

void daWmDokanRoute_c::calcModel(m3d::mdl_c &model) {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    mMatrix.trans(pos);
    mMatrix.ZXYrotM(angle);
    model.setLocalMtx(&mMatrix);
    model.setScale(mScale);
    model.calc(false);
}

void daWmDokanRoute_c::FUN_808d0270() {
    FUN_808d0360();
    FUN_808d0520();
    mScale.set(1.0f, 1.0f, 1.0f);
    FUN_808d02c0();
}

void daWmDokanRoute_c::FUN_808d02c0() {
    mProcState = PROC_STATE_DEFAULT;
}

void daWmDokanRoute_c::FUN_808d02d0() {}

void daWmDokanRoute_c::setCutEndSpecific(int cutsceneCommandId, bool param2) {
    if (cutsceneCommandId == dCsSeqMng_c::CUTSCENE_CMD_NONE) {
        return;
    }

    if (param2) {
        switch (cutsceneCommandId) {
            case dCsSeqMng_c::CUTSCENE_CMD_2:
                FUN_808d0660();
                break;
            default:
                setCutEnd();
                break;
        }
    } else {
        switch (cutsceneCommandId) {
            case dCsSeqMng_c::CUTSCENE_CMD_2:
                FUN_808d0740();
                break;
            default:
                setCutEnd();
                break;
        }
    }
}

void daWmDokanRoute_c::FUN_808d0360() {
    u32 nodeType = ACTOR_PARAM(Node);
    mPos = mPosCopy + sSettings[nodeType].mPosDelta;
    s16 pipeDir = sSettings[nodeType].mDirection;

    float frame = 0.0f;
    mVec3_c rot = mVec3_c::Ez;

    switch (pipeDir) {
        case 0:
            frame = 2.0f;
            rot = mVec3_c::Ez;
            break;

        case 1:
            rot = -mVec3_c::Ez;
            frame = 1.0f;
            break;

        case 2:
            rot = mVec3_c::Ex;
            frame = 0.0f;
            break;

        case 3:
            rot = -mVec3_c::Ex;
            frame = 0.0f;
            break;
        
        default:
            break;
    }

    s16 angle = rot.xzAng();
    mAngle.y = angle;
    mAngle3D.y = angle;

    mSrtAnim[0].setFrame(frame, 0);
}

void daWmDokanRoute_c::FUN_808d0520() {
    u32 nodeType = ACTOR_PARAM(Node);
    int courseNo = dWmLib::GetCourseNoFromPointName(sNodeNames[nodeType]);
    dInfo_c *info = dInfo_c::getInstance();
    float frame = 0.0f;

    switch (sIsNormalExit[nodeType]) {
        case 1:
            if (dWmLib::IsCourseOmoteClear(dScWMap_c::m_WorldNo, courseNo) &&
                !dWmLib::IsCourseFirstOmoteClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)
            ) {
                frame = GetFrame();
            }
        break;

        case 0:
            if (dWmLib::IsCourseUraClear(dScWMap_c::m_WorldNo, courseNo) &&
                !dWmLib::IsCourseFirstUraClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)
            ) {
                frame = GetFrame();
            }
        
            break;

        default:
            break;
    }

    mChrAnim[0].setFrame(frame);
    mChrAnim[0].setRate(0.0f);
}

void daWmDokanRoute_c::FUN_808d0660() {
    u32 nodeType = ACTOR_PARAM(Node);
    int courseNo = dWmLib::GetCourseNoFromPointName(sNodeNames[nodeType]);
    dInfo_c *info = dInfo_c::getInstance();
    int exitType = sIsNormalExit[nodeType];
    mState = STATE_0;

    if (exitType == 1 && dWmLib::IsCourseFirstOmoteClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode) ||
        exitType == 0 && dWmLib::IsCourseFirstUraClear(dScWMap_c::m_WorldNo, courseNo, info->mCurrentCourseNode)) {
        mTimer = sSettings[nodeType].mTimer;
        mState = STATE_1;
    }
}

void daWmDokanRoute_c::FUN_808d0740() {
    u32 nodeType = ACTOR_PARAM(Node);

    switch (mState) {
        case STATE_0:
            setCutEnd();
            break;
        
        case STATE_1:
            if (mTimer > 0) {
                mTimer--;
            } else {
                mState = STATE_2;
            }
            break;
        
        case STATE_2:
            dWmSeManager_c::m_pInstance->playSound(0x2D, mPos, 1);
            mChrAnim[0].setRate(sSettings[nodeType].mAnmRate);
            mTimer = sSettings[nodeType].mTimer2 - sSettings[nodeType].mTimer;
            mState = STATE_3;
            break;

        case STATE_3:
            if (mTimer > 0) {
                mTimer--;
                if (mTimer <= sSettings[nodeType].mTimerThreshold) {
                    float threshold = sSettings[nodeType].mTimerThreshold;
                    float timer = mTimer;
                    float rate = sSettings[nodeType].mAnmRate;
                    mChrAnim[0].setRate((1.0f - threshold - timer) / threshold * rate);
                }
            } else {
                mState = STATE_4;
            }
            break;
        
        case STATE_4:
            mChrAnim[0].setRate(0.0f);
            setCutEnd();
            break;

        default:
            break;
    }
}

// I don't even want to know what is going on here...
// Looks like an unrolled loop of sorts
float daWmDokanRoute_c::GetFrame() {
  bool bVar1;
  float fVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  int iVar8;
  int iVar9;
  int iVar10;
  int iVar11;
  u32 uVar12;
  float anmRate;

  u32 nodeType = ACTOR_PARAM(Node);
  s16 threshold = sSettings[nodeType].mTimerThreshold;
  s16 timer = sSettings[nodeType].mTimer;
  s16 timer2 = sSettings[nodeType].mTimer2;
  iVar3 = 0;
  float frame = ((float)(timer2 - timer) - (float)threshold) * sSettings[nodeType].mAnmRate;

  if (threshold > 0) {
    if (threshold > 8) {
      bVar1 = false;
      if ((threshold > -1) && (threshold < 0x7fffffff)) {
        bVar1 = true;
      }
      if (bVar1) {
        uVar12 = threshold - 1U >> 3;
        iVar10 = (int)sSettings[nodeType].mTimerThreshold;
        anmRate = sSettings[nodeType].mAnmRate;
        if (threshold - 8U > 0) {
          do {
            iVar4 = iVar3 + 1;
            iVar11 = iVar3 + 2;
            iVar9 = iVar3 + 3;
            iVar8 = iVar3 + 4;
            fVar2 = (float)iVar3;
            iVar7 = iVar3 + 5;
            iVar6 = iVar3 + 6;
            iVar5 = iVar3 + 7;
            iVar3 += 8;
            frame = frame + (1.0 - fVar2 / (float)iVar10) * anmRate +
                    (1.0 - (float)iVar4 / (float)iVar10) * anmRate +
                    (1.0 - (float)iVar11 / (float)iVar10) * anmRate +
                    (1.0 - (float)iVar9 / (float)iVar10) * anmRate +
                    (1.0 - (float)iVar8 / (float)iVar10) * anmRate +
                    (1.0 - (float)iVar7 / (float)iVar10) * anmRate +
                    (1.0 - (float)iVar6 / (float)iVar10) * anmRate +
                    (1.0 - (float)iVar5 / (float)iVar10) * anmRate;
            uVar12--;
          } while (uVar12 != 0);
        }
      }
    }
    iVar4 = (int)sSettings[nodeType].mTimerThreshold;
    iVar10 = iVar4 - iVar3;
    if (iVar3 < iVar4) {
      do {
        frame +=
            (1.0 - (float)iVar3 / (float)iVar4) * sSettings[nodeType].mAnmRate;
        iVar10--;
        iVar3 += 1;
      } while (iVar10 != 0);
    }
  }
  return frame;
}
