#include <game/bases/d_actor.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_yoshi.hpp>
#include <game/bases/d_attention.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_cd.hpp>
#include <game/bases/d_eff_actor_manager.hpp>
#include <game/bases/d_fukidashi_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_info.hpp>
#include <game/bases/d_multi_manager.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/bases/d_score_manager.hpp>
#include <game/bases/d_SmallScore.hpp>
#include <game/mLib/m_effect.hpp>
#include <nw4r/g3d.h>
#include <constants/sjis_constants.h>
#include <constants/sound_list.h>
/// @file

u8 dActor_c::mExecStopReq;
u8 dActor_c::mDrawStopReq;
u8 dActor_c::mExecStop;
u8 dActor_c::mDrawStop;
dActor_c::searchNearPlayerFunc dActor_c::mSearchNearPlayerFunc;
dActor_c::getTrgToSrcDirFunc dActor_c::mGetTrgToSrcDirFunc;

const float dActor_c::smc_CULL_XLIMIT = 80.0f;
const float dActor_c::smc_CULL_YLIMIT = 256.0f;
const float dActor_c::smc_CULL_AREA_XLIMIT = 128.0f;
const float dActor_c::smc_CULL_AREA_YLIMIT = 128.0f;

const mVec2_c dActor_c::smc_FUKIDASHI_RANGE = mVec2_c(9.0f, 12.0f);

u8 dActor_c::m_tmpCtLayerNo;

const u8 l_Ami_Line[] = { 1, 2 };
const float l_Ami_Zpos[] = { 1500.0f, -2500.0f };

u8* dActor_c::m_tmpCtSpawnFlags;
u16 dActor_c::m_tmpCtEventNums;
u64 dActor_c::m_tmpCtEventMask;
u8 dActor_c::m_tmpCtSpriteLayerNo;

dActor_c::dActor_c() :
    m_00(0), mCarryFukidashiPlayerNo(-1), m_17(0), m_1b(1.0f),
    mVisibleAreaSize(0.0f, 0.0f), mVisibleAreaOffset(0.0f, 0.0f),
    mMaxBound(0.0f, 0.0f, 0.0f, 0.0f),
    mpSpawnFlags(nullptr), mpDeleteVal(nullptr),
    mEatenByID(BASE_ID_NULL), mEatBehaviour(EAT_TYPE_EAT_PERMANENT),
    mAttentionMode(0), mPlayerNo(-1),
    mNoRespawn(false) {

    setKind(STAGE_ACTOR_GENERIC);

    mExecStopMask = 1;

    setDefaultMaxBound();
    mDestroyBound = sRangeDataF(256.0f, 256.0f, 80.0f, 80.0f);

    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    mAreaNo = file->getAreaNo(&mPos);

    mBc.mpRc = &mRc;
    mRc.set(this, 1);
    mRc.mpBc = &mBc;

    mBgCollFlags = COLL_NONE;
    mComboMultiplier = 0;

    mLayer = m_tmpCtLayerNo;
    mCc.mLayer = mLayer;
    mBc.mLayer = mLayer;
    mRc.mLayer = mLayer;

    mEatPoints = EAT_POINTS_200;
    mBlockHit = false;
}

dActor_c::~dActor_c() {
    mCc.release();
}

int dActor_c::preCreate() {
    return dBaseActor_c::preCreate() != NOT_READY;
}

void dActor_c::postCreate(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postCreate(status);
}

int dActor_c::preDelete() {
    if (dBaseActor_c::preDelete() == NOT_READY) {
        return NOT_READY;
    }
    clrComboCnt();
    return SUCCEEDED;
}

void dActor_c::postDelete(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postDelete(status);
}

int dActor_c::preExecute() {
    if (dBaseActor_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }
    if (dGameCom::isGameStop(dGameCom::GAME_STOP_ANY)) {
        return NOT_READY;
    }
    if (mExecStop & mExecStopMask) {
        return NOT_READY;
    }
    mRc.clrLink();
    if (mBlockHit == true) {
        mBlockHit = false;
        block_hit_init();
    }
    return SUCCEEDED;
}

