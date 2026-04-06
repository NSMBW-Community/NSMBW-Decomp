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

void daEnSnakeBlock_c::dBlock_c::createMdl(dHeapAllocator_c *alloc) {
    mResFile = dResMng_c::m_instance->getRes("block_snake_ice", "g3d/block_snake_ice.brres");

    nw4r::g3d::ResMdl mdl = mResFile.GetResMdl("block_snake_ice");
    mModel.create(mdl, alloc, 0x32c, 1, nullptr);
    dActor_c::setSoftLight_MapObj(mModel);

    mResTexSrt = mResFile.GetResAnmTexSrt("block_snake_ice");
    mAnmTexSrt.create(mdl, mResTexSrt, alloc, nullptr, 1);
    mAnmTexSrt.setAnm(mModel, mResTexSrt, 0, m3d::FORWARD_LOOP);
    mModel.setAnm(mAnmTexSrt, 0.0f);
    mAnmTexSrt.setRate(1.0f, 0);

    nw4r::g3d::ResAnmClr res_anmclr = mResFile.GetResAnmClr("ridden");
    mAnmClr.create(mdl, res_anmclr, alloc, nullptr, 1);
    mAnmClr.setAnm(mModel, res_anmclr, 0, m3d::FORWARD_ONCE);
    mModel.setAnm(mAnmClr);
    mAnmClr.setRate(0.0f, 0);
}

