#include <game/bases/d_wm_enemy.hpp>
#include <game/bases/d_wm_lib.hpp>
#include <game/bases/d_wm_se_manager.hpp>
#include <game/bases/d_wm_effect_manager.hpp>
#include <game/bases/d_a_wm_map.hpp>
#include <game/bases/d_a_wm_player.hpp>
#include <game/bases/d_s_world_map_static.hpp>
#include <game/bases/d_cs_seq_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_info.hpp>

const char *dWmEnemy_c::smc_PathPointA[] = {
    "Fa00", "Fa01", "Fa02", "Fa03", "Fa04",
    "Fa05", "Fa06", "Fa07", "Fa08"
};
const char *dWmEnemy_c::smc_PathPointB[] = {
    "Fb00", "Fb01", "Fb02", "Fb03", "Fb04",
    "Fb05", "Fb06", "Fb07", "Fb08"
};
const char *dWmEnemy_c::smc_PathPointC[] = {
    "Fc00", "Fc01", "Fc02", "Fc03", "Fc04",
    "Fc05", "Fc06", "Fc07", "Fc08"
};

namespace dWmEnemy {

const ProfileName sc_EnemyProfName[] = {
    fProfile::WM_KURIBO,
    fProfile::WM_BROS,
    fProfile::WM_PUKU,
    fProfile::WM_PAKKUN,
    fProfile::WM_JUGEM
};

bool isEnemyWalk() {
    int courseType = dWmLib::GetCourseTypeFromCourseNo(dInfo_c::m_instance->m_54);
    if (dScWMap_c::m_WorldNo == dScWMap_c::m_PrevWorldNo &&
        dScWMap_c::m_SceneNo == dScWMap_c::m_PrevSceneNo &&
        dWmLib::GetCurrentPlayResultStatus() != 0 &&
        dWmLib::GetCurrentPlayResultStatus() != 9 &&
        courseType != dWmLib::COURSE_TYPE_KINOKO_START &&
        courseType != dWmLib::COURSE_TYPE_PEACH_CASTLE &&
        courseType != dWmLib::COURSE_TYPE_INVALID &&
        courseType != dWmLib::COURSE_TYPE_KINOKO &&
        courseType != dWmLib::COURSE_TYPE_CANNON
    ) {
        return true;
    }
    return false;
}

} // namespace dWmEnemy

dWmEnemy_c::~dWmEnemy_c() {
    if (mpBgmSync != nullptr) {
        delete mpBgmSync;
    }
}

void dWmEnemy_c::initializeBase(const char **names, int count, bool cyclic) {
    daWmMap_c *wmMap = daWmMap_c::m_instance;
    dInfo_c::enemy_s enData;
    dInfo_c::m_instance->GetMapEnemyInfo(dScWMap_c::m_WorldNo, mParam & 0xf, enData);
    dWmConnect_c *connect = &wmMap->mWmConnect[wmMap->currIdx];
    if (count < 0) {
        count = wmMap->GetNodeCount(mParam & 0xf);
    }
    mPath.init(names, count, connect, cyclic, enData.m_08);
    mPath.SetStartPoint(getStartPoint());
    dWmConnect_c::Point_s *point = connect->GetPointFromIndex(mPath.mpCurrentPoint->mPointIndex);
    mPos = point->pos + getPointOffset(mPath.mpCurrentPoint->mIndex);
    bool enWalk = false;
    if (dWmEnemy::isEnemyWalk() && dWmLib::getEnemyRevivalCount(dScWMap_c::m_WorldNo, mParam & 0xf) == 0) {
        enWalk = true;
    }
    mEnWalk = enWalk;
    initShapeAngle();
    dWmBgmSync_c *bgmSync = new dWmBgmSync_c();
    mpBgmSync = bgmSync;
    bgmSync->mAngle = &mpUnkData->mAngle2;
    bgmSync->m_04 = bgmSync->mAngle->x.mAngle - 1;
    bgmSync->m_08 = bgmSync->mAngle->y.mAngle;
    mSeID = -1;
    mEfID = -1;
    mNotAnger = true;
    float scale = mpUnkData->m_04;
    mScale.x = scale;
    mScale.y = scale;
    mScale.z = scale;
    mArrivedAtTarget = true;
    mHitPlayer = false;
}

int dWmEnemy_c::getStartPoint() {
    return (mParam >> 4) & 0xf;
}

mVec3_c dWmEnemy_c::getPointOffset(int index) {
    return mVec3_c::Zero;
}

