#include <game/bases/d_a_en_door.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_a_player.hpp>
#include <game/framework/f_profile_name.hpp>
#include <game/mLib/m_effect.hpp>
#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <constants/sound_list.h>

STATE_DEFINE(daEnDoor_c, Search);
STATE_DEFINE(daEnDoor_c, Open);
STATE_DEFINE(daEnDoor_c, Close);
STATE_DEFINE(daEnDoor_c, Wait);
STATE_DEFINE(daEnDoor_c, Dummy);

daObjCenter_c *daEnDoor_c::searchParent_centerA(u32 rotationID) {
    daObjCenter_c *result = nullptr;

    while ((result = (daObjCenter_c *) fManager_c::searchBaseByProfName(fProf::OBJ_CENTER, result))) {
        if (rotationID == result->mRotationID) {
            return result;
        }
    }

    return 0;
}

daObjCenter2_c *daEnDoor_c::searchParent_centerB(u32 rotationID) {
    daObjCenter2_c *result = nullptr;

    while ((result = (daObjCenter2_c *) fManager_c::searchBaseByProfName(fProf::OBJ_CENTER2, result))) {
        if (rotationID == result->mRotationID) {
            return result;
        }
    }

    return 0;
}

dActor_c *daEnDoor_c::searchParent(u32 rotationID) {
    dActor_c *result = searchParent_centerA(rotationID);

    if (result != nullptr) {
        return result;
    }

    return searchParent_centerB(rotationID);
}

void daEnDoor_c::setParentInfo(dActor_c *parent) {
    mParentUniqueID = parent->mUniqueID;
    mDistanceWithParent = mPos - parent->mPos;
}

void daEnDoor_c::rotation_move(dActor_c *actor) {
    mVec3_c pos = actor->mPos;

    s16 parentAngle = actor->mAngle.z;
    parentAngle += 0x4000;
    mAngle.z = parentAngle + mOrientation;

    // [using mAng::cos and mAng::sin breaks the match]
    float parentAngleCos = nw4r::math::CosIdx(parentAngle);
    float parentAngleSin = nw4r::math::SinIdx(parentAngle);

    float distWithParentX = mDistanceWithParent.x;
    float distWithParentY = mDistanceWithParent.y;

    mPos.x = pos.x + ((distWithParentX * parentAngleCos) - (distWithParentY * parentAngleSin));
    mPos.y = pos.y + ((distWithParentX * parentAngleSin) + (distWithParentY * parentAngleCos));
}

dActor_c *daEnDoor_c::rotation_move() {
    dActor_c *result = (dActor_c *) fManager_c::searchBaseByID(mParentUniqueID);

    if (result != nullptr) {
        rotation_move(result);
    }

    return result;
}

dActor_c *daEnDoor_c::setParentInfo(u32 rotationID) {
    dActor_c *result = searchParent(rotationID);
    if (result != nullptr) {
        setParentInfo(result);
        rotation_move(result);

        changeState(StateID_Wait);
    }

    return result;
}

int daEnDoor_c::create() {
    allocate();

    mScale.set(1.0f, 1.0f, 1.0f);
    mCenterOffs.set(0.0f, 24.0f, 0.0f);
    mVisibleAreaOffset.set(0.0f, 24.0f);
    mVisibleAreaSize.set(32.0f, 48.0f);
    mMaxBound.mOffset.set(smc_CULL_XLIMIT, smc_CULL_YLIMIT);

    initCcData();

    mExecStopMask &= ~BIT_FLAG(STAGE_ACTOR_ENEMY);

    initAnm();
    initialize();

    u32 isRotationControlled = ACTOR_PARAM(HasParent);
    mPos.z = 32.0f;
    mParentUniqueID = BASE_ID_NULL;
    mOrientation = ACTOR_PARAM(Orientation) * 0x4000;
    mHasParent = isRotationControlled != 0;

    if (isRotationControlled) {
        mRotationID = ACTOR_PARAM(RotID);
        if (!setParentInfo(mRotationID)) {
            changeState(StateID_Search);
        }
    } else {
        changeState(StateID_Wait);
    }

    return SUCCEEDED;
}

void daEnDoor_c::allocate() {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);
    createMdl();
    mAllocator.adjustFrmHeap();
}

