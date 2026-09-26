#include <game/bases/d_a_en_snake_block.hpp>
#include <game/bases/d_rail.hpp>
#include <game/bases/d_s_stage.hpp>

#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <constants/sound_list.h>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_audio.hpp>

ACTOR_PROFILE(EN_SNAKEBLOCK, daEnSnakeBlock_c, 0);

const float daEnSnakeBlock_c::sc_FallAccel = -0.1875f;
const float daEnSnakeBlock_c::sc_FallMaxSpeed = -4.0f;

void daEnSnakeBlock_c::dBlock_c::createMdl(mAllocator_c *allocator) {
    mResFile = dResMng_c::m_instance->getRes("block_snake_ice", "g3d/block_snake_ice.brres");

    nw4r::g3d::ResMdl mdl = mResFile.GetResMdl("block_snake_ice");
    mModel.create(mdl, allocator,
                  nw4r::g3d::ScnMdl::ANM_TEXSRT | nw4r::g3d::ScnMdl::ANM_MATCLR | nw4r::g3d::ScnMdl::BUFFER_RESMATMISC,
                  1, nullptr);
    dActor_c::setSoftLight_MapObj(mModel);

    mResTexSrt = mResFile.GetResAnmTexSrt("block_snake_ice");
    mAnmTexSrt.create(mdl, mResTexSrt, allocator, nullptr, 1);
    mAnmTexSrt.setAnm(mModel, mResTexSrt, 0, m3d::FORWARD_LOOP);
    mModel.setAnm(mAnmTexSrt, 0.0f);
    mAnmTexSrt.setRate(1.0f, 0);

    nw4r::g3d::ResAnmClr resAnmClr = mResFile.GetResAnmClr("ridden");
    mAnmClr.create(mdl, resAnmClr, allocator, nullptr, 1);
    mAnmClr.setAnm(mModel, resAnmClr, 0, m3d::FORWARD_ONCE);
    mModel.setAnm(mAnmClr);
    mAnmClr.setRate(0.0f, 0);
}

void daEnSnakeBlock_c::dBlock_c::deleteBlock() {
    mModel.remove();
    mAnmClr.remove();
    mAnmTexSrt.remove();
}

void daEnSnakeBlock_c::dBlock_c::calcAnm() {
    mAnmClr.play();
    mAnmTexSrt.play();
}

void daEnSnakeBlock_c::dBlock_c::setAnmClr(const char *name) {
    nw4r::g3d::ResAnmClr res = mResFile.GetResAnmClr(name);
    mAnmClr.setAnm(mModel, res, 0, m3d::FORWARD_ONCE);
    mModel.setAnm(mAnmClr);
    mAnmClr.setRate(1.0f, 0);
}

void daEnSnakeBlock_c::dBlock_c::draw(const mVec3_c &offset) {
    mVec3_c drawPos = mPos + offset;
    dActor_c::changePosAngle(&drawPos, nullptr, 1);

    mMtx_c mtx = mMtx_c::createTrans(drawPos.x, drawPos.y, drawPos.z);
    mModel.setLocalMtx(&mtx);

    mModel.setScale(mVec3_c(1.0f, 1.0f, 1.0f));
    mModel.calc(false);
    mModel.entry();
}

void daEnSnakeBlock_c::dBlock_c::initCollision(daEnSnakeBlock_c *owner, mVec3_c &blockPos, bool icy) {
    mVec3_c pos = blockPos;
    mPos = pos;
    if (mpOwner == nullptr) {
        mpOwner = owner;
    }

    float dx = mPos.x - mpOwner->mPos.x;
    float dy = mPos.y - mpOwner->mPos.y;

    mBgCtr.set(mpOwner, dx - 8.0f, dy + 8.0f, dx + 8.0f, dy - 8.0f,
               callBackF, callBackH, callBackW, 1, 0, nullptr);
    mBgCtr.mFlags = 0;

    // [The unused parameter was probably used to set the collision to icy before it was
    // decided to always make it icy]
    if (icy || true) {
        mBgCtr.mFlags |= 4;
    }

    mBgCtr.entry();
}

