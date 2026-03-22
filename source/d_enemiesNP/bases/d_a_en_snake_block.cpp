#include <game/bases/d_a_en_snake_block.hpp>
#include <game/bases/d_rail.hpp>
#include <game/bases/d_s_stage.hpp>

#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <constants/sound_list.h>
#include <game/bases/d_audio.hpp>

ACTOR_PROFILE(EN_SNAKEBLOCK, daEnSnakeBlock_c, 0)

STATE_DEFINE(daEnSnakeBlock_c, Wait);
STATE_DEFINE(daEnSnakeBlock_c, Move);

daEnSnakeBlock_c::daEnSnakeBlock_c() {}
daEnSnakeBlock_c::dBlock_c::dBlock_c() {}
daEnSnakeBlock_c::dCtrlBlock_c::dCtrlBlock_c() {}


static float daEnSnakeBlock_c::sc_snakeSpeeds[3] = {
    0.5f, 1.0f, 1.4f
};

static sStateID_c * daEnSnakeBlock_c::sc_stopStates[4] = {
    StateID_Stop, StateID_Collapse1, StateID_Collapse2, StateID_Collapse3
};

static mVec2_c daEnSnakeBlock_c::sc_ctrlPosMods[5];
static int daEnSnakeBlock_c::sc_glbSnakeNum = 0;
static mVec2_c daEnSnakeBlock_c::sc_collapseSpeeds[5];

int daEnSnakeBlock_c::create() {
    if (sc_glbSnakeNum >= 1 && (((mParam >> 0xC) & 0xFF) != 0)) {
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
    mVec3_c offset = mVec3_c(0.0f, 0.0f, 0.0f);

    if (mShakeTime > 0) {
        offset.x = 0.0f + sc_snakeSpeeds[(mShakeTime >> 1) & 1];
    }

    mHead.draw(offset);
    mTail.draw(offset);
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
    sRailInfoData *rail = dRail_c::getRailInfoP((mParam >> 4) & 0xF);
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);

    sRailNodeData *node = &file->mpRailNodes[rail->mNodeIdx + ((mParam >> 12) & 0xFF)];

    float f1 = node->mX + (mBlockNum * 16.0f);
    float f2 = node->mY - 8.0f;
    mVec3_c head_pos = mVec3_c(f1, f2, 1516.0f);
    mHead.initBgCtr(this, &head_pos, (mParam >> 20) & 1);

    mVec3_c mid_pos;
    for (int i = 0; i < mBlockNum; i++) {
        mid_pos.set(f1, f2, 1500.0f);
        mBlocks[i].initBgCtr(this, &mid_pos, (mParam >> 20) & 1);
        f1 -= 16.0f;
    }

    mVec3_c tail_pos = mVec3_c(f1, f2, 1516.0f);
    mTail.initBgCtr(this, &tail_pos, (mParam >> 20) & 1);

    mHead.mTravelInfoIdx = mBlockNum + 1;
    mTail.mTravelInfoIdx = 1;

    // Note: Value 3 is out of bounds
    mHead.mSnakeSpeedIdx = (mParam >> 8) & 3;
    mTail.mSnakeSpeedIdx = (mParam >> 8) & 3;
}

void daEnSnakeBlock_c::calcAnm() {
    mHead.calcAnm();
    mTail.calcAnm();

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].calcAnm();
    }
}

void daEnSnakeBlock_c::createMdl() {
    mHead.createMdl(&mAllocator);
    mTail.createMdl(&mAllocator);

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].createMdl(&mAllocator);
    }
}

void daEnSnakeBlock_c::deleteBlock() {
    mHead.doDelete();
    mTail.doDelete();

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].doDelete();
    }
}

void daEnSnakeBlock_c::setStopState() {
    mpStopState = &sc_stopStates[mParam >> 0x1c];
}

void daEnSnakeBlock_c::calcBgCtr() {
    mHead.calcBgCtr();
    mTail.calcBgCtr();

    mTail.mBgCtr.mFlags |= 1;

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].calcBgCtr();
    }
}

