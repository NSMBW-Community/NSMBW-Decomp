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
        courseType != 9 && courseType != 10 && courseType != 11 && courseType != 4 && courseType != 6
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

void dWmEnemy_c::initializeBase(const char **names, int count, bool circular) {
    daWmMap_c *wmMap = daWmMap_c::m_instance;
    dInfo_c::enemy_s enData;
    dInfo_c::m_instance->GetMapEnemyInfo(dScWMap_c::m_WorldNo, mParam & 0xf, enData);
    if (count < 0) {

    }
    dWmConnect_c *connect;
    mPath.init(names, count, connect, circular, enData.mRandomMove);
    mPath.SetStartPoint(getStartPoint());
    dWmConnect_c::Point_s *point = connect->GetPointFromIndex(mPath.mpCurrentPoint->mPointIndex);
    mPos = point->pos + getPointOffset(mPath.mpCurrentPoint->mIndex);
    bool enWalk = false;
    if (dWmEnemy::isEnemyWalk() && dWmLib::getEnemyRevivalCount(dScWMap_c::m_WorldNo, mParam & 0xf) == 0) {
        enWalk = true;
    }
    mEnWalk = enWalk;
    initShapeAngle();
    mpBgmSync = new dWmBgmSync_c();
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
        (this->*ProcTbl[m_6c4])();
    }
    calculateEffect();
    calc();
    return SUCCEEDED;
}

void dWmEnemy_c::updateBgmAnimRate() {}

void dWmEnemy_c::calculateEffect() {}

void dWmEnemy_c::calc() {}

void dWmEnemy_c::init_exec() {
    m_6c4 = 0;
}

void dWmEnemy_c::mode_exec() {
    if (IsExecEnable() && m_6d8 && mpBgmSync->m_0d) {
        init_bgmDance();
    } else {
        if (IsNeedChasePlayer()) {
            init_waitWalk();
        }
    }
}

void dWmEnemy_c::init_lose() {
    initDemoStarLose();
    m_6c4 = 3;
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
    m_6c4 = 2;
    m_6d4 = 1;
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
    m_6c4 = 1;
    m_6cc = 2;
    m_6c8 = 10;
}

void dWmEnemy_c::mode_DemoContinue() {
    procDemoAnger();
}

void dWmEnemy_c::init_waitWalk() {
    m_6c4 = 4;
    m_6c8 = GetWalkWaitFrame() + 1;
}