int daEnDoor_c::execute() {
    if (mHasParent) {
        if (mParentUniqueID != BASE_ID_NULL) {
            if (!rotation_move()) {
                deleteActor(0);
                return SUCCEEDED;
            }
        }
    } else if (ActorScrOutCheck(SKIP_NONE)) {
        return SUCCEEDED;
    }

    mStateMgr.executeState();
    calcMdl();
    return SUCCEEDED;
}

int daEnDoor_c::draw() {
    mModel.entry();
    return SUCCEEDED;
}

int daEnDoor_c::doDelete() {
    if (mAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        mAnim.remove();
        mModel.remove();
    }

    return SUCCEEDED;
}

void daEnDoor_c::calcMdl() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    changePosAngle(&pos, &angle, 1);

    mMatrix.trans(pos);
    mMatrix.YrotM(angle.y);
    mMatrix.XrotM(angle.x);
    mMatrix.ZrotM(angle.z);

    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(mScale);
    mModel.calc(false);
}

void daEnDoor_c::ccCallback(dCc_c *self, dCc_c *other) {
    dAcPy_c *otherOwner = (dAcPy_c *) other->mpOwner;
    if (otherOwner->mKind != STAGE_ACTOR_PLAYER) {
        return;
    }

    daEnDoor_c *selfOwner = (daEnDoor_c *) self->mpOwner;
    if (selfOwner->isDummyOpen()) {
        if (otherOwner->isDoorDemoEnable()) {
            if (selfOwner->isClosed()) {
                selfOwner->changeState(StateID_Dummy);
            }

            self->mInfo |= CC_NO_HIT;
        }
    } else if (selfOwner->checkOpenOk() && otherOwner->setDoorDemo(selfOwner)) {
        selfOwner->mMayOpen = true;
        self->mInfo |= CC_NO_HIT;
    }
}

bool daEnDoor_c::checkOpenOk() {
    if (isClosed()) {
        return true;
    }

    if (isState(StateID_Open) && mDoorStatus == daEnDoor_c::DOOR_SUBSTATE_OPEN_OUT) {
        return true;
    }

    return false;
}

void daEnDoor_c::initializeState_Search() {}

void daEnDoor_c::finalizeState_Search() {}

void daEnDoor_c::executeState_Search() {
    setParentInfo(mRotationID);
}

void daEnDoor_c::initializeState_Open() {
    setOpenAnm();
    setOpenSE();
}

void daEnDoor_c::finalizeState_Open() {
    mIsOpen = false;
}

void daEnDoor_c::executeState_Open() {
    mModel.play();

    if (!mAnim.isStop()) {
        return;
    }

    mIsOpen = true;
    if (mDoorStatus != daEnDoor_c::DOOR_SUBSTATE_CLOSED) {
        return;
    }

    changeState(StateID_Close);
}

void daEnDoor_c::initializeState_Dummy() {
    dAudio::g_pSndObjMap->startSound(SE_OBJ_DDOOR_OPEN, mPos, 0);

    mVec3_c pos = getCenterPos();
    pos.z = 2800.0f;

    mEf::createEffect("Wm_en_obakedoor", 0, &pos, nullptr, nullptr);
    dActorMng_c::m_instance->createUpCoin(getCenterPos(), 0, 1, 0);
}

void daEnDoor_c::finalizeState_Dummy() {}

void daEnDoor_c::executeState_Dummy() {
    deleteActor(1);
}

void daEnDoor_c::initializeState_Close() {
    setCloseAnm();
}

void daEnDoor_c::finalizeState_Close() {}

void daEnDoor_c::executeState_Close() {
    mModel.play();

    setCloseMoveSE();

    if (!mAnim.isStop()) {
        return;
    }

    setCloseSE();

    changeState(StateID_Wait);
}

void daEnDoor_c::initializeState_Wait() {
    mDoorStatus = DOOR_SUBSTATE_WAIT;
    mMayOpen = false;
    setWaitAnm();
}

void daEnDoor_c::finalizeState_Wait() {}

void daEnDoor_c::executeState_Wait() {
    mModel.play();
    waitProc();

    if (!mMayOpen) {
        return;
    }

    if (mDoorStatus == DOOR_SUBSTATE_OPEN_OUT || mDoorStatus == DOOR_SUBSTATE_OPEN_IN) {
        changeState(StateID_Open);
    }
}