void daEnSnakeBlock_c::dBlock_c::calcCollision() {
    float dx = mPos.x - mpOwner->mPos.x;
    float dy = mPos.y - mpOwner->mPos.y;
    mBgCtr.setOfs(dx - 8.0f, dy + 8.0f, dx + 8.0f, dy - 8.0f, nullptr);
    mBgCtr.mFlags |= 2;
    mBgCtr.calc();
}

void daEnSnakeBlock_c::dBlock_c::calcFallSpeed() {
    mSpeed.y += sc_FallAccel;
    if (mSpeed.y < sc_FallMaxSpeed) {
        mSpeed.y = sc_FallMaxSpeed;
    }
}

void daEnSnakeBlock_c::dBlock_c::calcCollapse1(s8 *moveSequence) {
    static mVec2_c offsets[MOVE_DIR_COUNT] = {
        mVec2_c(0.0f, 0.0f),
        mVec2_c(0.0f, 2.0f),
        mVec2_c(0.0f, -2.0f),
        mVec2_c(-2.0f, 0.0f),
        mVec2_c(2.0f, 0.0f)
    };

    releaseBgCtr();

    u8 dir = moveSequence[mMoveSequenceIdx];
    if (dir == MOVE_DIR_NONE) {
        calcFallSpeed();
        mPos += mSpeed;
    } else {
        mPos.x += offsets[dir].x;
        mPos.y += offsets[dir].y;
        mVec2_c delta(mPos.x - mLastPos.x, mPos.y - mLastPos.y);
        if (delta.length() >= 16.0f) {
            mLastPos = mPos;
            mMoveSequenceIdx++;
        }
    }
}

void daEnSnakeBlock_c::dBlock_c::callBackF(dActor_c *self, dActor_c *other) {
    daEnSnakeBlock_c *snakeBlock = (daEnSnakeBlock_c *) self;

    bool isPlayer = false;
    if (other->mKind == dActor_c::STAGE_ACTOR_PLAYER) {
        isPlayer = true;
    } else if (other->mKind == dActor_c::STAGE_ACTOR_YOSHI && other->getPlrNo() != -1) {
        isPlayer = true;
    }

    if (isPlayer && snakeBlock->isState(StateID_Wait)) {
        snakeBlock->changeState(StateID_Move);
    }
}

void daEnSnakeBlock_c::dBlock_c::callBackH(dActor_c *self, dActor_c *other) {}
void daEnSnakeBlock_c::dBlock_c::callBackW(dActor_c *self, dActor_c *other, u8 x) {}

const mVec2_c daEnSnakeBlock_c::sc_TravelDirs[MOVE_DIR_COUNT] = {
    mVec2_c(0.0f, 0.0f),
    mVec2_c(0.0f, 1.0f),
    mVec2_c(0.0f, -1.0f),
    mVec2_c(-1.0f, 0.0f),
    mVec2_c(1.0f, 0.0f)
};

int daEnSnakeBlock_c::g_SnakeNum = 0;

const float daEnSnakeBlock_c::sc_SnakeSpeeds2[dCtrlBlock_c::SNAKE_SPEED_COUNT] = {
    0.5f, 1.0f, 1.4f
};

const float daEnSnakeBlock_c::sc_SnakeSpeeds[dCtrlBlock_c::SNAKE_SPEED_COUNT] = {
    0.5f, 1.0f, 1.4f
};

STATE_DEFINE(daEnSnakeBlock_c, Wait);
STATE_DEFINE(daEnSnakeBlock_c, Move);
STATE_DEFINE(daEnSnakeBlock_c, Shake);
STATE_DEFINE(daEnSnakeBlock_c, Collapse1);
STATE_DEFINE(daEnSnakeBlock_c, Collapse2);
STATE_DEFINE(daEnSnakeBlock_c, Collapse3);
STATE_DEFINE(daEnSnakeBlock_c, Stop);