void daEnSnakeBlock_c::initTravelInfo() {
    sRailInfoData *rail = dRail_c::getRailInfoP((mParam >> 4) & 0xF);
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);

    sRailNodeData *node = &file->mpRailNodes[rail->mNodeIdx + ((mParam >> 12) & 0xFF)];

    u32 dist = 0;
    u32 start_idx = (mParam >> 12) & 0xFF;
    for (int i = start_idx; i < (rail->mCount - 1); i++) {
        sRailNodeData * next_node = node + 1;
        s16 dx = abs((next_node->mX >> 4) - (node->mX >> 4));
        s16 dy = abs((next_node->mY >> 4) - (node->mY >> 4));
        dist += dx + dy;
        node = next_node;
    }

    mTravelInfoIdx = dist;
    mpTravelInfo = new u8[dist + 2];

    int i11 = 1;
    for (; start_idx < rail->mCount; start_idx++) {
        sRailNodeData * next_node = node + 1;
        s16 i8 = (next_node->mX >> 4) - (node->mX >> 4);
        s16 i7 = (next_node->mY >> 4) - (node->mY >> 4);

        s32 i4 = abs(i8);
        s32 i13 = (s16)i4;
        i4 = abs(i7);
        i4 = (s16)i4;

        u8 b5 = 0;
        u8 b6 = 0;

        if (i8 < 1) {
            if (i8 < 0) {
                b5 = 3;
            }
        } else {
            b5 = 4;
        }

        if (i7 < 0) {
            b6 = 1;
        } else if (i7 > 0) {
            b6 = 2;
        }

        i7 = 0;
        if (i4 < i13) {
            i8 = 0;
            if (i4 > 0) {
                if (i4 > 8) {
                    bool b2 = false;
                    if ((i4 > -1) && (i4 < 0x7FFFFFFF)) {
                        b2 = true;
                    }

                    if (b2) {
                        u32 u15 = ((u32)(i4 - 1)) >> 3;

                        if (u15 > 0) {
                            do {
                                u32 idx = i11 + i7;
                                i7 += 0x10;
                                i8 += 8;

                                mpTravelInfo[idx    ] = b5;
                                mpTravelInfo[idx + 1] = b5;
                                mpTravelInfo[idx + 2] = b5;
                                mpTravelInfo[idx + 3] = b5;
                                mpTravelInfo[idx + 4] = b5;
                                mpTravelInfo[idx + 5] = b5;
                                mpTravelInfo[idx + 6] = b5;
                                mpTravelInfo[idx + 7] = b5;
                                mpTravelInfo[idx + 8] = b5;
                                mpTravelInfo[idx + 9] = b5;
                                mpTravelInfo[idx + 10] = b5;
                                mpTravelInfo[idx + 11] = b5;
                                mpTravelInfo[idx + 12] = b5;
                                mpTravelInfo[idx + 13] = b5;
                                mpTravelInfo[idx + 14] = b5;
                                mpTravelInfo[idx + 15] = b5;

                                u15--;
                            } while (u15 != 0);
                        }
                    }
                    int i9 = i4 - i8;
                    if (i9 > 0) {
                        while (i9 != 0) {
                            int i8 = i11 + i7;
                            i7 += 2;
                            mpTravelInfo[i8] = b5;
                            mpTravelInfo[i8 + 1] = b5;
                        }
                    }
                }
                i13 -= i4;
                i4 = 0;
                if (i13 > 0) {
                    if (i13 > 8) {
                        bool b2 = false;
                        if ((i13 > -1) && (i13 < 0x7FFFFFFF)) {
                            b2 = true;
                        }
                        if (b2) {
                            int u15 = (i13 - 1) >> 3;
                            if (u15 > 0) {
                                while (u15 != 0) {
                                    int i8 = i11 + i7;
                                    i7 += 8;
                                    i4 += 8;
                                    mpTravelInfo[i8] = b5;
                                    mpTravelInfo[i8 + 1] = b5;
                                    mpTravelInfo[i8 + 2] = b5;
                                    mpTravelInfo[i8 + 3] = b5;
                                    mpTravelInfo[i8 + 4] = b5;
                                    mpTravelInfo[i8 + 5] = b5;
                                    mpTravelInfo[i8 + 6] = b5;
                                    mpTravelInfo[i8 + 7] = b5;
                                    u15--;
                                }
                            }
                        }
                    }
                    i8 = i13 - i4;
                    if (i4 < i13) {
                        do {
                            i4 = i11 + i7;
                            i7++;
                            mpTravelInfo[i4] = b5;
                            i8--;
                        } while (i8 != 0);
                    }
                }
            }
        } else {
            i8 = 0;
            if (i13 > 0) {
                if (i13 > 8) {
                    bool b2 = false;
                    if ((i13 > -1) && (i13 < 0x7FFFFFFF)) {
                        b2 = true;
                    }
                    if (b2) {
                        u32 u15 = ((u32)(i13 - 1)) >> 3;
                        if (u15 > 0) {
                            do {
                                int i9 = i11 + i7;
                                i7 += 16;
                                i8 += 8;
                                mpTravelInfo[i9   ] = b5;
                                mpTravelInfo[i9 + 1] = b5;
                                mpTravelInfo[i9 + 2] = b5;
                                mpTravelInfo[i9 + 3] = b5;
                                mpTravelInfo[i9 + 4] = b5;
                                mpTravelInfo[i9 + 5] = b5;
                                mpTravelInfo[i9 + 6] = b5;
                                mpTravelInfo[i9 + 7] = b5;
                                mpTravelInfo[i9 + 8] = b5;
                                mpTravelInfo[i9 + 9] = b5;
                                mpTravelInfo[i9 + 10] = b5;
                                mpTravelInfo[i9 + 11] = b5;
                                mpTravelInfo[i9 + 12] = b5;
                                mpTravelInfo[i9 + 13] = b5;
                                mpTravelInfo[i9 + 14] = b5;
                                mpTravelInfo[i9 + 15] = b5;
                                u15--;
                            } while (u15 != 0);
                        }
                    }
                    int i9 = i13 - i8;
                    if (i8 < i13) {
                        while (i9 != 0) {
                            i8 = i11 + i7;
                            i7 += 2;
                            mpTravelInfo[i8] = b5;
                            mpTravelInfo[i8 + 1] = b5;
                            i9--;
                        }
                    }
                }
            }
            i4 -= i13;
            i13 = 0;
            if (i4 > 0) {
                if (i4 > 8) {
                    bool b2 = false;
                    if ((i4 > -1) && (i4 < 0x7FFFFFFF)) {
                        b2 = true;
                    }

                    if (b2) {
                        u32 u15 = (u32)(i4 - 1) >> 3;
                        if (u15 > 0) {
                            while (u15 != 0) {
                                i8 = i11 + i7;
                                i7 += 8;
                                i13 += 8;
                                mpTravelInfo[i8] = b6;
                                mpTravelInfo[i8 + 1] = b6;
                                mpTravelInfo[i8 + 2] = b6;
                                mpTravelInfo[i8 + 3] = b6;
                                mpTravelInfo[i8 + 4] = b6;
                                mpTravelInfo[i8 + 5] = b6;
                                mpTravelInfo[i8 + 6] = b6;
                                mpTravelInfo[i8 + 7] = b6;
                                u15--;
                            }
                        }
                    }
                }

                i8 = i4 - i13;
                if (i8 > 0) {
                    while (i8 != 0) {
                        i4 = i11 + i7;
                        i7++;
                        i8--;
                        mpTravelInfo[i4] = b6;
                    }
                }
            }
        }

        node += 1;
        i11 += i7;
    }

    mpTravelInfo[0] = 0;
    mpTravelInfo[dist + 1] = 0;
}

