#include <game/bases/d_enemy_boss.hpp>
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>
#include <game/bases/d_actor_manager.hpp>
#include <game/bases/d_quake.hpp>
#include <game/sLib/s_GlobalData.hpp>

template<>
const dEnBoss_c::GlobalData_t sGlobalData_c<dEnBoss_c>::mData = {
    false
};

STATE_VIRTUAL_DEFINE(dEnBoss_c, DemoWait);
STATE_VIRTUAL_DEFINE_INH(dEnBoss_c, DieFumi, sStateID_c); // [Why is this not properly inherited?]
STATE_VIRTUAL_DEFINE(dEnBoss_c, DieFire);
STATE_VIRTUAL_DEFINE(dEnBoss_c, DieSlide);
STATE_VIRTUAL_DEFINE(dEnBoss_c, DieShell);
STATE_VIRTUAL_DEFINE(dEnBoss_c, DieStar);
STATE_VIRTUAL_DEFINE(dEnBoss_c, DieQuake);

dEnBoss_c::dEnBoss_c() : mTenmetsuTime(0) {
    m_d0 = 0;
    mQuakeDamage = 0;
    mpBossLife = nullptr;
}

dEnBoss_c::~dEnBoss_c() {}

int dEnBoss_c::create() {
    allocate();
    mFlags = EN_FLAG_4;
    createInit();
    mActorProperties |= 0x200;
    changeState(StateID_DemoWait);
    mStateMgr.refreshState();
    return SUCCEEDED;
}

int dEnBoss_c::createInit() {
    return 1;
}

void dEnBoss_c::allocate() {
    mAllocator.createFrmHeapToCurrent(-1, mHeap::g_gameHeaps[mHeap::GAME_HEAP_DEFAULT], nullptr, 0x20, mHeap::OPT_NONE);
    createModel();
    createBossLife();
    mAllocator.adjustFrmHeapRestoreCurrent();
}

void dEnBoss_c::createBossLife() {
    if (mpBossLife == nullptr) {
        mpBossLife = new dBossLife_Common_c(18);
    }
}