int dWmEnemy_c::execute() {
    if (mpBgmSync != nullptr) {
        mpBgmSync->execute();
        if (mpBgmSync->m_0c) {
            updateBgmAnimRate();
        }
    }
    setCutEndSpecific(dCsSeqMng_c::ms_instance->GetCutName(), dCsSeqMng_c::ms_instance->m_164);
    if (IsExecEnable() || dCsSeqMng_c::ms_instance->GetCutName() == 0x57) {
        static const ProcFunc ProcTbl[] = {
            mode_exec,
            mode_DemoContinue,
            mode_bgmDance,
            mode_lose,
            mode_waitWalk
        };
        (this->*ProcTbl[mCurrProc])();
    }
    calculateEffect();
    calc();
    return SUCCEEDED;
}

void dWmEnemy_c::updateBgmAnimRate() {}

void dWmEnemy_c::calculateEffect() {}

void dWmEnemy_c::calc() {}

void dWmEnemy_c::init_exec() {
    mCurrProc = PROC_TYPE_EXEC;
}

void dWmEnemy_c::mode_exec() {
    if (IsExecEnable() && mNotAnger && mpBgmSync->m_0d) {
        init_bgmDance();
    } else {
        if (IsNeedChasePlayer()) {
            init_waitWalk();
        }
    }
}

void dWmEnemy_c::init_lose() {
    initDemoStarLose();
    mCurrProc = PROC_TYPE_LOSE;
}

void dWmEnemy_c::initDemoStarLose() {
    initDemoLose();
}

void dWmEnemy_c::mode_lose() {
    if (procDemoStarLose()) {
        mVisible = false;
    }
}

bool dWmEnemy_c::procDemoStarLose() {
    return procDemoLose();
}

void dWmEnemy_c::init_bgmDance() {
    mCurrProc = PROC_TYPE_BGM_DANCE;
    mBgmDanceRelated = 1;
    initDemoBgmDance();
}

void dWmEnemy_c::initDemoBgmDance() {}

void dWmEnemy_c::mode_bgmDance() {
    if (procDemoBgmDance()) {
        init_exec();
    }
}

bool dWmEnemy_c::procDemoBgmDance() {
    return true;
}

void dWmEnemy_c::init_DemoContinue() {
    mCurrProc = PROC_TYPE_DEMO_CONTINUE;
    mDemoContinueRelated = 2;
    mWalkWaitTimer = 10;
}

void dWmEnemy_c::mode_DemoContinue() {
    procDemoAnger();
}

void dWmEnemy_c::init_waitWalk() {
    mCurrProc = PROC_TYPE_WAIT_WALK;
    mWalkWaitTimer = GetWalkWaitFrame() + 1;
}

void dWmEnemy_c::mode_waitWalk() {
    if (!mHitPlayer) {
        if (mWalkWaitTimer > 0) {
            if (mWalkWaitTimer == 1) {
                mPath.mAdvancePoint = IsRandomMove();
                int dir;
                bool randomWalk = IsRandomWalk();
                if (randomWalk) {
                    mPath.mDir2 = mPath.mDir1;
                }
                if (randomWalk) {
                    dir = mPath.mDir1 == 0;
                } else {
                    dir = mPath.mDir1;
                }
                mPath.mDir1 = dir;
                initWalk();
            }
            mWalkWaitTimer--;
        } else {
            if (doWalk() && mCurrProc == PROC_TYPE_WAIT_WALK) {
                if (!mHitPlayer) {
                    PostWaitWalk();
                    mPath.mAdvancePoint = true;
                    mPath.mDir1 = mPath.mDir2;
                }
                if (!daWmPlayer_c::checkWalkPlayers()) {
                    init_exec();
                }
            }
        }
    }
}

void dWmEnemy_c::initWalk(float f) {
    mNextPoint = getNextPointInfo();
    mPrevPoint = mPos;
    mSpeedF = f;
    mAccelY = -5.0f;
    mMaxFallSpeed = -10.0f;
    mArrivedAtTarget = false;
    mDemoAngle.y = (getNextPointInfo() - mPos).xzAng();
    if (isNextThroughPoint()) {
        mNextPointType = 1;
    } else {
        mNextPointType = 0;
    }
}