void dActor_c::postExecute(fBase_c::MAIN_STATE_e status) {
    if (status == SUCCESS) {
        if (!(mActorProperties & 0x400)) {
            mPos.x = dScStage_c::getLoopPosX(mPos.x);
        }
        mCc.clear();
        if (!mDeleteRequested) {
            mBc.checkLink();
            mRc.chkLink();
        }
        if (mActorProperties & 0x8) {
            dAttention_c::mspInstance->entry(mUniqueID);
        }
        m_1eb.set(0.0f, 0.0f);
    }
    dBaseActor_c::postExecute(status);
}

int dActor_c::preDraw() {
    if (dBaseActor_c::preDraw() == NOT_READY || (mDrawStop & getKindMask()) != 0) {
        return NOT_READY;
    }
    if (mEatState == EAT_STATE_EATEN) {
        return NOT_READY;
    }
    if ((mActorProperties & 2) && ActorDrawCullCheck()) {
        return NOT_READY;
    }
    return SUCCEEDED;
}

void dActor_c::postDraw(fBase_c::MAIN_STATE_e status) {
    dBaseActor_c::postDraw(status);
}

const char *dActor_c::getKindString() const {
    return D_ACTOR_KIND_STRING;
}

void dActor_c::setTmpCtData(u8 layer) {
    m_tmpCtLayerNo = layer;
    return;
}

dActor_c *dActor_c::construct(ProfileName profName, unsigned long param, const mVec3_c *position, const mAng3_c *rotation, u8 layer) {
    setTmpCtData(layer);
    return (dActor_c *) dBaseActor_c::construct(profName, param, position, rotation);
}

dActor_c *dActor_c::construct(ProfileName profName, dBase_c *parent, unsigned long param, const mVec3_c *position, const mAng3_c *rotation, u8 layer) {
    setTmpCtData(layer);
    return (dActor_c *) dBaseActor_c::construct(profName, parent, param, position, rotation);
}

void dActor_c::setKind(u8 kind) {
    mKind = kind;
    mpKindString = getKindString();
}

void dActor_c::setSearchNearPlayerFunc(int loopType) {
    static const searchNearPlayerFunc searchFunc[dScStage_c::LOOP_COUNT] = {
        searchNearPlayerNormal,
        searchNearPlayerLoop,
        searchNearPlayerLoop
    };
    mSearchNearPlayerFunc = searchFunc[loopType];
}

dAcPy_c *dActor_c::searchNearPlayer(mVec2_c &delta) {
    mVec3_c centerPos = getCenterPos();
    return searchNearPlayer_Main(delta, mVec2_c(centerPos.x, centerPos.y));
}

dAcPy_c *dActor_c::searchNearPlayer_Main(mVec2_c &delta, const mVec2_c &pos) {
    return mSearchNearPlayerFunc(delta, pos);
}

dAcPy_c *dActor_c::searchNearPlayerNormal(mVec2_c &delta, const mVec2_c &pos) {
    dAcPy_c *closestPlayer = nullptr;
    float closestDist = 1e9;
    for (int i = 0; i < 4; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player == nullptr || !daPyMng_c::checkPlayer(i)) {
            continue;
        }

        float xDiff = player->mPos.x + player->mCenterOffs.x - pos.x;
        float yDiff = player->mPos.y + player->mCenterOffs.y - pos.y;
        float dist = xDiff * xDiff + yDiff * yDiff;

        if (closestDist > dist) {
            delta.set(xDiff, yDiff);
            closestPlayer = player;
            closestDist = dist;
        }
    }
    return closestPlayer;
}

dAcPy_c *dActor_c::searchNearPlayerLoop(mVec2_c &delta, const mVec2_c &pos) {
    dAcPy_c *closestPlayer = nullptr;

    float loopOffset = dBg_c::m_bg_p->mLoopOffset;

    mVec2_c loopPos;
    loopPos.x = dScStage_c::getLoopPosX(pos.x);
    loopPos.y = pos.y;
    float closestDist = 1e9;

    dAcPy_c *player;
    for (int i = 0; i < 4; i++) {
        if ((player = daPyMng_c::getPlayer(i)) == nullptr || !daPyMng_c::checkPlayer(i)) {
            continue;
        }

        mVec2_c loopPlayerPos;
        loopPlayerPos.x = dScStage_c::getLoopPosX(player->getCenterX());
        loopPlayerPos.y = player->getCenterY();

        mVec2_c adjDelta;
        adjDelta.x = loopPlayerPos.x - loopPos.x;

        // Handle loop-over
        if (adjDelta.x < 0.0f) {
            if (adjDelta.x < -loopOffset / 2) {
                adjDelta.x += loopOffset;
            }
        } else {
            if (adjDelta.x > loopOffset / 2) {
                adjDelta.x -= loopOffset;
            }
        }

        adjDelta.y = loopPlayerPos.y - loopPos.y;

        float dist = adjDelta.x * adjDelta.x + adjDelta.y * adjDelta.y;
        if (closestDist > dist) {
            delta.set(adjDelta.x, adjDelta.y);
            closestPlayer = player;
            closestDist = dist;
        }
    }
    return closestPlayer;
}