bool daEnSnakeBlock_c::dCtrlBlock_c::moveBlock(s8 *moveSequence) {
    float speed = sc_SnakeSpeeds[mSnakeSpeed];

    mVec2_c dir;
    dir.x = sc_TravelDirs[moveSequence[mMoveSequenceIdx]].x;
    dir.y = sc_TravelDirs[moveSequence[mMoveSequenceIdx]].y;

    mPos.x += speed * dir.x;
    mPos.y += speed * dir.y;

    if (std::fabs(mPos.x - mLastPos.x) >= 16.0f || std::fabs(mPos.y - mLastPos.y) >= 16.0f) {
        return true;
    }
    return false;
}

bool daEnSnakeBlock_c::dCtrlBlock_c::nextTravelMove(s8 *moveSequence) {
    mVec2_c dir;
    dir.x = sc_TravelDirs[moveSequence[mMoveSequenceIdx]].x;
    dir.y = sc_TravelDirs[moveSequence[mMoveSequenceIdx]].y;

    mPos.x = mLastPos.x + dir.x * 16.0f;
    mPos.y = mLastPos.y + dir.y * 16.0f;
    mLastPos = mPos;

    mMoveSequenceIdx++;
    return moveSequence[mMoveSequenceIdx] == MOVE_DIR_NONE;
}

int daEnSnakeBlock_c::create() {
    if (g_SnakeNum > 0 && ACTOR_PARAM(RailStartIdx) != 0) {
        deleteRequest();
        return NOT_READY;
    }

    g_SnakeNum++;
    mBlockCount = ACTOR_PARAM(BlockCount) + 2;
    mSnakeType = ACTOR_PARAM(SnakeType);

    initBlocks();
    initBlockCollision();

    setStopState();
    changeState(StateID_Wait);

    return SUCCEEDED;
}

int daEnSnakeBlock_c::execute() {
    mStateMgr.executeState();

    calcAnm();
    calcActorPos();
    calcCollision();

    if (chkOffScreen()) {
        deleteActor(0);
    }

    return SUCCEEDED;
}

int daEnSnakeBlock_c::draw() {
    static const float sc_shakeOffsetX[2] = {
        1.0f, -1.0f
    };

    mVec3_c offset(0.0f, 0.0f, 0.0f);
    if (mShakeTimer > 0) {
        offset.x += sc_shakeOffsetX[(mShakeTimer >> 1) & 1];
    }

    getHeadBlock().draw(offset);
    getTailBlock().draw(offset);
    for (int i = 0; i < mBlockCount; i++) {
        mBlocks[i].draw(offset);
    }

    return SUCCEEDED;
}

void daEnSnakeBlock_c::deleteReady() { }

int daEnSnakeBlock_c::doDelete() {
    if (mAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        deleteBlocks();
    }

    g_SnakeNum--;
    if (g_SnakeNum < 0) {
        g_SnakeNum = 0;
    }

    return SUCCEEDED;
}

void daEnSnakeBlock_c::initBlocks() {
    mAllocator.createFrmHeapToCurrent(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20, mHeap::OPT_NONE);
    createMdl();
    initMoveSequence();
    mAllocator.adjustFrmHeapRestoreCurrent();
}