bool dWmEnemy_c::doWalk() {
    if (!mArrivedAtTarget) {
        if (mDemoAngle.y != mAngle3D.y) {
            sLib::addCalcAngle(&mAngle3D.y.mAngle, mDemoAngle.y, 10, 0x2000, 0x400);
            mAngle = mAngle3D;
        } else {
            if (mSeID < 0) {
                dWmSeManager_c *seManager = dWmSeManager_c::m_pInstance;
                mSeID = seManager->playSound(getEnemyWalkSeID(), 0, mPos, (mParam & 0xf) + 1, 1);
            }
            setWalkAnm(getWalkAnmRate());
            calcSpeed();
            posMove();
            adjustHeightBase(mPrevPoint, mNextPoint, -1);
            bool hitToPlayer = false;
            if (CheckIsHitToPlayer()) {
                hitToPlayer = true;
            }
            if (!mArrivedAtTarget && checkArriveTargetXYZ(mPrevPoint, mNextPoint)) {
                bool tmp = false;
                if (mNextPointType == 1) {
                    tmp = true;
                }
                mPos = mNextPoint;
                mSpeedF = 0.0f;
                mArrivedAtTarget = true;
                updatePathInfo(!tmp);
            }
            if (hitToPlayer) {
                return true;
            }
        }
    }
    if (mArrivedAtTarget) {
        setWalkAnm(1.0f);
        if (mNextPointType == 0) {
            deleteSound();
            mDemoAngle.y = getWaitAngle();
            sLib::addCalcAngle(&mAngle3D.y.mAngle, mDemoAngle.y, 10, 0x2000, 0x400);
        } else {
            mAngle3D.y = mDemoAngle.y;
        }
        mAngle = mAngle3D;
        if (mDemoAngle.y == mAngle3D.y) {
            if (mNextPointType == 1) {
                initWalk();
                return false;
            }
            return true;
        }
    }
    return false;
}

mVec3_c dWmEnemy_c::getNextPointInfo() {
    daWmMap_c *wmMap = daWmMap_c::m_instance;
    dWmConnect_c *connect = &wmMap->mWmConnect[wmMap->currIdx];
    dWmConnect_c::Point_s *point = connect->GetPointFromIndex(mPath.GetNextPointIdx());
    int idx = mPath.GetNextPointInfo(false)->mIndex;
    return point->pos + getPointOffset(idx);
}

mVec3_c dWmEnemy_c::getCurrentPointInfo() {
    daWmMap_c *wmMap = daWmMap_c::m_instance;
    dWmConnect_c *connect = &wmMap->mWmConnect[wmMap->currIdx];
    dWmConnect_c::Point_s *point = connect->GetPointFromIndex(mPath.mpCurrentPoint->mPointIndex);
    return point->pos + getPointOffset(mPath.mpCurrentPoint->mIndex);
}

void dWmEnemy_c::initShapeAngle() {
    short angle = getWaitAngle();
    mAngle.y = angle;
    mAngle3D.y = angle;
}

bool dWmEnemy_c::csCommand(int id, bool b) {
    dWmDemoActor_c *actor = dCsSeqMng_c::ms_instance->m_1ac;
    bool isEnd = false;
    if (id == -1) {
        return false;
    }
    if (b) {
        switch (id) {
            case 4:
                if (mEnWalk) {
                    initWalk();
                }
                break;
            case 86:
                if (actor == this) {
                    initDemoAnger();
                }
                break;
            case 89:
                if (FUN_800f88d0()) {
                    initDemoLose();
                }
                break;
            case 91:
                mRotateTimer = 5;
                break;
        }
    }
    switch (id) {
        case 4:
            if (mEnWalk) {
                isEnd = true;
                if (doWalk()) {
                    isEnd = false;
                }
            } else {
                mIsCutEnd = true;
            }
            break;
        case 86:
            if (actor == this) {
                isEnd = !procDemoAnger();
            } else {
                isEnd = false;
            }
            break;
        case 89:
            if (FUN_800f88d0()) {
                isEnd = !procDemoLose();
            } else {
                isEnd = false;
            }
            break;
        case 91:
            if (mHitPlayer && dScWMap_c::m_WorldNo != WORLD_4) {
                isEnd = true;
                if (mRotateTimer > 0) {
                    doWalk();
                    mRotateTimer--;
                }
                if (mArrivedAtTarget || mRotateTimer == 0) {
                    if (!mArrivedAtTarget) {
                        updatePathInfo(true);
                    }
                    setWalkAnm(1.0f);
                    deleteSound();
                    PostWaitWalk();
                    isEnd = false;
                    mPath.mAdvancePoint = true;
                    mPath.mDir1 = mPath.mDir2;
                    mRotateTimer--;
                }
            }
            break;
    }
    return isEnd;
}