void dActor_c::setGetTrgToSrcDirFunc(int loopType) {
    static const getTrgToSrcDirFunc getdirFunc[dScStage_c::LOOP_COUNT] = {
        getTrgToSrcDirNormal,
        getTrgToSrcDirLoop,
        getTrgToSrcDirLoop
    };
    mGetTrgToSrcDirFunc = getdirFunc[loopType];
}

bool dActor_c::getTrgToSrcDir_Main(float trgX, float srcX) {
    return mGetTrgToSrcDirFunc(trgX, srcX);
}

bool dActor_c::getTrgToSrcDirNormal(float trgX, float srcX) {
    return trgX < srcX;
}

bool dActor_c::getTrgToSrcDirLoop(float trgX, float srcX) {
    float loopTrgX = dScStage_c::getLoopPosX(trgX);
    float loopSrcX = dScStage_c::getLoopPosX(srcX);
    float diffX = loopTrgX - loopSrcX;

    float loopOffset = dBg_c::m_bg_p->mLoopOffset / 2;
    if (diffX < 0.0f) {
        return !(diffX < -loopOffset);
    } else {
        return diffX > loopOffset;
    }
}

void dActor_c::changePosAngle(mVec3_c *pos, mAng3_c *ang, int param3) {
    dScStage_c::changePos(pos);
}

void dActor_c::setChangePosAngleFunc(int loopType) {
    dScStage_c::setChangePosFunc(loopType);
}

void dActor_c::setLoopFunc(int loopType) {
    setChangePosAngleFunc(loopType);
    setSearchNearPlayerFunc(loopType);
    setGetTrgToSrcDirFunc(loopType);
}

bool dActor_c::checkAreaNo() {
    for (int i = 0; i < 4; i++) {
        dActor_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && player->mAreaNo == mAreaNo) {
            return true;
        }
    }
    return false;
}

void dActor_c::setSoftLight_Player(m3d::bmdl_c &mdl) {
    dGameCom::SetSoftLight_Player(mdl, 0);
}

void dActor_c::setSoftLight_Enemy(m3d::bmdl_c &mdl) {
    dGameCom::SetSoftLight_Enemy(mdl, 0);
}

void dActor_c::setSoftLight_Map(m3d::bmdl_c &mdl) {
    dGameCom::SetSoftLight_Map(mdl, 0);
}

void dActor_c::setSoftLight_MapObj(m3d::bmdl_c &mdl) {
    dGameCom::SetSoftLight_MapObj(mdl, 0);
}

void dActor_c::setSoftLight_Boss(m3d::bmdl_c &mdl) {
    dGameCom::SetSoftLight_Boss(mdl, 0);
}

void dActor_c::setSoftLight_Item(m3d::bmdl_c &mdl) {
    nw4r::g3d::ResMdl resMdl = mdl.getResMdl();
    m3d::replaceLightMapTexture("lm_01i", mdl, 0);
    m3d::replaceLightMapTexture("lm_02i", mdl, 0);

    for (size_t i = 0; i < resMdl.GetResMatNumEntries(); i++) {
        nw4r::g3d::ScnMdl::CopiedMatAccess cma(nw4r::g3d::G3dObj::DynamicCast<nw4r::g3d::ScnMdl>(mdl.getScn()), i);
        nw4r::g3d::ResMatMisc mat = cma.GetResMatMisc(false);
        if (mat.IsValid()) {
            mat.SetLightSetIdx(6);
        }
    }
}