void dWmEnemy_c::mode_waitWalk() {
    if (!m_6e4) {
        if (m_6c8 > 0) {
            if (m_6c8 == 1) {
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
            m_6c8--;
        } else {
            if (doWalk() && m_6c4 == 4) {
                if (!m_6e4) {
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
    m_698 = getNextPointInfo();
    m_6a4 = mPos;
    mSpeedF = f;
    mAccelY = -5.0f;
    mMaxFallSpeed = -10.0f;
    m_6b6 = false;
    m_6b2 = (getNextPointInfo() - mPos).xzAng();
    if (isNextThroughPoint()) {
        m_6b8 = 1;
    } else {
        m_6b8 = 0;
    }
}

bool dWmEnemy_c::doWalk() {
    if (!m_6b6) {
        if (m_6b2 != mAngle3D.y) {
            sLib::addCalcAngle(&mAngle3D.y.mAngle, m_6b2, 10, 0x2000, 0x400);
            mAngle = mAngle3D;
        } else {
            if (m_6dc < 0) {
                dWmSeManager_c *seManager = dWmSeManager_c::m_pInstance;
                m_6dc = seManager->playSound(getEnemyWalkSeID(), 0, mPos, (mParam & 0xf) + 1, 1);
            }
            setWalkAnm(getWalkAnmRate());
            calcSpeed();
            posMove();
            adjustHeightBase(m_6a4, m_698, -1);
            bool hitToPlayer = false;
            if (CheckIsHitToPlayer()) {
                hitToPlayer = true;
            }
            if (!m_6b6 && checkArriveTargetXYZ(m_6a4, m_698)) {
                bool tmp = false;
                if (m_6b8 == 1) {
                    tmp = true;
                }
                mPos = m_698;
                mSpeedF = 0.0f;
                m_6b6 = true;
                updatePathInfo(!tmp);
            }
            if (hitToPlayer) {
                return true;
            }
        }
    }
    if (m_6b6) {
        setWalkAnm(1.0f);
        if (m_6b8 == 0) {
            deleteSound();
            m_6b2 = getWaitAngle();
            sLib::addCalcAngle(&mAngle3D.y.mAngle, m_6b2, 10, 0x2000, 0x400);
        } else {
            mAngle3D.y = m_6b2;
        }
        mAngle = mAngle3D;
        if (m_6b2 == mAngle3D.y) {
            if (m_6b8 == 1) {
                initWalk();
                return false;
            }
            return true;
        }
    }
    return false;
}

mVec3_c dWmEnemy_c::getNextPointInfo() {
    // TODO
    return getPointOffset(mPath.GetNextPointInfo(false)->mIndex);
}

mVec3_c dWmEnemy_c::getCurrentPointInfo() {
    // TODO
    return getPointOffset(mPath.GetNextPointInfo(false)->mIndex);
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
                if (m_6b7) {
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
                m_6bc = 5;
                break;
        }
    }
    switch (id) {
        case 4:
            if (m_6b7) {
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
            if (m_6e4 && dScWMap_c::m_WorldNo != WORLD_4) {
                isEnd = true;
                if (m_6bc > 0) {
                    doWalk();
                    m_6bc--;
                }
                if (m_6b6 || m_6bc == 0) {
                    if (!m_6b6) {
                        updatePathInfo(true);
                    }
                    setWalkAnm(1.0f);
                    deleteSound();
                    PostWaitWalk();
                    isEnd = false;
                    mPath.mAdvancePoint = true;
                    mPath.mDir1 = mPath.mDir2;
                    m_6bc--;
                }
            }
            break;
    }
    return isEnd;
}

void dWmEnemy_c::initDemoAnger() {
    mVec3_c playerPos = daWmPlayer_c::ms_instance->mPos;
    m_6b2 = (playerPos - mPos).xzAng();
    m_6d8 = 0;
    m_6bc = 10;
}

bool dWmEnemy_c::procDemoAnger() {
    bool res = false;
    if (m_6c4 != 2) {
        if (m_6bc > 0) {
            m_6bc--;
        } else {
            sLib::addCalcAngle(&mAngle3D.y.mAngle, m_6b2, 100, 0x4000, 0x800);
            mAngle = mAngle3D;
            if (m_6b2 == mAngle3D.y) {
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
    // TODO
    return true;
}

bool dWmEnemy_c::FUN_800f88d0() {
    return (mParam & 0xf) == dInfo_c::m_instance->m_9c;
}

bool dWmEnemy_c::isDead() {
    return dWmLib::getEnemyRevivalCount(dScWMap_c::m_WorldNo, mParam & 0xf) != 0;
}

void dWmEnemy_c::ModelCalc(m3d::mdl_c *mdl, float f1, float f2, float f3) {
    mdl->entry();
    mMatrix.trans(mPos).ZXYrotM(mAngle);
    mMatrix.trans(mVec3_c(-f1, 0.0f, 0.0f));
    mdl->setLocalMtx(&mMatrix);
    mdl->setScale(mScale);
    mdl->calc(false);
    CalcShadow(f2, f3);
}

void dWmEnemy_c::updatePathInfo(bool b) {
    mPath.UpdatePoint();
    m_6b2 = (getNextPointInfo() - mPos).xzAng();
    if (b) {
        int v = vfc4(mPath.GetPathPointNo(mPath.mpCurrentPoint->mpName));
        dInfo_c::m_instance->SetMapEnemyInfo(dScWMap_c::m_WorldNo, mParam & 0xf, dScWMap_c::m_SceneNo, v);
        dInfo_c::m_instance->FUN_800bbc40(dScWMap_c::m_WorldNo, mParam & 0xf, mPath.mDir1);
    }
}

void dWmEnemy_c::deleteSound() {
    if (m_6dc < 0) {
        return;
    }
    dWmSeManager_c::m_pInstance->endSound(m_6dc);
    m_6dc = -1;
}

void dWmEnemy_c::deleteEffect() {
    if (m_6e0 < 0) {
        return;
    }
    dWmEffectManager_c::m_pInstance->endEffect(m_6e0);
    m_6e0 = -1;
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
    const int cutsceneIDs[] = {
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
    if (!m_6e4 && !isDead() && IsPlayerComing()) {
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
    if (m_6c4 == 4 && randomNumber <= GetChangeDirRate()) {
        randomWalk = true;
    }
    return randomWalk;
}

bool dWmEnemy_c::IsHitToWaitPlayer() {
    bool res = false;
    if (!m_6e4) {
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
    if (m_6e4) {
        return false;
    }
    if (daWmPlayer_c::ms_instance->m_22c == mPath.GetNextPointIdx() && !daWmPlayer_c::ms_instance->m_18c) {
        if (mPos.distTo(daWmPlayer_c::ms_instance->mPos) < GetTerritory() * 2.0f) {
            return true;
        }
    }
    return false;
}

bool dWmEnemy_c::CheckIsHitToPlayer() {
    bool hitWait = IsHitToWaitPlayer();
    bool hitMove = IsHitToMovePlayer();
    if (hitWait || hitMove) {
        m_6e4 = true;
        if (daWmPlayer_c::isPlayerStarMode()) {
            daWmPlayer_c::ms_instance->FUN_80101280(this);
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
    for (int i = 0; i < ARRAY_SIZE(dWmEnemy::sc_EnemyProfName); i++) {
        dWmEnemy_c *en = (dWmEnemy_c *) fManager_c::searchBaseByProfName(dWmEnemy::sc_EnemyProfName[i], nullptr);
        bool res = true;
        while (en != nullptr) {
            en = (dWmEnemy_c *) fManager_c::searchBaseByProfName(dWmEnemy::sc_EnemyProfName[i], en);
            res &= en->isWaitWalkEnd();
        }
        return res;
    }
    return true;
}