void daEnSnakeBlock_c::dBlock_c::doDelete() {
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

void daEnSnakeBlock_c::dBlock_c::draw(const mVec3_c &pos) {
    mVec3_c a = mPos + pos;
    dActor_c::changePosAngle(&a, nullptr, 1);

    mMtx_c b = mMtx_c::createTrans(a);
    mModel.setLocalMtx(&b);

    mVec3_c one = mVec3_c(1.0f, 1.0f, 1.0f);
    mModel.setScale(one);
    mModel.calc(false);
    mModel.entry();
}

void daEnSnakeBlock_c::dBlock_c::initBgCtr(daEnSnakeBlock_c *owner, mVec3_c &blockPos, int _unused) {
    mVec3_c pos = blockPos;
    mPos = pos;
    if (mpOwner == nullptr) {
        mpOwner = owner;
    }

    float dx = mPos.x - mpOwner->mPos.x;
    float dy = mPos.y - mpOwner->mPos.y;

    mBgCtr.set(mpOwner, dx - 8.0f, dy + 8.0f, dx + 8.0f, dy - 8.0f,
        callBackF, callBackH, callBackW, 1, 0, nullptr
    );
    mBgCtr.mFlags = 0;
    // The unused flag was probably used to set the collision to icy before they
    // decided to always make it icy.
    /* if (_unused) { */
    mBgCtr.mFlags |= 4;
    /* } */
    mBgCtr.entry();
}

void daEnSnakeBlock_c::dBlock_c::calcBgCtr() {
    float dx = mPos.x - mpOwner->mPos.x;
    float dy = mPos.y - mpOwner->mPos.y;
    mBgCtr.setOfs(dx - 8.0f, dy + 8.0f, dx + 8.0f, dy - 8.0f, nullptr);
    mBgCtr.mFlags |= 2;
    mBgCtr.calc();
}

void daEnSnakeBlock_c::dBlock_c::setFallCollapse() {
    mSpeed.y += sc_FallAccel;
    if (mSpeed.y < sc_FallMaxSpeed) {
        mSpeed.y = sc_FallMaxSpeed;
    }
}

void daEnSnakeBlock_c::dBlock_c::calcCollapse1(s8 *travelInfo) {
    static mVec2_c floats[] = {
        mVec2_c(0.0f, 0.0f),
        mVec2_c(0.0f, 2.0f),
        mVec2_c(0.0f, -2.0f),
        mVec2_c(-2.0f, 0.0f),
        mVec2_c(2.0f, 0.0f)
    };

    if (mBgCtr.m_dc) {
        mBgCtr.release();
    }

    u8 info = travelInfo[mTravelInfoIdx];
    if (info == 0) {
        setFallCollapse();
        mPos += mSpeed;
    } else {
        mPos.x += floats[info].x;
        mPos.y += floats[info].y;
        mVec2_c diff(mPos.x - mLastPos.x, mPos.y - mLastPos.y);
        if (diff.length() >= 16.0f) {
            mLastPos = mPos;
            mTravelInfoIdx++;
        }
    }
}

void daEnSnakeBlock_c::dBlock_c::callBackF(dActor_c *self, dActor_c *other) {
    daEnSnakeBlock_c *y = (daEnSnakeBlock_c *) self;

    bool x = false;
    if (other->mKind == dActor_c::STAGE_ACTOR_PLAYER) {
        x = true;
    } else if (other->mKind == dActor_c::STAGE_ACTOR_YOSHI && *other->getPlrNo() != -1) {
        x = true;
    }

    if (x && y->isState(StateID_Wait)) {
        y->changeState(StateID_Move);
    }
}

void daEnSnakeBlock_c::dBlock_c::callBackH(dActor_c *self, dActor_c *other) {}
void daEnSnakeBlock_c::dBlock_c::callBackW(dActor_c *self, dActor_c *other, u8 x) {}

mVec2_c daEnSnakeBlock_c::sc_ctrlPosMods[5] = {
    mVec2_c(0.0f, 0.0f),
    mVec2_c(0.0f, 1.0f),
    mVec2_c(0.0f, -1.0f),
    mVec2_c(-1.0f, 0.0f),
    mVec2_c(1.0f, 0.0f)
};

int daEnSnakeBlock_c::sc_glbSnakeNum = 0;

STATE_DEFINE(daEnSnakeBlock_c, Wait);
STATE_DEFINE(daEnSnakeBlock_c, Move);
STATE_DEFINE(daEnSnakeBlock_c, Shake);
STATE_DEFINE(daEnSnakeBlock_c, Collapse1);
STATE_DEFINE(daEnSnakeBlock_c, Collapse2);
STATE_DEFINE(daEnSnakeBlock_c, Collapse3);
STATE_DEFINE(daEnSnakeBlock_c, Stop);

const float daEnSnakeBlock_c::sc_snakeSpeeds2[3] = {
    0.5f, 1.0f, 1.4f
};

const float daEnSnakeBlock_c::sc_snakeSpeeds[3] = {
    0.5f, 1.0f, 1.4f
};

const float daEnSnakeBlock_c::sc_snakeDir[2] = {
    1.0f, -1.0f
};

bool daEnSnakeBlock_c::dCtrlBlock_c::calcPos(s8 *travelInfo) {
    float speed = sc_snakeSpeeds[mSnakeSpeedIdx];

    mVec2_c newPos;
    newPos.x = sc_ctrlPosMods[travelInfo[mTravelInfoIdx]].x;
    newPos.y = sc_ctrlPosMods[travelInfo[mTravelInfoIdx]].y;

    mPos.x += speed * newPos.x;
    mPos.y += speed * newPos.y;

    if (std::fabs(mPos.x - mLastPos.x) >= 16.0f || std::fabs(mPos.y - mLastPos.y) >= 16.0f) {
        return true;
    }
    return false;
}

bool daEnSnakeBlock_c::dCtrlBlock_c::calcTravelPos(s8 *travelInfo) {
    mVec2_c newPos;
    newPos.x = sc_ctrlPosMods[travelInfo[mTravelInfoIdx]].x;
    newPos.y = sc_ctrlPosMods[travelInfo[mTravelInfoIdx]].y;

    mPos.x = mLastPos.x + newPos.x * 16.0f;
    mPos.y = mLastPos.y + newPos.y * 16.0f;

    mLastPos = mPos;

    mTravelInfoIdx++;
    return travelInfo[mTravelInfoIdx] == 0;
}

int daEnSnakeBlock_c::create() {
    if (sc_glbSnakeNum > 0 && (((mParam >> 0xC) & 0xFF) != 0)) {
        deleteRequest();
        return NOT_READY;
    }

    sc_glbSnakeNum++;
    mBlockNum = (mParam & 0xF) + 2;
    mSnakeType = (mParam >> 20) & 1;

    initBlock();
    initBlockPath();
    setStopState();

    changeState(StateID_Wait);

    return SUCCEEDED;
}

int daEnSnakeBlock_c::execute() {
    mStateMgr.executeState();

    calcAnm();
    setActorPos();
    calcBgCtr();

    if (chkOffScreen()) {
        deleteActor(0);
    }

    return SUCCEEDED;
}

int daEnSnakeBlock_c::draw() {
    mVec3_c offset(0.0f, 0.0f, 0.0f);

    if (mShakeTime > 0) {
        offset.x += sc_snakeDir[(mShakeTime >> 1) & 1];
    }

    getHeadBlock()->draw(offset);
    getTailBlock()->draw(offset);
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].draw(offset);
    }

    return SUCCEEDED;
}