void daEnSnakeBlock_c::setBlockPos() {
    for (int i = 0; i < mBlockNum - 1; i++) {
        dBlock_c &a = mBlocks[mBlockNum - 1 - i];
        dBlock_c &b = mBlocks[mBlockNum - 2 - i];

        b.mPos = a.mPos;
    }

    mBlocks[0].mPos = mVec3_c(mHead.mPos, 1500.0f);
}

void daEnSnakeBlock_c::setActorPos() {
    mPos = mVec3_c(mHead.mPos, 1500.0f);
}

bool daEnSnakeBlock_c::chkCollapseDelete() {
    return mTail.mPos.y < (dBgParameter_c::ms_Instance_p->mPos.y - dBgParameter_c::ms_Instance_p->mSize.y) - 16.0f;
}

bool daEnSnakeBlock_c::chkOffScreen() {
    if (mStateMgr.getStateID() == &StateID_Wait) {
        return false;
    }
    return (
        dBgParameter_c::ms_Instance_p->mPos.x +
        dBgParameter_c::ms_Instance_p->mSize.x + 512.0f
        <= mPos.x
    );
}

void daEnSnakeBlock_c::initializeState_Wait() {}
void daEnSnakeBlock_c::finalizeState_Wait() {}
void daEnSnakeBlock_c::executeState_Wait() {}

