#include <game/bases/d_a_en_shell.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_res_mng.hpp>

STATE_VIRTUAL_DEFINE(daEnShell_c, Carry);
STATE_VIRTUAL_DEFINE(daEnShell_c, Sleep);
STATE_VIRTUAL_DEFINE(daEnShell_c, Slide);
STATE_VIRTUAL_DEFINE(daEnShell_c, Wakeup);
STATE_VIRTUAL_DEFINE(daEnShell_c, WakeupReverse);
STATE_VIRTUAL_DEFINE(daEnShell_c, WakeupTurn);
STATE_VIRTUAL_DEFINE(daEnShell_c, DieFall);

daEnShell_c::daEnShell_c() : m_254(0), m_28c(BASE_ID_NULL), m_2a8(0), m_2ac(0.0f) {
    mFumiProc.refresh(new NonUniqueFumiCheck_c());
    mEatBehaviour = EAT_TYPE_EAT;
    mFlags = 3;
}

daEnShell_c::~daEnShell_c() {
    for (int i = 0; i < PLAYER_COUNT; i++) {
        dAcPy_c *player = daPyMng_c::getPlayer(i);
        if (player != nullptr && fManager_c::searchBaseByID(player->mCarryActorID) == this) {
            player->cancelCarry(this);
        }
    }
}

void daEnShell_c::createShell(const char *s1, const char *s2, const char *s3, const char *s4, float f) {
    mAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20);

    mResFile = dResMng_c::m_instance->getRes(s1, s2);
    nw4r::g3d::ResMdl mdl = mResFile.GetResMdl(s3);
    mModel.create(mdl, &mAllocator, nw4r::g3d::ScnMdl::BUFFER_RESMATMISC | nw4r::g3d::ScnMdl::ANM_TEXPAT);

    dActor_c::setSoftLight_Enemy(mModel);

    nw4r::g3d::ResAnmChr anim;
    nw4r::g3d::ResAnmTexPat animTexPat;
    if (mProfName == fProfile::EN_NOKONOKO || mProfName == fProfile::EN_PATAPATA) {
        anim = mResFile.GetResAnmChr("revival_shell");
        mAnim.create(mdl, anim, &mAllocator);
    }

    if (s4 != nullptr) {
        mResAnmTexPat = mResFile.GetResAnmTexPat(s4);
        mAnimTex.create(mdl, mResAnmTexPat, &mAllocator);
        mAnimTex.setAnm(mModel, mResAnmTexPat, 0, m3d::FORWARD_ONCE);
        mModel.setAnm(mAnimTex);
        mAnimTex.setFrame(f, 0);
        mAnimTex.setRate(0.0f, 0);
    }

    mAllocator.adjustFrmHeap();
}

void daEnShell_c::calcShellMdl() {
    mVec3_c pos = mPos;
    mAng3_c angle = mAngle;
    if (isState(StateID_Carry)) {
        pos = calcCarryPos(m_268);
        mPos.x = pos.x;
        mPos.y = pos.y;
    } else if (m_274) {
        pos.z = 3248.0f;
    }
    changePosAngle(&pos, &angle, 1);
    float shiftY = 0.0f;
    if (m_29c < 96 && m_29c != 0 && mSpeed.y == 0.0f && m_2a0 != 0) {
        shiftY = 16.0f;
    }
    if (m_2a0) {
        if (angle.y >= 0) {
            angle.y = -angle.y.mAngle + 0x8000;
        } else {
            angle.y = -angle.y.mAngle - 0x8000;
        }
    }
    mMatrix.trans(pos.x, pos.y, pos.z);
    mMatrix.YrotM(l_EnMuki[mDirection] * 16384);

    mMatrix.concat(mMtx_c::createTrans(0.0f, 7.0f, 0.0f));
    mMatrix.XrotM(angle.x);
    mMatrix.concat(mMtx_c::createTrans(0.0f, -7.0f, 0.0f));

    mMatrix.concat(mMtx_c::createTrans(0.0f, shiftY, 0.0f));
    mMatrix.XrotM(m_296.x);
    mMatrix.ZrotM(m_296.z);
    mMatrix.concat(mMtx_c::createTrans(0.0f, -shiftY, 0.0f));

    mMatrix.YrotM(l_EnMuki[mDirection ^ 1] * 16384);
    mMatrix.YrotM(angle.y);

    mModel.setLocalMtx(&mMatrix);
    mModel.setScale(mBoyoMng.getScale());
    mModel.calc(false);

    calcShellEffectPos();
}

void daEnShell_c::postExecute(fBase_c::MAIN_STATE_e status) {
    if (status == SUCCESS) {
        if (!isState(StateID_Ice)) {
            if (!mNoRespawn && hasamiCheck()) {
                setDeathInfo_Hasami();
            }
            mVec3_c pos = mPos;
            if (m_258 == 0) {
                pos = getCenterPos();
            }
            WaterCheck(pos, 1.0f);
            if (m_274 && mBc.isFoot()) {
                m_274 = 0;
            }
        }

        if (m_290 > 0) {
            m_290--;
            if (m_290 == 0) {
                dActor_c *actor = (dActor_c *) fManager_c::searchBaseByID(m_28c);
                if (actor != nullptr) {
                    ((int *) actor)[0x54] = 0;
                }
                m_28c = BASE_ID_NULL;
            }
        }
    }

    return dEn_c::postExecute(status);
}

bool daEnShell_c::drawShell() {
    mModel.entry();
    return true;
}

bool daEnShell_c::EtcDamageCheck(dCc_c *self, dCc_c *other) {
    if (m_28c == other->mpOwner->mUniqueID) {
        return false;
    }
    return dEn_c::EtcDamageCheck(self, other);
}

void daEnShell_c::Normal_VsEnHitCheck(dCc_c *self, dCc_c *other) {
    dActor_c *otherActor = other->getOwner();
    if (
        isState(StateID_Carry) &&
        other->mCcData.mVsDamage & (1 << CC_ATTACK_SHELL) &&
        otherActor->mProfName != fProfile::EN_HATENA_BALLOON &&
        hitCallback_Shell(self, other)
    ) {
        other->mInfo |= CC_NO_HIT;
        return;
    }
    if (
        isState(StateID_Slide) &&
        (other->mCcData.mStatus & CC_STATUS_8 &&
        hitCallback_Shell(self, other))
    ) {
        self->mInfo |= CC_NO_HIT;
        return;
    }
    float collOffsetX = mCc.mCollOffsetX[CC_KIND_ENEMY];
    if (
        mDirection == 1 && collOffsetX > 0.0f ||
        mDirection == 0 && collOffsetX < 0.0f
    ) {
        setEnemyTurn();
    }
}