void daEnSnakeBlock_c::deleteReady() { }

int daEnSnakeBlock_c::doDelete() {
    if (mAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        deleteBlock();
    }

    sc_glbSnakeNum--;
    if (sc_glbSnakeNum < 0) {
        sc_glbSnakeNum = 0;
    }

    return SUCCEEDED;
}

void daEnSnakeBlock_c::initBlock() {
    mAllocator.createFrmHeapToCurrent(-1, mHeap::g_gameHeaps[0], nullptr, 0x20, mHeap::OPT_NONE);
    createMdl();
    initTravelInfo();
    mAllocator.adjustFrmHeapRestoreCurrent();
}

void daEnSnakeBlock_c::initBlockPath() {
    sRailInfoData *rail = dRail_c::getRailInfoP(ACTOR_PARAM(RailIdx));
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);

    sRailNodeData *node = &file->mpRailNodes[rail->mNodeIdx];
    node = &node[ACTOR_PARAM(RailStartIdx)];

    bool icy = (mParam >> 20) & 1;

    float nodeX = node[0].mX;
    float nodeY = node[0].mY;
    float x = nodeX + mBlockNum * 16.0f + 8.0f;
    float y = -nodeY - 8.0f;

    mVec3_c headPos(x, y, 1516.0f);

    getHeadBlock()->initBgCtr(this, headPos, icy);

    dBlock_c *curr = mBlocks;
    int i = 0;
    if (mBlockNum > 0) {
        while (i < mBlockNum) {
            mVec3_c midPos(x, y, 1500.0f);
            curr->initBgCtr(this, midPos, icy);
            x -= 16.0f;
            curr++;
            i++;
        }
    }

    mVec3_c tailPos = mVec3_c(x, y, 1516.0f);
    getTailBlock()->initBgCtr(this, tailPos, icy);

    getHeadBlock()->mTravelInfoIdx = mBlockNum + 1;
    getTailBlock()->mTravelInfoIdx = 1;

    // Note: Value 3 is out of bounds
    getHeadBlock()->mSnakeSpeedIdx = (mParam >> 8) & 3;
    getTailBlock()->mSnakeSpeedIdx = (mParam >> 8) & 3;
}

void daEnSnakeBlock_c::calcAnm() {
    getHeadBlock()->calcAnm();
    getTailBlock()->calcAnm();

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].calcAnm();
    }
}

void daEnSnakeBlock_c::createMdl() {
    getHeadBlock()->createMdl(&mAllocator);
    getTailBlock()->createMdl(&mAllocator);

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].createMdl(&mAllocator);
    }
}

void daEnSnakeBlock_c::deleteBlock() {
    getHeadBlock()->doDelete();
    getTailBlock()->doDelete();

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].doDelete();
    }
}

sStateID_c *daEnSnakeBlock_c::sc_stopStates[4] = {
    &StateID_Stop, &StateID_Collapse1, &StateID_Collapse2, &StateID_Collapse3
};

void daEnSnakeBlock_c::setStopState() {
    mpStopState = sc_stopStates[mParam >> 0x1c];
}