void daEnSnakeBlock_c::initBlockCollision() {
    sRailInfoData *rail = dRail_c::getRailInfoP(ACTOR_PARAM(RailID));
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);

    sRailNodeData *node = file->mpRailNodes + rail->mNodeIdx;
    node = &node[ACTOR_PARAM(RailStartIdx)];
    bool icy = ACTOR_PARAM(SnakeType);

    float nodeX = node->mX;
    float nodeY = node->mY;
    float blockPosX = nodeX + mBlockCount * 16.0f + 8.0f;
    float blockPosY = -nodeY - 8.0f;

    mVec3_c headPos(blockPosX, blockPosY, 1516.0f);
    getHeadBlock().initCollision(this, headPos, icy);

    dBlock_c *block = mBlocks;
    int i = 0;
    if (mBlockCount > 0) {
        while (i < mBlockCount) {
            mVec3_c blockPos(blockPosX, blockPosY, 1500.0f);
            block->initCollision(this, blockPos, icy);
            blockPosX -= 16.0f;
            block++;
            i++;
        }
    }

    mVec3_c tailPos = mVec3_c(blockPosX, blockPosY, 1516.0f);
    getTailBlock().initCollision(this, tailPos, icy);

    getHeadBlock().mMoveSequenceIdx = mBlockCount + 1;
    getTailBlock().mMoveSequenceIdx = 1;

    getHeadBlock().mSnakeSpeed = (dCtrlBlock_c::SnakeSpeed_e) ACTOR_PARAM(SnakeSpeed);
    getTailBlock().mSnakeSpeed = (dCtrlBlock_c::SnakeSpeed_e) ACTOR_PARAM(SnakeSpeed);
}

void daEnSnakeBlock_c::calcAnm() {
    getHeadBlock().calcAnm();
    getTailBlock().calcAnm();

    for (int i = 0; i < mBlockCount; i++) {
        mBlocks[i].calcAnm();
    }
}

void daEnSnakeBlock_c::createMdl() {
    getHeadBlock().createMdl(&mAllocator);
    getTailBlock().createMdl(&mAllocator);

    for (int i = 0; i < mBlockCount; i++) {
        mBlocks[i].createMdl(&mAllocator);
    }
}

void daEnSnakeBlock_c::deleteBlocks() {
    getHeadBlock().deleteBlock();
    getTailBlock().deleteBlock();

    for (int i = 0; i < mBlockCount; i++) {
        mBlocks[i].deleteBlock();
    }
}

void daEnSnakeBlock_c::setStopState() {
    static sStateID_c *l_StopStates[4] = {
        &StateID_Stop,
        &StateID_Collapse1,
        &StateID_Collapse2,
        &StateID_Collapse3
    };

    mpStopState = l_StopStates[ACTOR_PARAM(StopMode)];
}

void daEnSnakeBlock_c::calcCollision() {
    getHeadBlock().calcCollision();
    getTailBlock().calcCollision();
    getTailBlock().mBgCtr.mFlags |= 1;

    for (int i = 0; i < mBlockCount; i++) {
        mBlocks[i].calcCollision();
    }
}