void dActor_c::deleteActor(u8 deleteForever) {
    deleteRequest();

    u8 *spawnFlags = mpSpawnFlags;
    u16 *deleteVal = mpDeleteVal;
    if (spawnFlags == nullptr || deleteVal == nullptr) {
        return;
    }

    if (!deleteForever) {
        if (spawnFlags != nullptr) {
            *spawnFlags = *spawnFlags & ~ACTOR_SPAWNED;
        }
    } else {
        if (spawnFlags != nullptr) {
            *spawnFlags = *spawnFlags | ACTOR_NO_RESPAWN;
        }
        if (deleteVal != nullptr) {
            *deleteVal = 300;
        }
    }
}

bool dActor_c::areaCullCheck(const mVec3_c &pos, const sRangeDataF &bound, u8 areaID) const {
    dCdFile_c *course = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    sAreaData *area = course->getAreaDataP(areaID, nullptr);
    if (area == nullptr) {
        return true;
    }

    // [Probably some more inlining going on here]
    mVec2_c b;
    mVec2_c bt;
    bt.x = pos.x + bound.mOffset.x - bound.mSize.x;
    bt.y = pos.y + bound.mOffset.y + bound.mSize.y;
    b.set(bt.x, bt.y);

    b.x -= area->mRangeData.mX;
    b.y += area->mRangeData.mY;

    mVec2_c doubleBoundSize = bound.getSize();
    doubleBoundSize.x = 2.0f * doubleBoundSize.x;
    doubleBoundSize.y = 2.0f * doubleBoundSize.y;

    mVec2_c maxBoundSize = mMaxBound.getSize();

    if (!(course->mpOptions->mFlags & sOptionData::WRAP_AROUND_EDGES)) {
        if (b.x + doubleBoundSize.x < -maxBoundSize.x || b.x > area->mRangeData.mWidth + maxBoundSize.x) {
            return true;
        }
    }
    if (b.y - doubleBoundSize.y > maxBoundSize.y || b.y < -(area->mRangeData.mHeight + maxBoundSize.y)) {
        return true;
    }

    return false;
}

bool dActor_c::ActorScrOutCheck(u16 flags) {
    if (mEatState == EAT_STATE_EATEN) {
        return false;
    }

    if (!(flags & SKIP_RIDE_CHECK) && mBc.checkRide()) {
        return false;
    }

    sRangeDataF visibleBound;
    visibleBound.mOffset.x = mVisibleAreaOffset.x;
    visibleBound.mOffset.y = mVisibleAreaOffset.y;
    visibleBound.mSize.x = mVisibleAreaSize.x * 0.5f;
    visibleBound.mSize.y = mVisibleAreaSize.y * 0.5f;

    bool outOfScreen = false;
    if (areaCullCheck(mPos, visibleBound, mAreaNo)) {
        outOfScreen = true;
    } else if (!(flags & SKIP_SCREEN_CHECK)) {
        if (screenCullCheck(mPos, visibleBound, getDestroyBound(), mAreaNo)) {
            outOfScreen = true;
        }
    }

    if (outOfScreen && !(flags & SKIP_ACTOR_DELETE)) {
        deleteActor(mNoRespawn);
    }

    return outOfScreen;
}

bool dActor_c::ActorDrawCullCheck() {
    sRangeDataF bound;
    bound.mOffset.x = mVisibleAreaOffset.x;
    bound.mOffset.y = mVisibleAreaOffset.y;
    bound.mSize.x = mVisibleAreaSize.x * 0.5f;
    bound.mSize.y = mVisibleAreaSize.y * 0.5f + 16.0f;

    mVec3_c pos = mPos;
    changePosAngle(&pos, nullptr, 1);

    return dGameCom::someCheck(&pos, &bound);
}

bool dActor_c::checkBgColl() {
    if (mBgCollFlags != 0) {
        if ((mBgCollFlags & COLL_HEAD) && mBc.isHead()) {
            return true;
        }
        if ((mBgCollFlags & COLL_WALL_L) && mBc.isWallL()) {
            return true;
        }
        if ((mBgCollFlags & COLL_FOOT) && mBc.isFoot()) {
            return true;
        }
        if ((mBgCollFlags & COLL_WALL_R) && mBc.isWallR()) {
            return true;
        }
    }
    return false;
}

