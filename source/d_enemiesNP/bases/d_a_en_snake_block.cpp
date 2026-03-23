#include <game/bases/d_a_en_snake_block.hpp>
#include <game/bases/d_rail.hpp>
#include <game/bases/d_s_stage.hpp>

#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_bg_parameter.hpp>
#include <constants/sound_list.h>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_audio.hpp>

ACTOR_PROFILE(EN_SNAKEBLOCK, daEnSnakeBlock_c, 0);

int daEnSnakeBlock_c::sc_glbSnakeNum = 0;

mVec2_c daEnSnakeBlock_c::sc_ctrlPosMods[5] = {
    mVec2_c(0.0f, 0.0f),
    mVec2_c(0.0f, 1.0f),
    mVec2_c(0.0f, -1.0f),
    mVec2_c(-1.0f, 0.0f),
    mVec2_c(1.0f, 0.0f)
};

float daEnSnakeBlock_c::sc_snakeSpeeds[3] = {
    0.5f, 1.0f, 1.4f
};

STATE_DEFINE(daEnSnakeBlock_c, Wait);
STATE_DEFINE(daEnSnakeBlock_c, Move);
STATE_DEFINE(daEnSnakeBlock_c, Shake);
STATE_DEFINE(daEnSnakeBlock_c, Collapse1);
STATE_DEFINE(daEnSnakeBlock_c, Collapse2);
STATE_DEFINE(daEnSnakeBlock_c, Collapse3);
STATE_DEFINE(daEnSnakeBlock_c, Stop);

void daEnSnakeBlock_c::dBlock_c::createMdl(dHeapAllocator_c *alloc) {
    mResFile = dResMng_c::m_instance->getRes("block_snake_ice", "g3d/block_snake_ice.brres");

    nw4r::g3d::ResMdl mdl = mResFile.GetResMdl("block_snake_ice");
    mModel.create(mdl, alloc, 0x32c, 1);
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
    mSpeed.y += -0.1875f;
    if (mSpeed.y < -4.0f) {
        mSpeed.y = -4.0f;
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

    if (mBgCtr.m_e2) {
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

bool daEnSnakeBlock_c::dCtrlBlock_c::calcPos(s8 *travelInfo) {
    // int inf = travelInfo[mTravelInfoIdx];
    float speed = sc_snakeSpeeds[mSnakeSpeedIdx];
    mVec2_c v(
        speed * sc_ctrlPosMods[travelInfo[mTravelInfoIdx]].x,
        speed * sc_ctrlPosMods[travelInfo[mTravelInfoIdx]].y
    );
    mPos.x += v.x;
    mPos.y += v.y;

    if (std::fabs(mPos.x - mLastPos.x) >= 16.0f || std::fabs(mPos.y - mLastPos.y) >= 16.0f) {
        return true;
    }
    return false;
}

bool daEnSnakeBlock_c::dCtrlBlock_c::calcTravelPos(s8 *travelInfo) {
    mVec2_c newPos;
    newPos.x = sc_ctrlPosMods[travelInfo[mTravelInfoIdx]].x;
    newPos.y = sc_ctrlPosMods[travelInfo[mTravelInfoIdx]].y;

    float x = mLastPos.x + newPos.x * 16.0f;
    float y = mLastPos.y + newPos.y * 16.0f;

    mLastPos.z = mPos.z;

    mPos.x = x;
    mPos.y = y;
    mLastPos.x = x;
    mLastPos.y = y;

    return travelInfo[++mTravelInfoIdx] == 0;
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
        offset.x += sc_snakeSpeeds[(mShakeTime >> 1) & 1];
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
    sRailInfoData *rail = dRail_c::getRailInfoP((mParam >> 4) & 0xF);
    dCdFile_c *file = dCd_c::m_instance->getFileP(dScStage_c::m_instance->mCurrFile);

    sRailNodeData *node = &file->mpRailNodes[rail->mNodeIdx + ((mParam >> 12) & 0xFF)];

    float f1 = node->mX + (mBlockNum * 16.0f);
    float f2 = node->mY - 8.0f;
    mVec3_c head_pos = mVec3_c(f1, f2, 1516.0f);
    getHeadBlock()->initBgCtr(this, head_pos, (mParam >> 20) & 1);

    mVec3_c mid_pos;
    for (int i = 0; i < mBlockNum; i++) {
        mid_pos.set(f1, f2, 1500.0f);
        mBlocks[i].initBgCtr(this, mid_pos, (mParam >> 20) & 1);
        f1 -= 16.0f;
    }

    mVec3_c tail_pos = mVec3_c(f1, f2, 1516.0f);
    getTailBlock()->initBgCtr(this, tail_pos, (mParam >> 20) & 1);

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
    mpTravelInfo = new s8[dist + 2];

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

    mBlocks[0].mPos = mVec3_c(getHeadBlock()->mPos, 1500.0f);
}

void daEnSnakeBlock_c::setActorPos() {
    mPos = mVec3_c(getHeadBlock()->mPos, 1500.0f);
}

bool daEnSnakeBlock_c::chkCollapseDelete() {
    return getTailBlock()->mPos.y < (dBgParameter_c::ms_Instance_p->mPos.y - dBgParameter_c::ms_Instance_p->mSize.y) - 16.0f;
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
    getHeadBlock()->setAnmClr("create");
    getHeadBlock()->mAnmClr.setRate(0.0f, 0);
    getHeadBlock()->mAnmClr.setFrame(25.0f, 0);
    getTailBlock()->setAnmClr("ridden");

    mBlocks[0].setAnmClr("create");
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].setAnmClr("ridden");
    }

    getHeadBlock()->mLastPos = getHeadBlock()->mPos;
    getTailBlock()->mLastPos = getTailBlock()->mPos;

    mCreateAnmBlockIdx = 0;
    float f = getHeadBlock()->mAnmClr.getFrameMax(0);
    mCreateAnmBlockNum = f / (16.0f / sc_snakeSpeeds[mParam >> 8 & 0xF]);
}
void daEnSnakeBlock_c::finalizeState_Move() {}
void daEnSnakeBlock_c::executeState_Move() {
    bool b3 = getHeadBlock()->calcPos(mpTravelInfo);
    bool b4 = getTailBlock()->calcPos(mpTravelInfo);
    bool b5 = false;
    bool b6 = false;

    if (b3 || b4) {
        b5 = getHeadBlock()->calcTravelPos(mpTravelInfo);
        b6 = getTailBlock()->calcTravelPos(mpTravelInfo);

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
    getHeadBlock()->setAnmClr("ridden");
    getHeadBlock()->mAnmClr.setRate(0.0f, 0);
    getTailBlock()->setAnmClr("ridden");
    getTailBlock()->mAnmClr.setRate(0.0f, 0);

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
    getHeadBlock()->setAnmClr("ridden");
    getHeadBlock()->mAnmClr.setRate(0.0f, 0);
    getTailBlock()->setAnmClr("ridden");
    getTailBlock()->mAnmClr.setRate(0.0f, 0);

    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].setAnmClr("ridden");
        mBlocks[i].mAnmClr.setRate(0.0f, 0);
    }
}
void daEnSnakeBlock_c::finalizeState_Stop() {}
void daEnSnakeBlock_c::executeState_Stop() {}