void daEnSnakeBlock_c::initMoveSequence() {
    sRailInfoData *rail = dRail_c::getRailInfoP(ACTOR_PARAM(RailID));
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    sRailNodeData *node = cdFile->mpRailNodes + rail->mNodeIdx;

    s16 dx;
    s16 dy;
    s16 dx_abs;
    s16 dy_abs;

    ulong dist = 0;
    int startIdx = ACTOR_PARAM(RailStartIdx);
    u16 railNodeCount = rail->mCount;

    for (int i = startIdx; i < railNodeCount - 1; i++) {
        sRailNodeData *currNode = node + i;
        sRailNodeData *nextNode = node + i + 1;
        dx = abs((nextNode->mX >> 4) - (currNode->mX >> 4));
        dy = abs((nextNode->mY >> 4) - (currNode->mY >> 4));

        dist += dx + dy;
    }

    mMoveSequenceIdx = dist;
    mpMoveSequence = new s8[dist + 2]; // Add 2 for the dummy MOVE_DIR_NONE values at the start/end

    ulong currIdx = 1;
    for (int i = startIdx; i < rail->mCount - 1; i++) {
        sRailNodeData *currNode = node + i;
        sRailNodeData *nextNode = node + i + 1;
        dx = (nextNode->mX >> 4) - (currNode->mX >> 4);
        dy = (nextNode->mY >> 4) - (currNode->mY >> 4);

        dx_abs = abs(dx);
        dy_abs = abs(dy);

        MoveDir_e xDir = MOVE_DIR_NONE;
        MoveDir_e yDir = MOVE_DIR_NONE;

        if (dx > 0) {
            xDir = MOVE_DIR_RIGHT;
        } else if (dx < 0) {
            xDir = MOVE_DIR_LEFT;
        }

        if (dy < 0) {
            yDir = MOVE_DIR_UP;
        } else if (dy > 0) {
            yDir = MOVE_DIR_DOWN;
        }

        ulong moveCount = 0;
        ulong idx;
        if (dx_abs <= dy_abs) {
            for (int j = 0; j < dx_abs; j++) {
                idx = currIdx + moveCount;
                mpMoveSequence[idx] = xDir;
                mpMoveSequence[idx + 1] = yDir;
                moveCount += 2;
            }
            for (int j = 0; j < dy_abs - dx_abs; j++) {
                idx = currIdx + moveCount;
                mpMoveSequence[idx] = yDir;
                moveCount++;
            }
        } else {
            for (int j = 0; j < dy_abs; j++) {
                idx = currIdx + moveCount;
                mpMoveSequence[idx] = xDir;
                mpMoveSequence[idx + 1] = yDir;
                moveCount += 2;
            }
            for (int j = 0; j < dx_abs - dy_abs; j++) {
                idx = currIdx + moveCount;
                mpMoveSequence[idx] = xDir;
                moveCount++;
            }
        }

        currIdx += moveCount;
    }

    mpMoveSequence[0] = MOVE_DIR_NONE;
    mpMoveSequence[dist + 1] = MOVE_DIR_NONE;
}

void daEnSnakeBlock_c::calcBlockPos() {
    dBlock_c *currBlock = &mBlocks[mBlockCount - 2];
    dBlock_c *nextBlock = &mBlocks[mBlockCount - 1];
    mVec3_c pos;
    for (int i = 0; i < mBlockCount - 1; i++) {
        pos.x = currBlock->getPos().x;
        pos.y = currBlock->getPos().y;
        pos.z = 1500.0f;
        nextBlock->setPos(pos);

        nextBlock--;
        currBlock--;
    }

    // For the first body block, use the head block's position
    pos.x = getHeadBlock().getPos().x;
    pos.y = getHeadBlock().getPos().y;
    pos.z = 1500.0f;
    mBlocks[0].setPos(pos);
}

void daEnSnakeBlock_c::calcActorPos() {
    mPos.x = getHeadBlock().getPos().x;
    mPos.y = getHeadBlock().getPos().y;
    mPos.z = 1500.0f;
}

bool daEnSnakeBlock_c::chkCollapseDelete() {
    return getTailBlock().getPos().y < dBgParameter_c::ms_Instance_p->yEnd() - 16.0f;
}

bool daEnSnakeBlock_c::chkOffScreen() {
    if (isState(StateID_Wait)) {
        return false;
    }

    return mPos.x >= dBgParameter_c::ms_Instance_p->xEnd() + 512.0f;
}

void daEnSnakeBlock_c::initializeState_Wait() {}
void daEnSnakeBlock_c::finalizeState_Wait() {}
void daEnSnakeBlock_c::executeState_Wait() {}

void daEnSnakeBlock_c::initializeState_Move() {
    getHeadBlock().setAnmClr("create");
    getHeadBlock().mAnmClr.setRate(0.0f, 0);
    getHeadBlock().mAnmClr.setFrame(25.0f, 0);
    getTailBlock().setAnmClr("ridden");

    mBlocks[0].setAnmClr("create");
    for (int i = 1; i < mBlockCount; i++) {
        mBlocks[i].setAnmClr("ridden");
    }

    getHeadBlock().mLastPos = getHeadBlock().mPos;
    getTailBlock().mLastPos = getTailBlock().mPos;

    int framesPerBlock = 16.0f / sc_SnakeSpeeds[ACTOR_PARAM(SnakeSpeed)];
    mGradientActiveBlocks = 0;
    mGradientMaxBlocks = getHeadBlock().mAnmClr.getFrameMax(0) / framesPerBlock;
}