bool dActor_c::carryFukidashiCheck(int fukidashiAction, mVec2_c fukidashiTriggerSize) {
    mVec3_c centerPos = getCenterPos();

    mVec3_c minTriggerPos(centerPos.x - fukidashiTriggerSize.x, centerPos.y - fukidashiTriggerSize.y, mPos.z);
    mVec3_c maxTriggerPos(centerPos.x + fukidashiTriggerSize.x, centerPos.y + fukidashiTriggerSize.y, mPos.z);

    if (mCarryFukidashiPlayerNo <= 3 && dInfo_c::m_instance->mFukidashiActionPerformed[mCarryFukidashiPlayerNo][fukidashiAction]) {
        mCarryFukidashiPlayerNo = -1;
    }

    if (mCarryFukidashiPlayerNo <= 3) {
        // Already displaying a fukidashi
        dAcPy_c *player = daPyMng_c::getPlayer(mCarryFukidashiPlayerNo);
        if (player != nullptr) {
            bool canDrawFukidashi = player->isDrawingCarryFukidashi();

            sRangeDataF playerBoundBox;
            player->getCcBounds(playerBoundBox);
            mVec3_c playerPos(
                dScStage_c::getLoopPosX(playerBoundBox.mOffset.x + player->mPos.x),
                playerBoundBox.mOffset.y + player->mPos.y,
                player->mPos.z
            );

            mVec3_c minPlayerPos(playerPos.x - playerBoundBox.mSize.x - 2.0f, playerPos.y - playerBoundBox.mSize.y, playerPos.z);
            mVec3_c maxPlayerPos(playerPos.x + playerBoundBox.mSize.x + 2.0f, playerPos.y + playerBoundBox.mSize.y, playerPos.z);

            if (!dfukidashiManager_c::m_instance->mInfos[mCarryFukidashiPlayerNo].mVisible) {
                dGameCom::showFukidashi(mCarryFukidashiPlayerNo, fukidashiAction);
            }

            bool overlap = dGameCom::checkRectangleOverlap(&minTriggerPos, &maxTriggerPos, &minPlayerPos, &maxPlayerPos, 0.0f);
            if ((!canDrawFukidashi) || (!overlap)) {
                dGameCom::hideFukidashiTemporarily(mCarryFukidashiPlayerNo, fukidashiAction, 0);
                mCarryFukidashiPlayerNo = -1;
            }
        }
    } else {
        // Not displaying a fukidashi yet, search for a player to show it
        dAcPy_c *player = searchCarryFukidashiPlayer(fukidashiAction);
        if (player != nullptr) {
            bool canDrawFukidashi = player->isDrawingCarryFukidashi();

            sRangeDataF playerBoundBox;
            player->getCcBounds(playerBoundBox);
            mVec3_c playerPos(
                dScStage_c::getLoopPosX(playerBoundBox.mOffset.x + player->mPos.x),
                playerBoundBox.mOffset.y + player->mPos.y,
                player->mPos.z
            );

            mVec3_c minPlayerPos(playerPos.x - playerBoundBox.mSize.x, playerPos.y - playerBoundBox.mSize.y, playerPos.z);
            mVec3_c maxPlayerPos(playerPos.x + playerBoundBox.mSize.x, playerPos.y + playerBoundBox.mSize.y, playerPos.z);

            bool overlap = dGameCom::checkRectangleOverlap(&minTriggerPos, &maxTriggerPos, &minPlayerPos, &maxPlayerPos, 0.0f);
            if (canDrawFukidashi && overlap) {
                mCarryFukidashiPlayerNo = *player->getPlrNo();
            }
        }
    }

    return false;
}

void dActor_c::carryFukidashiCancel(int fukidashiAction, int playerId) {
    for (int i = 0; i < 4; i++) {
        if (i == playerId) {
            dGameCom::hideFukidashiForLevel(i, fukidashiAction, 0);
            dGameCom::hideFukidashiForSession(i, fukidashiAction);
        } else {
            dGameCom::hideFukidashiTemporarily(i, fukidashiAction, 0);
        }
    }
    mCarryFukidashiPlayerNo = -1;
}

dAcPy_c *dActor_c::searchCarryFukidashiPlayer(int fukidashiAction) {
    mVec3_c center = getCenterPos();

    dAcPy_c *closestPlayer = nullptr;
    float closestDist = 1e9;
    for (int i = 0; i < 4; i++) {
        if (daPyMng_c::checkPlayer(i) && !dInfo_c::m_instance->mFukidashiActionPerformed[i][fukidashiAction]) {
            dAcPy_c *player = daPyMng_c::getPlayer(i);
            if (player == nullptr) {
                continue;
            }

            float diffX = player->mPos.x + player->mCenterOffs.x - center.x;
            float diffY = player->mPos.y + player->mCenterOffs.y - center.y;
            float dist = diffX * diffX + diffY * diffY;
            if (closestDist > dist) {
                closestPlayer = player;
                closestDist = dist;
            }
        }
    }
    return closestPlayer;
}