void dWmEnemy_c::initDemoAnger() {
    mVec3_c playerPos = daWmPlayer_c::ms_instance->mPos;
    mDemoAngle.y = (playerPos - mPos).xzAng();
    mNotAnger = false;
    mRotateTimer = 10;
}

bool dWmEnemy_c::procDemoAnger() {
    bool res = false;
    if (mCurrProc != PROC_TYPE_BGM_DANCE) {
        if (mRotateTimer > 0) {
            mRotateTimer--;
        } else {
            sLib::addCalcAngle(&mAngle3D.y.mAngle, mDemoAngle.y, 100, 0x4000, 0x800);
            mAngle = mAngle3D;
            if (mDemoAngle.y == mAngle3D.y) {
                res = true;
            }
        }
    }
    return res;
}

void dWmEnemy_c::initDemoLose() {}

void dWmEnemy_c::demoJump(const mVec3_c &pos, JumpData_s &jumpData, float scale) {
    _initDemoJumpBase(pos, 0,
        jumpData.mFrames, jumpData.mJumpSpeed,
        jumpData.mStartScale * scale, jumpData.mEndScale * scale,
        mVec3_c::Ey
    );
}

bool dWmEnemy_c::procDemoLose() {
    return true;
}

void dWmEnemy_c::procDemoLoseEnd() {
    mVisible = false;
    dInfo_c::enemy_s enData;
    dInfo_c::m_instance->GetMapEnemyInfo(dScWMap_c::m_WorldNo, mParam & 0xf, enData);
    enData.m_04 = -1;
}

bool dWmEnemy_c::procDemoLoseBase(short angle) {
    bool res = false;
    rotDirectionX(angle, false);
    if (_procDemoJumpBase()) {
        dWmEffectManager_c::m_pInstance->playEffect(dWmEffectManager_c::EFFECT_ENEMY_DEATH, &mPos, nullptr, nullptr);
        dWmSeManager_c::m_pInstance->playSound(0x56, mPos, 1);
        mPos.y -= 10000.0f;
        res = true;
        procDemoLoseEnd();
    }
    return res;
}

bool dWmEnemy_c::isNextThroughPoint() {
    return isThroughPoint(mPath.GetNextPointIdx());
}

bool dWmEnemy_c::isThroughPoint(int idx) {
    daWmMap_c *wmMap = daWmMap_c::m_instance;
    return wmMap->mCsvData[wmMap->currIdx].GetPointName(idx)[0] == 'K';
}

bool dWmEnemy_c::FUN_800f88d0() {
    return dInfo_c::m_instance->m_9c == (mParam & 0xf);
}

bool dWmEnemy_c::isDead() {
    return dWmLib::getEnemyRevivalCount(dScWMap_c::m_WorldNo, mParam & 0xf) != 0;
}

void dWmEnemy_c::ModelCalc(m3d::mdl_c *mdl, float yOffset, float shadowYOffset, float shadowScale) {
    mdl->play();
    mVec3_c selfPos = mPos;
    mAng3_c ang = mAngle;
    selfPos.y += yOffset;
    mMatrix.trans(selfPos).ZXYrotM(ang);
    mMatrix.concat(mMtx_c::createTrans(mVec3_c(0.0f, -yOffset, 0.0f)));
    mdl->setLocalMtx(&mMatrix);
    mdl->setScale(mScale);
    mdl->calc(false);
    CalcShadow(shadowYOffset, shadowScale);
}

void dWmEnemy_c::updatePathInfo(bool b) {
    dInfo_c *info = dInfo_c::m_instance;
    mPath.UpdatePoint();
    mDemoAngle.y = (getNextPointInfo() - mPos).xzAng();
    if (b) {
        int wNo = dScWMap_c::m_WorldNo;
        int sNo = dScWMap_c::m_SceneNo;
        int param = mParam & 0xf;
        int v = vfc4(mPath.GetPathPointNo(mPath.mpCurrentPoint->mpName));
        info->SetMapEnemyInfo(wNo, param, sNo, v);
        info->FUN_800bbc40(wNo, param, mPath.mDir1);
    }
}

void dWmEnemy_c::deleteSound() {
    if (mSeID < 0) {
        return;
    }
    dWmSeManager_c::m_pInstance->endSound(mSeID);
    mSeID = -1;
}

void dWmEnemy_c::deleteEffect() {
    if (mEfID < 0) {
        return;
    }
    dWmEffectManager_c::m_pInstance->endEffect(mEfID);
    mEfID = -1;
}