void daEnSnakeBlock_c::initializeState_Collapse1() {
    static mVec2_c sc_collapseSpeeds[5] = {
        mVec2_c(0.0f, 0.0f),
        mVec2_c(0.0f, 0.0f),
        mVec2_c(0.0f, 0.0f),
        mVec2_c(0.0f, 0.0f),
        mVec2_c(0.0f, 0.0f)
    };

    static float floats[] = {
        0.0f, 0.0f, 1.0f, 3.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f
    };

    getHeadBlock()->mLastPos = getHeadBlock()->mPos;
    getTailBlock()->mLastPos = getTailBlock()->mPos;
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mLastPos = mBlocks[i].mPos;
    }
    int prev_idx = getHeadBlock()->mTravelInfoIdx;
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mTravelInfoIdx = prev_idx;
        prev_idx--;
    }
    getTailBlock()->mTravelInfoIdx = prev_idx;

    mVec3_c collapse_speed = mVec3_c(sc_collapseSpeeds[mpTravelInfo[mTravelInfoIdx]], 0.0f);
    getHeadBlock()->mSpeed = collapse_speed;
    getTailBlock()->mSpeed = collapse_speed;
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mSpeed = collapse_speed;
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
    getHeadBlock()->mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    getTailBlock()->mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    }
}
void daEnSnakeBlock_c::finalizeState_Collapse2() {}
void daEnSnakeBlock_c::executeState_Collapse2() {
    if (getHeadBlock()->mBgCtr.m_e2) {
        getHeadBlock()->mBgCtr.release();
    }
    getHeadBlock()->setFallCollapse();
    getHeadBlock()->mPos += getHeadBlock()->mSpeed;

    if (mCollapse2Idx > mBlockNum * 8) {
        if (getTailBlock()->mBgCtr.m_e2) {
            getTailBlock()->mBgCtr.release();
        }
        getTailBlock()->setFallCollapse();
        getTailBlock()->mPos += getTailBlock()->mSpeed;
    }

    int i5 = mCollapse2Idx >> 3;
    int i6 = mBlockNum - 1;
    if (i5 > i6) {
        i5 = i6;
    }

    for (int i = 0; i <= i5; i++) {
        dBlock_c * block = &mBlocks[i];
        block->mBgCtr.release(); //< double release?
        if (block->mBgCtr.m_e2) {
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
    getHeadBlock()->mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    getTailBlock()->mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < mBlockNum; i++) {
        mBlocks[i].mSpeed = mVec3_c(0.0f, 0.0f, 0.0f);
    }
}
void daEnSnakeBlock_c::finalizeState_Collapse3() {}
void daEnSnakeBlock_c::executeState_Collapse3() {
    if (getHeadBlock()->mBgCtr.m_e2) {
        getHeadBlock()->mBgCtr.release();
    }
    getHeadBlock()->setFallCollapse();
    getHeadBlock()->mPos += getHeadBlock()->mSpeed;

    if (getTailBlock()->mBgCtr.m_e2) {
        getTailBlock()->mBgCtr.release();
    }
    getTailBlock()->setFallCollapse();
    getTailBlock()->mPos += getTailBlock()->mSpeed;

    for (int i = 0; i < mBlockNum; i++) {
        if (mBlocks[i].mBgCtr.m_e2) {
            mBlocks[i].mBgCtr.release();
        }
        mBlocks[i].setFallCollapse();
        mBlocks[i].mPos += mBlocks[i].mSpeed;
    }

    if (chkCollapseDelete()) {
        deleteActor(1);
    }
}