mVec2_c dActor_c::getLookatPos() const {
    return mVec2_c(mPos.x + mCenterOffs.x, mPos.y + mCenterOffs.y);
}

void dActor_c::block_hit_init() {}

void dActor_c::allEnemyDeathEffSet() {
    mVec3_c effPos = getCenterPos();
    effPos.y += 4.0f;
    mEf::createEffect("Wm_en_burst_s", 0, &effPos, nullptr, nullptr);
}

void dActor_c::killActor(s8 playerId, int noScore) {
    mVec3_c effPos = mPos;
    mVec3_c centerOffs = mCenterOffs;

    mNoRespawn = true;
    for (int i = 0; i < daPyMng_c::mNum; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && fManager_c::searchBaseByID(player->mCarryActorID) == this) {
            player->cancelCarry(this);
        }
    }

    effPos += centerOffs;
    mEf::createEffect("Wm_en_burst_s", 0, &effPos, nullptr, nullptr);
    deleteActor(1);

    if (!noScore) {
        if (playerId < 0) {
            dScoreMng_c::m_instance->UnKnownScoreSet(this, 1, dScoreMng_c::smc_SCORE_X, dScoreMng_c::smc_SCORE_Y);
        } else {
            dScoreMng_c::m_instance->ScoreSet(this, 1, playerId, dScoreMng_c::smc_SCORE_X, dScoreMng_c::smc_SCORE_Y);
        }
    }
}

void dActor_c::setSpinLiftUpActor(dActor_c *carryingActor) {}

void dActor_c::setEatTongue(dActor_c *eatingActor) {
    mPreEatScale = mScale;
}

void dActor_c::setEatTongueOff(dActor_c *eatingActor) {}

void dActor_c::setEatMouth(dActor_c *eatingActor) {}

bool dActor_c::setEatSpitOut(dActor_c *eatingActor) {
    return true;
}

bool dActor_c::setEatGlupDown(dActor_c *eatingActor) {
    static const int yoshiEatPopupTypes[] = { dSmallScore_c::POPUP_TYPE_200, dSmallScore_c::POPUP_TYPE_1000 };
    static const int yoshiEatPoints[] = { 200, 1000 };

    if (mEatPoints != EAT_POINTS_NONE) {
        mVec3_c smallScorePos = eatingActor->mPos;
        smallScorePos.y += 40.0f;

        s8 plrNo = *eatingActor->getPlrNo();
        dGameCom::CreateSmallScore(smallScorePos, yoshiEatPopupTypes[mEatPoints], plrNo, false);

        if (plrNo != -1) {
            daPyMng_c::addScore(yoshiEatPoints[mEatPoints], plrNo);
            dMultiMng_c::mspInstance->incEnemyDown(plrNo);
        }
    }

    deleteRequest();
    return true;
}

void dActor_c::setAfterEatScale() {
    mScale = mPreEatScale;
}

void dActor_c::calcSpitOutPos(dActor_c *eatingActor) {
    mMtx_c mouthMtx;
    daYoshi_c *yoshi = (daYoshi_c *) eatingActor;

    yoshi->getMouthMtx(&mouthMtx);
    mMtx_c transposeMtx;
    PSMTXTrans(transposeMtx, 10.0f, -7.0f, 0.0f);
    mMtx_c resMtx;
    PSMTXConcat(mouthMtx, transposeMtx, mouthMtx);
    mPos.x = mouthMtx.transX();
    mPos.y = mouthMtx.transY();
}

float dActor_c::calcEatScaleRate(dActor_c *eatingActor) {
    float res = 1.0f;
    daYoshi_c *yoshi = (daYoshi_c *) eatingActor;

    if (yoshi->m_a0 <= 1) {
        res = yoshi->m_a0 / 1.25f;
    }
    return res;
}

void dActor_c::calcEatInScale(dActor_c *eatingActor) {
    mScale = mPreEatScale;

    float scaleRate = calcEatScaleRate(eatingActor);
    if (scaleRate < 1.0f) {
        mScale *= scaleRate;
    }
}