void daEnSnakeBlock_c::calcBgCtr() {
    getHeadBlock()->calcBgCtr();
    getTailBlock()->calcBgCtr();

    getTailBlock()->mBgCtr.mFlags |= 1;

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].calcBgCtr();
    }
}

void daEnSnakeBlock_c::initTravelInfo() {
    sRailInfoData *rail = dRail_c::getRailInfoP(ACTOR_PARAM(RailIdx));
    dCdFile_c *cdFile = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);
    sRailNodeData *node = cdFile->mpRailNodes + rail->mNodeIdx;

    s16 dx;
    s16 dy;
    s16 dx_abs;
    s16 dy_abs;

    ulong dist = 0;

    int startIdx = ACTOR_PARAM(RailStartIdx);
    u16 railCount = rail->mCount;

    for (int i = startIdx; i < railCount - 1; i++) {
        sRailNodeData *curr = node + i;
        sRailNodeData *next = node + i + 1;
        dx = abs((next->mX >> 4) - (curr->mX >> 4));
        dy = abs((next->mY >> 4) - (curr->mY >> 4));

        dist += dx + dy;
    }

    mTravelInfoIdx = dist;
    mpTravelInfo = new s8[dist + 2];

    ulong currIdx = 1;
    for (int i = startIdx; i < rail->mCount - 1; i++) {
        sRailNodeData *curr = node + i;
        sRailNodeData *next = node + i + 1;
        dx = (next->mX >> 4) - (curr->mX >> 4);
        dy = (next->mY >> 4) - (curr->mY >> 4);

        dx_abs = abs(dx);
        dy_abs = abs(dy);

        TravelDir_e xDir = TRAVEL_DIR_NONE;
        TravelDir_e yDir = TRAVEL_DIR_NONE;

        if (dx > 0) {
            xDir = TRAVEL_DIR_RIGHT;
        } else if (dx < 0) {
            xDir = TRAVEL_DIR_LEFT;
        }

        if (dy < 0) {
            yDir = TRAVEL_DIR_UP;
        } else if (dy > 0) {
            yDir = TRAVEL_DIR_DOWN;
        }

        ulong count = 0;
        ulong idx;
        if (dx_abs <= dy_abs) {
            for (int j = 0; j < dx_abs; j++) {
                idx = currIdx + count;
                mpTravelInfo[idx] = xDir;
                mpTravelInfo[idx + 1] = yDir;
                count += 2;
            }
            for (int j = 0; j < dy_abs - dx_abs; j++) {
                idx = currIdx + count;
                mpTravelInfo[idx] = yDir;
                count++;
            }
        } else {
            for (int j = 0; j < dy_abs; j++) {
                idx = currIdx + count;
                mpTravelInfo[idx] = xDir;
                mpTravelInfo[idx + 1] = yDir;
                count += 2;
            }
            for (int j = 0; j < dx_abs - dy_abs; j++) {
                idx = currIdx + count;
                mpTravelInfo[idx] = xDir;
                count++;
            }
        }

        currIdx += count;
    }

    mpTravelInfo[0] = TRAVEL_DIR_NONE;
    mpTravelInfo[dist + 1] = TRAVEL_DIR_NONE;
}

void daEnSnakeBlock_c::setBlockPos() {
    dBlock_c *prev = &mBlocks[mBlockNum - 2];
    dBlock_c *curr = &mBlocks[mBlockNum - 1];
    mVec3_c tmp;
    for (int i = 0; i < mBlockNum - 1; i++) {
        tmp.x = prev->getPos().x;
        tmp.y = prev->getPos().y;
        tmp.z = 1500.0f;
        curr->setPos(tmp);

        curr--;
        prev--;
    }

    tmp.x = getHeadBlock()->getPos().x;
    tmp.y = getHeadBlock()->getPos().y;
    tmp.z = 1500.0f;
    mBlocks[0].setPos(tmp);
}

void daEnSnakeBlock_c::setActorPos() {
    mPos.x = getHeadBlock()->getPos().x;
    mPos.y = getHeadBlock()->getPos().y;
    mPos.z = 1500.0f;
}