int dEnBoss_c::preExecute() {
    if (dEn_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    if (mTenmetsuTime > 0) {
        mTenmetsuTime--;
        if (mTenmetsuTime > 0) {
            tenmetsuProc();
        } else {
            tenmetsuFin();
        }
    }

    if (mQuakeDamage && !(dQuake_c::m_instance->mFlags & dQuake_c::FLAG_1)) {
        mQuakeDamage = 0;
    }

    return SUCCEEDED;
}

void dEnBoss_c::postExecute(MAIN_STATE_e state) {
    if (state == SUCCESS) {
        mSndObject.mPos = dAudio::cvtSndObjctPos(mPos);
        mSndObject.calc();
    }
    dEn_c::postExecute(state);
}

bool dEnBoss_c::isQuakeDamage() {
    if (mQuakeDamage) {
        return false;
    }
    if (isState(StateID_DemoWait)) {
        return false;
    }
    return mBc.isFoot();
}

void dEnBoss_c::Normal_VsPlHitCheck(dCc_c *self, dCc_c *other) {
    if (isFumiInvalid()) {
        dEn_c::Normal_VsPlHitCheck(self, other);
        return;
    }

    daPlBase_c *player = (daPlBase_c *) other->getOwner();

    switch (Enfumi_check(self, other, 2)) {
        case 1:
        case 3:
            if (!isFumiDmgInvalid()) {
                fumiProc(other->getOwner());
            }
            break;
        case 0:
            dEn_c::Normal_VsPlHitCheck(self, other);
            if (player->isStatus(0x3F)) {
                int plrNo = *player->getPlrNo();
                if (plrNo >= 0 && plrNo < PLAYER_COUNT) {
                    mNoHitPlayer.mTimer[plrNo] = 16;
                }
            }
            break;
    }
}

void dEnBoss_c::Normal_VsYoshiHitCheck(dCc_c *self, dCc_c *other) {
    if (isFumiInvalid()) {
        dEn_c::Normal_VsYoshiHitCheck(self, other);
        return;
    }

    switch (Enfumi_check(self, other, 0)) {
        case 1:
            if (!isFumiDmgInvalid()) {
                fumiProc(other->getOwner());
            }
            break;
        case 0:
            dEn_c::Normal_VsPlHitCheck(self, other);
            break;
    }
}

void dEnBoss_c::deadAllKill() {
    dActorMng_c::m_instance->allEnemyDeath(0);
}

void dEnBoss_c::fumiProc(dActor_c *actor) {
    if (sGlobalData_c<dEnBoss_c>::mData.mInstantKill) {
        mpBossLife->mLife = 0;
    } else {
        mpBossLife->updateFumi();
    }

    getTenmetsuTimePress(false);
    if (mpBossLife->mLife <= 0) {
        fumideadEffect();
        fumideadSE();
        fumiDeadVo();
        mpBossLife->mLife = 0;
        deadAllKill();
        daPyDemoMng_c::mspInstance->setBossDown((daPlBase_c *) actor);
        setFumiDead(actor);
        deadProc();
    } else {
        fumidmgEffect();
        fumidmgSE();
        setFumiDamage(actor);
        damageProc();
    }
}

bool dEnBoss_c::hitCallback_Fire(dCc_c *self, dCc_c *other) {
    if (isFireInvalid()) {
        fireballInvalid(self, other);
        return true;
    }

    dActor_c *player = other->getOwner();
    int prev2 = getLife();
    int prev = mpBossLife->updateFire();

    if (sGlobalData_c<dEnBoss_c>::mData.mInstantKill) {
        mpBossLife->mLife = 0;
    }

    if (prev >= prev2) {
        fireballInvalid(self, other);
    } else {
        if (isDead()) {
            mpBossLife->mLife = 0;
            mTenmetsuTime = getTenmetsuTime_Fire();
            tenmetsuReady();
            hitFireEffect();
            firedeadSE();
            deadAllKill();
            daPyDemoMng_c::getInstance()->setBossDown((daPlBase_c *) player);
            setFireDead(player);
            deadProc();
        } else {
            mTenmetsuTime = getTenmetsuTime_Fire();
            tenmetsuReady();
            hitFireEffect();
            firedmgSE();
            setFireDamage(player);
            damageProc();
        }
    }

    return true;
}

bool dEnBoss_c::hitCallback_Shell(dCc_c *self, dCc_c *other) {
    if (isShellInvalid()) {
        return false;
    }

    dActor_c *player = other->getOwner();
    int prev2 = getLife();
    int prev = mpBossLife->updateShell();

    if (sGlobalData_c<dEnBoss_c>::mData.mInstantKill) {
        mpBossLife->mLife = 0;
    }

    if (prev < prev2) {
        if (isDead()) {
            mpBossLife->mLife = 0;
            mTenmetsuTime = getTenmetsuTime_Shell();
            tenmetsuReady();
            hitShellEffect();
            shelldeadSE();
            deadAllKill();
            daPyDemoMng_c::mspInstance->setBossDown((daPlBase_c *) player);
            setShellDead(player);
            deadProc();
        } else {
            mTenmetsuTime = getTenmetsuTime_Shell();
            tenmetsuReady();
            hitShellEffect();
            shelldmgSE();
            setShellDamage(player);
            damageProc();
        }
    }

    return true;
}

bool dEnBoss_c::hitCallback_HipAttk(dCc_c *self, dCc_c *other) {
    if (isFumiInvalid()) {
        return false;
    }

    dActor_c *player = other->getOwner();
    FumiJumpSet(player);
    mTenmetsuTime = getTenmetsuTime_Press();
    tenmetsuReady();
    mpBossLife->updateHipAtk();

    if (sGlobalData_c<dEnBoss_c>::mData.mInstantKill) {
        mpBossLife->mLife = 0;
    }

    if (mpBossLife->mLife <= 0) {
        fumideadEffect();
        fumideadSE();
        fumiDeadVo();
        mpBossLife->mLife = 0;
        deadAllKill();
        daPyDemoMng_c::mspInstance->setBossDown((daPlBase_c *) player);
        setHipatkDead(player);
        deadProc();
    } else {
        fumidmgEffect();
        fumidmgSE();
        setHipatkDamage(player);
        damageProc();
    }

    return true;
}

bool dEnBoss_c::hitCallback_Spin(dCc_c *self, dCc_c *other) {
    if (isFumiInvalid()) {
        return false;
    }

    dActor_c *player = other->getOwner();
    FumiJumpSet(player);
    mTenmetsuTime = getTenmetsuTime_Press();
    tenmetsuReady();
    mpBossLife->updateFumi();

    if (sGlobalData_c<dEnBoss_c>::mData.mInstantKill) {
        mpBossLife->mLife = 0;
    }

    if (mpBossLife->mLife <= 0) {
        fumideadEffect();
        fumideadSE();
        fumiDeadVo();
        mpBossLife->mLife = 0;
        deadAllKill();
        daPyDemoMng_c::getInstance()->setBossDown((daPlBase_c *) player);
        setHipatkDead(player);
        deadProc();
    } else {
        fumidmgEffect();
        fumidmgSE();
        setHipatkDamage(player);
        damageProc();
    }

    return true;
}

bool dEnBoss_c::hitCallback_Star(dCc_c *self, dCc_c *other) {
    daPlBase_c *player = (daPlBase_c *) other->getOwner();

    if (isStarInvalid()) {
        player->setForcedDamage(this, daPlBase_c::DAMAGE_3);
        return true;
    }

    mTenmetsuTime = getTenmetsuTime_Press();
    tenmetsuReady();
    mpBossLife->updateStar();

    if (sGlobalData_c<dEnBoss_c>::mData.mInstantKill) {
        mpBossLife->mLife = 0;
    }

    if (isDead()) {
        mpBossLife->mLife = 0;
        deadAllKill();
        daPyDemoMng_c::mspInstance->setBossDown(player);
        stardeadSE();
        setStarDead(player);
        deadProc();
    } else {
        stardmgSE();
        setStarDamage(player);
        damageProc();
    }

    return true;
}

void dEnBoss_c::setDeathInfo_Quake(int) {
    mQuakeDamage = 1;
    mTenmetsuTime = getTenmetsuTime_Press();
    tenmetsuReady();
    mpBossLife->updateQuake();

    if (sGlobalData_c<dEnBoss_c>::mData.mInstantKill) {
        mpBossLife->mLife = 0;
    }

    if (isDead()) {
        mpBossLife->mLife = 0;
        daPyDemoMng_c::mspInstance->setBossDownPlayerNo(-1);
        quakedeadSE();
        deadAllKill();
        setQuakeDead();
        deadProc();
    } else {
        quakedmgSE();
        setQuakeDamage();
        damageProc();
    }
}

bool dEnBoss_c::hitCallback_PenguinSlide(dCc_c *self, dCc_c *other) {
    return false;
}

void dEnBoss_c::initializeState_DemoWait() {}
void dEnBoss_c::finalizeState_DemoWait() {}
void dEnBoss_c::executeState_DemoWait() {}

void dEnBoss_c::initializeState_DieFumi() {}
void dEnBoss_c::finalizeState_DieFumi() {}
void dEnBoss_c::executeState_DieFumi() {}

void dEnBoss_c::initializeState_DieFire() { initializeState_DieFumi(); }
void dEnBoss_c::finalizeState_DieFire() { finalizeState_DieFumi(); }
void dEnBoss_c::executeState_DieFire() { executeState_DieFumi(); }

void dEnBoss_c::initializeState_DieSlide() { initializeState_DieFumi(); }
void dEnBoss_c::finalizeState_DieSlide() { finalizeState_DieFumi(); }
void dEnBoss_c::executeState_DieSlide() { executeState_DieFumi(); }

void dEnBoss_c::initializeState_DieShell() { initializeState_DieFumi(); }
void dEnBoss_c::finalizeState_DieShell() { finalizeState_DieFumi(); }
void dEnBoss_c::executeState_DieShell() { executeState_DieFumi(); }

void dEnBoss_c::initializeState_DieStar() { initializeState_DieFumi(); }
void dEnBoss_c::finalizeState_DieStar() { finalizeState_DieFumi(); }
void dEnBoss_c::executeState_DieStar() { executeState_DieFumi(); }

void dEnBoss_c::initializeState_DieQuake() { initializeState_DieFumi(); }
void dEnBoss_c::finalizeState_DieQuake() { finalizeState_DieFumi(); }
void dEnBoss_c::executeState_DieQuake() { executeState_DieFumi(); }