void dActor_c::eatMove(dActor_c *eatingActor) {
    mMtx_c tongueTipMtx;
    daYoshi_c *yoshi = (daYoshi_c *) eatingActor;

    yoshi->getTongueTipMtx(&tongueTipMtx);
    mPos.x = tongueTipMtx.transX();
    mPos.y = tongueTipMtx.transY() - mCenterOffs.y;
}

void dActor_c::vfb4() {}

void dActor_c::cancelFunsuiActUpper() {}

void dActor_c::cancelFunsuiActSide(int) {}

void dActor_c::cancelFunsuiActVanish() {}

void dActor_c::slideComboSE(int multiplier, bool shortCombo) {
    static const dAudio::SoundEffectID_t cs_combo_se[] = {
        SE_EMY_KAME_HIT_1,
        SE_EMY_KAME_HIT_2,
        SE_EMY_KAME_HIT_3,
        SE_EMY_KAME_HIT_4,
        SE_EMY_KAME_HIT_5,
        SE_EMY_KAME_HIT_6,
        SE_EMY_KAME_HIT_7,
        SE_EMY_KAME_HIT_7,
        SE_EMY_KAME_HIT_7
    };

    if ((u8) mPlayerNo <= 3) {
        int combo = multiplier;
        if (combo >= ARRAY_SIZE(cs_combo_se)) {
            combo = ARRAY_SIZE(cs_combo_se) - 1;
        }
        int comboNeededForClaps = shortCombo ? 4 : 7;
        if (combo >= comboNeededForClaps) {
            dMultiMng_c::mspInstance->setClapSE();
        }

        cs_combo_se[combo].playEmySound(getCenterPos(), dAudio::getRemotePlayer(mPlayerNo));
    }
}

void dActor_c::clrComboCnt() {
    mComboMultiplier = 0;
}

void dActor_c::waterSplashEffect(const mVec3_c &pos, float size) {
    mVec3_c shiftedPos(pos, 6500.0f);

    int waterDepth = dBc_c::checkWaterDepth(shiftedPos.x, shiftedPos.y, mLayer, mBc.mLineKind, nullptr);

    u32 splashInfo = 0;
    if (waterDepth < 3) {
        splashInfo = 1;
    }
    splashInfo |= (mLayer << 16);

    mVec3_c splashSize(size, size, size);
    dEffActorMng_c::m_instance->createWaterSplashEff(shiftedPos, splashInfo, -1, splashSize);

    dAudio::g_pSndObjMap->startSound(SE_OBJ_CMN_SPLASH, shiftedPos, 0);

    dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 6);
}

void dActor_c::yoganSplashEffect(const mVec3_c &pos, float size) {
    mVec3_c shiftedPos(pos, 6500.0f);

    u32 splashInfo = 4 | (mLayer << 16);

    mVec3_c splashSize(size, size, size);
    dEffActorMng_c::m_instance->createWaterSplashEff(shiftedPos, splashInfo, -1, splashSize);

    dAudio::g_pSndObjMap->startSound(SE_OBJ_CMN_SPLASH_LAVA, shiftedPos, 0);

    if (size < 1.0f) {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 16);
    } else {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 14);
    }
}

void dActor_c::poisonSplashEffect(const mVec3_c &pos, float size) {
    mVec3_c shiftedPos(pos, 6500.0f);

    u32 splashInfo = 6 | (mLayer << 16);

    mVec3_c splashSize(size, size, size);
    dEffActorMng_c::m_instance->createWaterSplashEff(shiftedPos, splashInfo, -1, splashSize);

    dAudio::g_pSndObjMap->startSound(SE_OBJ_CMN_SPLASH_POISON, pos, 0);

    if (size < 1.0f) {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 23);
    } else {
        dBg_c::m_bg_p->setWaterInWave(pos.x, pos.y, 21);
    }
}

bool dActor_c::checkCarried(int *playerNum) {
    for (int i = 0; i < 4; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && fManager_c::searchBaseByID(player->mCarryActorID) == this) {
            if (playerNum != nullptr) {
                *playerNum = *player->getPlrNo();
            }
            return true;
        }
    }
    if (playerNum != nullptr) {
        *playerNum = -1;
    }
    return false;
}

void dActor_c::removeCc() {
    mCc.release();
}

void dActor_c::reviveCc() {
    mCc.entry();
}