void daEnSnakeBlock_c::finalizeState_Move() {}

void daEnSnakeBlock_c::executeState_Move() {
    dBlock_c *block;
    bool headBlockMoved = getHeadBlock().moveBlock(mpMoveSequence);
    bool tailBlockMoved = getTailBlock().moveBlock(mpMoveSequence);
    bool headBlockSeqCompleted = false;
    bool tailBlockSeqCompleted = false;

    if (headBlockMoved || tailBlockMoved) {
        headBlockSeqCompleted = getHeadBlock().nextTravelMove(mpMoveSequence);
        tailBlockSeqCompleted = getTailBlock().nextTravelMove(mpMoveSequence);

        dAudio::SoundEffectID_t(scSnakeSoundID).playMapSound(mPos, 0);

        calcBlockPos();
        mGradientActiveBlocks++;
        if (mGradientActiveBlocks > mGradientMaxBlocks) {
            mGradientActiveBlocks = mGradientMaxBlocks;
        }

        block = mBlocks;
        int animFrame;
        int framesPerBlock = 16.0f / sc_SnakeSpeeds[ACTOR_PARAM(SnakeSpeed)];
        int i = 0;
        animFrame = 0;

        for (; i < mGradientActiveBlocks; i++) {
            block->setAnmClr("create");
            block->mAnmClr.setFrame(animFrame, 0);
            block++;
            animFrame += framesPerBlock;
        }
    }

    if (headBlockSeqCompleted || tailBlockSeqCompleted) {
        if (mpStopState->isEqual(StateID_Stop)) {
            changeState(StateID_Stop);
        } else {
            changeState(StateID_Shake);
        }
    }
}

void daEnSnakeBlock_c::initializeState_Shake() {
    getHeadBlock().setAnmClr("ridden");
    getHeadBlock().mAnmClr.setRate(0.0f, 0);
    getTailBlock().setAnmClr("ridden");
    getTailBlock().mAnmClr.setRate(0.0f, 0);

    for (int i = 0; i < mBlockCount; i++) {
        dBlock_c &curr = mBlocks[i];
        curr.setAnmClr("ridden");
        curr.mAnmClr.setRate(0.0f, 0);
    }

    mShakeTimer = 60;
}

void daEnSnakeBlock_c::finalizeState_Shake() {}

void daEnSnakeBlock_c::executeState_Shake() {
    if (--mShakeTimer == 0) {
        changeState(*mpStopState);
    }
}

void daEnSnakeBlock_c::initializeState_Stop() {
    getHeadBlock().setAnmClr("ridden");
    getHeadBlock().mAnmClr.setRate(0.0f, 0);
    getTailBlock().setAnmClr("ridden");
    getTailBlock().mAnmClr.setRate(0.0f, 0);

    for (int i = 0; i < mBlockCount; i++) {
        dBlock_c &block = mBlocks[i];
        block.setAnmClr("ridden");
        block.mAnmClr.setRate(0.0f, 0);
    }
}

void daEnSnakeBlock_c::finalizeState_Stop() {}
void daEnSnakeBlock_c::executeState_Stop() {}