int dWmEnemy_c::getEnemyWalkSeID() {
    switch (dScWMap_c::m_WorldNo) {
        case WORLD_1:
            return 87;
        case WORLD_3:
            return 89;
        case WORLD_5:
            return 91;
        case WORLD_7:
            return 93;
        default:
            return 121;
    }
}

bool dWmEnemy_c::IsExecEnable() {
    static const int cutsceneIDs[] = {
        -1, 56, 136, 86, 87
    };
    return isCutscenePlaying(cutsceneIDs, ARRAY_SIZE(cutsceneIDs));
}

bool dWmEnemy_c::IsPlayerComingCore() {
    int point = daWmPlayer_c::ms_instance->m_234;
    if (point >= 0 && mPath.isPointIn(point) && !isThroughPoint(point)) {
        if (isDead()) {
            return true;
        }
        if (GetIndex() != point) {
            return true;
        }
    }
    return false;
}

bool dWmEnemy_c::IsNeedChasePlayer() {
    bool res = false;
    if (!mHitPlayer && !isDead() && IsPlayerComing()) {
        res = true;
    }
    return res;
}

bool dWmEnemy_c::IsRandomWalk() {
    int count = dGameCom::rndInt(100);
    int randomNumber = 0;
    while (count >= 0) {
        randomNumber = dGameCom::rndInt(100);
        count--;
    }
    bool randomWalk = false;
    if (mCurrProc == PROC_TYPE_WAIT_WALK && randomNumber <= GetChangeDirRate()) {
        randomWalk = true;
    }
    return randomWalk;
}

bool dWmEnemy_c::IsHitToWaitPlayer() {
    bool res = false;
    if (!mHitPlayer) {
        bool b1 = false;
        bool b2 = false;
        int nextPointIdx = daWmPlayer_c::ms_instance->m_22c;
        if (mPath.GetNextPointIdx() == nextPointIdx && !daWmPlayer_c::ms_instance->m_18c) {
            b2 = true;
        }
        if (b2) {
            if (mPos.distTo(daWmPlayer_c::ms_instance->mPos) < GetTerritory()) {
                b1 = true;
            }
        }
        if (b1) {
            res = true;
        }
    }
    return res;
}

bool dWmEnemy_c::IsHitToMovePlayer() {
    bool res = false;
    if (!mHitPlayer) {
        bool b1 = false;
        bool b2 = false;
        int nextPointIdx = daWmPlayer_c::ms_instance->m_230;
        if (mPath.GetNextPointIdx() == nextPointIdx && daWmPlayer_c::ms_instance->m_18c) {
            b2 = true;
        }
        if (b2) {
            if (mPos.distTo(daWmPlayer_c::ms_instance->mPos) < GetTerritory() * 2.0f) {
                b1 = true;
            }
        }
        if (b1) {
            res = true;
        }
    }
    return res;
}

bool dWmEnemy_c::CheckIsHitToPlayer() {
    bool hitWait = IsHitToWaitPlayer();
    bool hitMove = IsHitToMovePlayer();
    if (hitWait || hitMove) {
        mHitPlayer = true;
        if (daWmPlayer_c::isPlayerStarMode()) {
            daWmPlayer_c::ms_instance->setEnemyDieByStar(this);
            deleteSound();
            return true;
        }
    }
    if (hitWait) {
        daWmPlayer_c::ms_instance->m_2fc = true;
        dCsSeqMng_c::ms_instance->FUN_801017c0(0x29, this, 0, 0x80);
        deleteSound();
        return true;
    } else {
        if (hitMove) {
            dCsSeqMng_c::ms_instance->FUN_801017c0(0x29, this, 0, 0x80);
            deleteSound();
            return true;
        } else {
            return false;
        }
    }
    return false;
}

bool dWmEnemy_c::isAllEnemyMoveEnd(void) {
    dWmEnemy_c *en;
    bool res = true;
    for (int i = 0; i < ARRAY_SIZE(dWmEnemy::sc_EnemyProfName); i++) {
        en = (dWmEnemy_c *) fManager_c::searchBaseByProfName(dWmEnemy::sc_EnemyProfName[i], nullptr);
        if (en != nullptr) {
            while (en != nullptr) {
                res &= en->isWaitWalkEnd();
                en = (dWmEnemy_c *) fManager_c::searchBaseByProfName(dWmEnemy::sc_EnemyProfName[i], en);
            }
            break;
        }
    }
    return res;
}