void daEnSnakeBlock_c::initializeState_Move() {
    mHead.setAnmClr("create");
    mHead.mAnmClr.setRate(0.0f, 0);
    mHead.mAnmClr.setFrame(25.0f, 0);
    mTail.setAnmClr("ridden");

    mBlocks[0].setAnmClr("create");
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].setAnmClr("ridden");
    }

    mHead.mLastPos = mHead.mPos;
    mTail.mLastPos = mTail.mPos;

    mCreateAnmBlockIdx = 0;
    float f = mHead.mAnmClr.getFrameMax(0);
    mCreateAnmBlockNum = f / (16.0f / sc_snakeSpeeds[mParam >> 8 & 0xF]);
}
void daEnSnakeBlock_c::finalizeState_Move() {}
void daEnSnakeBlock_c::executeState_Move() {
    bool b3 = mHead.calcPos(mpTravelInfo);
    bool b4 = mTail.calcPos(mpTravelInfo);
    bool b5 = false;
    bool b6 = false;

    if (b3 || b4) {
        b5 = mHead.calcTravelPos(mpTravelInfo);
        b6 = mTail.calcTravelPos(mpTravelInfo);

        dAudio::SoundEffectID_t(SE_OBJ_SNAKE_BLOCK).playMapSound(mPos, 0);

        setBlockPos();
        mCreateAnmBlockIdx++;
        if (mCreateAnmBlockNum < mCreateAnmBlockIdx) {
            mCreateAnmBlockIdx = mCreateAnmBlockNum;
        }

        int frame_idx = 0;
        int frame_delta = (int)(16.0f / sc_snakeSpeeds[(mParam >> 8) & 0xF]);

        for (int i = 0; i < mCreateAnmBlockIdx; i++) {
            mBlocks[i].setAnmClr("create");
            mBlocks[i].mAnmClr.setFrame(frame_idx, 0);
            frame_idx += frame_delta;
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
    mHead.setAnmClr("ridden");
    mHead.mAnmClr.setRate(0.0f, 0);
    mTail.setAnmClr("ridden");
    mTail.mAnmClr.setRate(0.0f, 0);

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].setAnmClr("ridden");
        mBlocks[i].mAnmClr.setRate(0.0f, 0);
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
    mHead.setAnmClr("ridden");
    mHead.mAnmClr.setRate(0.0f, 0);
    mTail.setAnmClr("ridden");
    mTail.mAnmClr.setRate(0.0f, 0);

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].setAnmClr("ridden");
        mBlocks[i].mAnmClr.setRate(0.0f, 0);
    }
}
void daEnSnakeBlock_c::finalizeState_Stop() {}
void daEnSnakeBlock_c::executeState_Stop() {}

void daEnSnakeBlock_c::initializeState_Collapse1() {
    static float floats[] = {
        0.0f, 0.0f, 1.0f, 3.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    mHead.mLastPos = mHead.mPos;
    mTail.mLastPos = mTail.mPos;
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mLastPos = mBlocks[i].mPos;
    }
    int prev_idx = mHead.mTravelInfoIdx;
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mTravelInfoIdx = prev_idx;
        prev_idx--;
    }
    mTail.mTravelInfoIdx = prev_idx;

    mVec3_c collapse_speed = mVec3_c(sc_collapseSpeeds[mpTravelInfo[mTravelInfoIdx]], 0.0f);
    mHead.mSpeed = collapse_speed;
    mTail.mSpeed = collapse_speed;
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mSpeed = collapse_speed;
    }
}
void daEnSnakeBlock_c::finalizeState_Collapse1() {}
void daEnSnakeBlock_c::executeState_Collapse1() {
    mHead.calcCollapse1(mpTravelInfo);
    mTail.calcCollapse1(mpTravelInfo);
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].calcCollapse1(mpTravelInfo);
    }

    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}

void daEnSnakeBlock_c::initializeState_Collapse2() {
    mCollapse2Idx = 0;
    mHead.mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    mTail.mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    }
}
void daEnSnakeBlock_c::finalizeState_Collapse2() {}
void daEnSnakeBlock_c::executeState_Collapse2() {
    if (mHead.mBgCtr.m_dc) {
        mHead.mBgCtr.release();
    }
    mHead.setFallCollapse();
    mHead.mPos += mHead.mSpeed;

    if (mCollapse2Idx > mBlockNum * 8) {
        if (mTail.mBgCtr.m_dc) {
            mTail.mBgCtr.release();
        }
        mTail.setFallCollapse();
        mTail.mPos += mTail.mSpeed;
    }

    int i5 = mCollapse2Idx >> 3;
    int i6 = mBlockNum - 1;
    if (i5 > i6) {
        i5 = i6;
    }

    for (int i = 0; i <= i5; i++) {
        dBlock_c * block = &mBlocks[i];
        block->mBgCtr.release(); //< double release?
        if (block->mBgCtr.m_dc) {
            block->mBgCtr.release();
        }
        block->setFallCollapse();
        block->mPos += block->mSpeed;
    }

    mCollapse2Idx++;
    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}

void daEnSnakeBlock_c::initializeState_Collapse3() {
    mHead.mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    mTail.mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    }
}
void daEnSnakeBlock_c::finalizeState_Collapse3() {}
void daEnSnakeBlock_c::executeState_Collapse3() {
    if (mHead.mBgCtr.m_dc) {
        mHead.mBgCtr.release();
    }
    mHead.setFallCollapse();
    mHead.mPos += mHead.mSpeed;

    if (mTail.mBgCtr.m_dc) {
        mTail.mBgCtr.release();
    }
    mTail.setFallCollapse();
    mTail.mPos += mTail.mSpeed;

    for (int i = 0; i < mBlockNum; i++) {
        if (mBlocks[i].mBgCtr.m_dc) {
            mBlocks[i].mBgCtr.release();
        }
        mBlocks[i].setFallCollapse();
        mBlocks[i].mPos += mBlocks[i].mSpeed;
    }

    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}