void daEnSnakeBlock_c::initializeState_Collapse1() {
    static mVec2_c sc_collapseSpeeds[MOVE_DIR_COUNT] = {
        mVec2_c(0.0f, 0.0f),
        mVec2_c(1.0f, 3.0f),
        mVec2_c(0.0f, -1.0f),
        mVec2_c(0.0f, 0.0f),
        mVec2_c(0.0f, 0.0f)
    };

    getHeadBlock().mLastPos = getHeadBlock().mPos;
    getTailBlock().mLastPos = getTailBlock().mPos;
    for (int i = 0; (int) i < mBlockCount; i++) {
        dBlock_c &block = mBlocks[i];
        block.mLastPos = block.mPos;
    }

    dBlock_c *block = &getHeadBlock();
    int idx = block->mMoveSequenceIdx;
    for (u32 i = 0; (int) i < mBlockCount; i++) { // [fake match]
        block = &mBlocks[i];
        block->mMoveSequenceIdx = idx;
        idx--;
    }
    getTailBlock().mMoveSequenceIdx = idx;

    int dir = mpMoveSequence[mMoveSequenceIdx];
    mVec2_c collapseSpeed = sc_collapseSpeeds[dir];
    getHeadBlock().mSpeed.set(collapseSpeed.x, collapseSpeed.y, 0.0f);
    getTailBlock().mSpeed.set(collapseSpeed.x, collapseSpeed.y, 0.0f);
    for (u32 i = 0; (int) i < mBlockCount; i++) { // [fake match]
        dBlock_c &block = mBlocks[i];
        block.mSpeed.set(collapseSpeed.x, collapseSpeed.y, 0.0f);
    }
}

void daEnSnakeBlock_c::finalizeState_Collapse1() {}

void daEnSnakeBlock_c::executeState_Collapse1() {
    getHeadBlock().calcCollapse1(mpMoveSequence);
    getTailBlock().calcCollapse1(mpMoveSequence);
    for (int i = 0; i < mBlockCount; i++) {
        mBlocks[i].calcCollapse1(mpMoveSequence);
    }

    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}

void daEnSnakeBlock_c::initializeState_Collapse2() {
    mCollapseTimer = 0;
    getHeadBlock().mSpeed.set(0.0f, 0.0f, 0.0f);
    getTailBlock().mSpeed.set(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < mBlockCount; i++) {
        dBlock_c &block = mBlocks[i];
        block.mSpeed.set(0.0f, 0.0f, 0.0f);
    }
}

void daEnSnakeBlock_c::finalizeState_Collapse2() {}

void daEnSnakeBlock_c::executeState_Collapse2() {
    getHeadBlock().releaseBgCtr();
    getHeadBlock().calcFallSpeed();
    getHeadBlock().posMove();

    if (mCollapseTimer > mBlockCount * 8) {
        getTailBlock().releaseBgCtr();
        getTailBlock().calcFallSpeed();
        getTailBlock().posMove();
    }

    int collapsingBlockCount = mCollapseTimer >> 3;
    if (collapsingBlockCount > mBlockCount - 1) {
        collapsingBlockCount = mBlockCount - 1;
    }

    for (int i = 0; i < collapsingBlockCount + 1; i++) {
        dBlock_c &block = mBlocks[i];
        block.mBgCtr.release(); // [oversight: collision released twice]
        block.releaseBgCtr();
        block.calcFallSpeed();
        block.posMove();
    }

    mCollapseTimer++;
    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}

void daEnSnakeBlock_c::initializeState_Collapse3() {
    getHeadBlock().mSpeed.set(0.0f, 0.0f, 0.0f);
    getTailBlock().mSpeed.set(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < mBlockCount; i++) {
        dBlock_c &block = mBlocks[i];
        block.mSpeed.set(0.0f, 0.0f, 0.0f);
    }
}

void daEnSnakeBlock_c::finalizeState_Collapse3() {}

void daEnSnakeBlock_c::executeState_Collapse3() {
    getHeadBlock().releaseBgCtr();
    getHeadBlock().calcFallSpeed();
    getHeadBlock().mPos += getHeadBlock().mSpeed;

    getTailBlock().releaseBgCtr();
    getTailBlock().calcFallSpeed();
    getTailBlock().mPos += getTailBlock().mSpeed;

    for (int i = 0; i < mBlockCount; i++) {
        dBlock_c &block = mBlocks[i];
        block.releaseBgCtr();
        block.calcFallSpeed();
        block.mPos += block.mSpeed;
    }

    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}

const int daEnSnakeBlock_c::scSnakeSoundID = SE_OBJ_SNAKE_BLOCK;