bool daEnSnakeBlock_c::chkCollapseDelete() {
    return getTailBlock()->getPos().y < dBgParameter_c::ms_Instance_p->yEnd() - 16.0f;
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
    getHeadBlock()->setAnmClr("create");
    getHeadBlock()->mAnmClr.setRate(0.0f, 0);
    getHeadBlock()->mAnmClr.setFrame(25.0f, 0);
    getTailBlock()->setAnmClr("ridden");

    mBlocks[0].setAnmClr("create");
    for (int i = 1; i < mBlockNum; i++) {
        dBlock_c &curr = mBlocks[i];
        curr.setAnmClr("ridden");
    }

    getHeadBlock()->mLastPos = getHeadBlock()->mPos;
    getTailBlock()->mLastPos = getTailBlock()->mPos;

    int snakeSpeed = 16.0f / sc_snakeSpeeds[mParam >> 8 & 0x3];

    mCreateAnmBlockIdx = 0;
    mCreateAnmBlockNum = getHeadBlock()->mAnmClr.getFrameMax(0) / snakeSpeed;
}
void daEnSnakeBlock_c::finalizeState_Move() {}
void daEnSnakeBlock_c::executeState_Move() {
    dBlock_c *curr;
    bool b3 = getHeadBlock()->calcPos(mpTravelInfo);
    bool b4 = getTailBlock()->calcPos(mpTravelInfo);
    bool b5 = false;
    bool b6 = false;

    if (b3 || b4) {
        b5 = getHeadBlock()->calcTravelPos(mpTravelInfo);
        b6 = getTailBlock()->calcTravelPos(mpTravelInfo);

        dAudio::SoundEffectID_t(scSnakeSoundID).playMapSound(mPos, 0);

        setBlockPos();
        mCreateAnmBlockIdx++;
        if (mCreateAnmBlockIdx > mCreateAnmBlockNum) {
            mCreateAnmBlockIdx = mCreateAnmBlockNum;
        }

        curr = mBlocks;
        int frame;
        int snakeSpeed = 16.0f / sc_snakeSpeeds[mParam >> 8 & 0x3];
        int i = 0;
        frame = 0;

        for (; i < mCreateAnmBlockIdx; i++) {
            curr->setAnmClr("create");
            curr->mAnmClr.setFrame(frame, 0);
            curr++;
            frame += snakeSpeed;
        }
    }

    if (b5 || b6) {
        if (mpStopState->isEqual(StateID_Stop)) {
            changeState(StateID_Stop);
        } else {
            changeState(StateID_Shake);
        }
    }
}

void daEnSnakeBlock_c::initializeState_Shake() {
    getHeadBlock()->setAnmClr("ridden");
    getHeadBlock()->mAnmClr.setRate(0.0f, 0);
    getTailBlock()->setAnmClr("ridden");
    getTailBlock()->mAnmClr.setRate(0.0f, 0);

    for (int i = 0; i < mBlockNum; i++) {
        dBlock_c &curr = mBlocks[i];
        curr.setAnmClr("ridden");
        curr.mAnmClr.setRate(0.0f, 0);
    }

    mShakeTime = 60;
}
void daEnSnakeBlock_c::finalizeState_Shake() {}
void daEnSnakeBlock_c::executeState_Shake() {
    if (--mShakeTime == 0) {
        changeState(*mpStopState);
    }
}

void daEnSnakeBlock_c::initializeState_Stop() {
    getHeadBlock()->setAnmClr("ridden");
    getHeadBlock()->mAnmClr.setRate(0.0f, 0);
    getTailBlock()->setAnmClr("ridden");
    getTailBlock()->mAnmClr.setRate(0.0f, 0);

    for (int i = 0; i < mBlockNum; i++) {
        dBlock_c &curr = mBlocks[i];
        curr.setAnmClr("ridden");
        curr.mAnmClr.setRate(0.0f, 0);
    }
}
void daEnSnakeBlock_c::finalizeState_Stop() {}
void daEnSnakeBlock_c::executeState_Stop() {}

