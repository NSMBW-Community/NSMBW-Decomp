#include <game/bases/d_bg.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_awa.hpp>

ACTOR_PROFILE(AWA, dAwa_c, 2);

// [There are a few float constants that must appear here.]
DECL_WEAK void DUMMY_ORDERING() {
    static const float unused[] = { 0.1f, 2.0f };
}

int dAwa_c::create() {

    static const dAwa_c::CalcSpeed Proc_tbl[] = {
        &dAwa_c::calcSpeedUp,
        &dAwa_c::calcSpeedDown,
        &dAwa_c::calcSpeedRight,
        &dAwa_c::calcSpeedLeft
    };

    createModel();
    setAnim();

    u8 dir = ACTOR_PARAM(Direction);

    mScale.set(1.0f, 1.0f, 1.0f);
    mSpeed.set(0.0f, 0.0f, 0.0f);
    mCalcSpeedFn = Proc_tbl[dir];

    return SUCCEEDED;
}

void dAwa_c::createModel() {
    mAllocator.createFrmHeap(0x80000, mHeap::g_gameHeaps[0], nullptr, 0x20);
    const char* fileName = "g3d/waterbubble.brres";
    mResFile = dResMng_c::m_instance->getRes("waterbubble", fileName);

    nw4r::g3d::ResMdl model = mResFile.GetResMdl("waterbubble");
    mBubbleModel.create(model, &mAllocator, 0x20, 1, nullptr);
    dActor_c::setSoftLight_MapObj(mBubbleModel);

    nw4r::g3d::ResAnmChr anim = mResFile.GetResAnmChr("waterbubble");
    mBubbleAnim.create(model, anim, &mAllocator, nullptr);
    mAllocator.adjustFrmHeap();
}

int dAwa_c::execute() {
    mLifeTimer--;

    if (mLifeTimer == 0 || mPos.y >= dBg_c::m_bg_p->getLiquidHeight()) {
        deleteRequest();
    } else {
        calc();
    }

    return SUCCEEDED;
}

int dAwa_c::draw() {
    mVec3_c pos = mPos;
    changePosAngle(&pos, nullptr, 1);
    mMatrix.trans(pos.x, pos.y, pos.z);
    mBubbleModel.setLocalMtx(&mMatrix);
    mBubbleModel.setScale(mScale);
    mBubbleModel.calc(false);
    mBubbleModel.entry();
    return SUCCEEDED;
}

void dAwa_c::deleteReady() {}

int dAwa_c::doDelete() {
    if (mAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        mBubbleAnim.remove();
        mBubbleModel.remove();
    }

    return SUCCEEDED;
}

void dAwa_c::setAnim() {
    nw4r::g3d::ResAnmChr anim = mResFile.GetResAnmChr("waterbubble");
    mBubbleAnim.setAnm(mBubbleModel, anim, m3d::FORWARD_LOOP);
    mBubbleModel.setAnm(mBubbleAnim, 0.0f);
    mBubbleAnim.setRate(1.0f);
}

void dAwa_c::calc() {
    mBubbleModel.play();
    (this->*mCalcSpeedFn)();
    posMove();
}

void dAwa_c::calcSpeedUp() {
    mSpeed.y += 0.1f;
    if (mSpeed.y > 2.0f) {
        mSpeed.y = 2.0f;
    }
}

void dAwa_c::calcSpeedDown() {
    mSpeed.y -= 0.1f;
    if (mSpeed.y < -2.0f) {
        mSpeed.y = -2.0f;
    }
}

void dAwa_c::calcSpeedRight() {
    mSpeed.x += 0.1f;
    if (mSpeed.x > 2.0f) {
        mSpeed.x = 2.0f;
    }
}

void dAwa_c::calcSpeedLeft() {
    mSpeed.x -= 0.1f;
    if (mSpeed.x > -2.0f) {
        mSpeed.x = -2.0f;
    }
}