void daEnSnakeBlock_c::initializeState_Collapse1() {
    static mVec2_c sc_collapseSpeeds[5] = {
        mVec2_c(0.0f, 0.0f),
        mVec2_c(1.0f, 3.0f),
        mVec2_c(0.0f, -1.0f),
        mVec2_c(0.0f, 0.0f),
        mVec2_c(0.0f, 0.0f)
    };

    getHeadBlock()->mLastPos = getHeadBlock()->mPos;
    getTailBlock()->mLastPos = getTailBlock()->mPos;
    for (int i = 0; (int) i < mBlockNum; i++) {
        dBlock_c &curr = mBlocks[i];
        curr.mLastPos = curr.mPos;
    }

    dBlock_c *curr2 = getHeadBlock();
    int prevIdx = curr2->mTravelInfoIdx;
    for (u32 i = 0; (int) i < mBlockNum; i++) { // [fake match]
        curr2 = &mBlocks[i];
        curr2->mTravelInfoIdx = prevIdx;
        prevIdx--;
    }
    getTailBlock()->mTravelInfoIdx = prevIdx;

    int idx = mpTravelInfo[mTravelInfoIdx];
    mVec2_c collapse_speed = sc_collapseSpeeds[idx];
    getHeadBlock()->setSpeed(collapse_speed.x, collapse_speed.y, 0.0f);
    getTailBlock()->setSpeed(collapse_speed.x, collapse_speed.y, 0.0f);
    for (u32 i = 0; (int) i < mBlockNum; i++) { // [fake match]
        dBlock_c &curr = mBlocks[i];
        curr.setSpeed(collapse_speed.x, collapse_speed.y, 0.0f);
    }
}
void daEnSnakeBlock_c::finalizeState_Collapse1() {}
void daEnSnakeBlock_c::executeState_Collapse1() {
    getHeadBlock()->calcCollapse1(mpTravelInfo);
    getTailBlock()->calcCollapse1(mpTravelInfo);
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].calcCollapse1(mpTravelInfo);
    }

    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}

void daEnSnakeBlock_c::initializeState_Collapse2() {
    mCollapse2Idx = 0;
    getHeadBlock()->resetSpeed();
    getTailBlock()->resetSpeed();
    for (int i = 0; i < mBlockNum; i++) {
        dBlock_c &curr = mBlocks[i];
        curr.resetSpeed();
    }
}
void daEnSnakeBlock_c::finalizeState_Collapse2() {}
void daEnSnakeBlock_c::executeState_Collapse2() {
    mCtrlBlock[0].process();

    if (mCollapse2Idx > mBlockNum * 8) {
        mCtrlBlock[1].process();
    }

    int i5 = mCollapse2Idx >> 3;
    if (i5 > mBlockNum - 1) {
        i5 = mBlockNum - 1;
    }

    for (int i = 0; i < i5 + 1; i++) {
        dBlock_c &curr = mBlocks[i];
        curr.mBgCtr.release();
        curr.process();
    }

    mCollapse2Idx++;
    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}

void daEnSnakeBlock_c::initializeState_Collapse3() {
    getHeadBlock()->resetSpeed();
    getTailBlock()->resetSpeed();

    for (int i = 0; i < mBlockNum; i++) {
        dBlock_c &curr = mBlocks[i];
        curr.resetSpeed();
    }
}
void daEnSnakeBlock_c::finalizeState_Collapse3() {}
void daEnSnakeBlock_c::executeState_Collapse3() {
    mCtrlBlock[0].process();
    mCtrlBlock[1].process();

    for (int i = 0; i < mBlockNum; i++) {
        dBlock_c &curr = mBlocks[i];
        if (curr.mBgCtr.m_dc) {
            curr.mBgCtr.release();
        }
        curr.setFallCollapse();
        curr.mPos += curr.mSpeed;
    }

    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}

const int daEnSnakeBlock_c::scSnakeSoundID = SE_OBJ_SNAKE_BLOCK;
