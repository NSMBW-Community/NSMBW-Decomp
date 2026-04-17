#pragma once

#include <game/bases/d_enemy.hpp>
#include <game/bases/d_boss_life.hpp>
#include <game/bases/d_audio.hpp>
#include <game/bases/d_heap_allocator.hpp>
#include <constants/sound_list.h>

/// @brief The base class for boss enemies.
class dEnBoss_c : public dEn_c {
public:
    struct GlobalData_t {
        bool mInstantKill;
    };

    dEnBoss_c();
    virtual ~dEnBoss_c();

    // fBase_c overrides

    virtual int create();
    virtual int preExecute();
    virtual void postExecute(MAIN_STATE_e state);

    // dEn_c overrides

    virtual void FumiScoreSet(dActor_c *actor) {}
    virtual void Normal_VsPlHitCheck(dCc_c *self, dCc_c *other);
    virtual void Normal_VsYoshiHitCheck(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Fire(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Shell(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_HipAttk(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Spin(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Star(dCc_c *self, dCc_c *other);
    virtual bool hitCallback_Ice(dCc_c *self, dCc_c *other) { iceballInvalid(self, other); return false; }
    virtual bool hitCallback_Slip(dCc_c *self, dCc_c *other) { return false; }
    virtual bool hitCallback_WireNet(dCc_c *self, dCc_c *other) { return false; }
    virtual bool hitCallback_PenguinSlide(dCc_c *self, dCc_c *other);
    virtual void setDeathInfo_Quake(int);
    virtual bool isQuakeDamage();

    // New virtual functions

    STATE_VIRTUAL_FUNC_DECLARE(dEnBoss_c, DemoWait);
    STATE_VIRTUAL_FUNC_DECLARE(dEnBoss_c, DieFumi);
    STATE_VIRTUAL_FUNC_DECLARE(dEnBoss_c, DieFire);
    STATE_VIRTUAL_FUNC_DECLARE(dEnBoss_c, DieSlide);
    STATE_VIRTUAL_FUNC_DECLARE(dEnBoss_c, DieShell);
    STATE_VIRTUAL_FUNC_DECLARE(dEnBoss_c, DieStar);
    STATE_VIRTUAL_FUNC_DECLARE(dEnBoss_c, DieQuake);

    virtual void setBattleReady() {}
    virtual void createModel() {}
    virtual void createBossLife();
    virtual int createInit();
    virtual void tenmetsuReady() {} ///< Prepares the flashing effect after the boss takes damage.
    virtual void tenmetsuProc() {} ///< Handles the flashing effect after the boss takes damage.
    virtual void tenmetsuFin() {} ///< Ends the flashing effect after the boss takes damage.
    virtual int getTenmetsuTime_Fire() { return 40; }
    virtual int getTenmetsuTime_Shell() { return 40; }
    virtual int getTenmetsuTime_Press() { return 40; }
    virtual void deadAllKill();
    virtual void setFumiDamage(dActor_c *) {}
    virtual void setFumiDead(dActor_c *) {}
    virtual void setFireDamage(dActor_c *) {}
    virtual void setFireDead(dActor_c *) {}
    virtual void setHipatkDamage(dActor_c *actor) { setFumiDamage(actor); }
    virtual void setHipatkDead(dActor_c *actor) { setFumiDead(actor); }
    virtual void setSlideDamage(dActor_c *) {}
    virtual void setSlideDead(dActor_c *) {}
    virtual void setStarDamage(dActor_c *) {}
    virtual void setStarDead(dActor_c *) {}
    virtual void setQuakeDamage() {}
    virtual void setQuakeDead() {}
    virtual void setShellDamage(dActor_c *) {}
    virtual void setShellDead(dActor_c *) {}
    virtual void damageProc() {}
    virtual void deadProc() {}
    virtual bool isFumiInvalid() const { return false; }
    virtual bool isFumiDmgInvalid() const { return false; }
    virtual bool isFireInvalid() const { return false; }
    virtual bool isSlideInvalid() const { return true; }
    virtual bool isShellInvalid() const { return false; }
    virtual bool isStarInvalid() const { return false; }
    virtual void fumideadEffect() {}
    virtual void fumidmgEffect() {}
    virtual void hitFireEffect() {}
    virtual void hitShellEffect() {}
    virtual void fumidmgSE() { playSE(SE_BOSS_CMN_STOMPED); }
    virtual void fumideadSE() { playSE(SE_BOSS_CMN_STOMPED_LAST1); }
    virtual void stardmgSE() { playSE(SE_BOSS_CMN_DAMAGE_STAR_DEF); }
    virtual void stardeadSE() { playSE(SE_BOSS_CMN_DAMAGE_STAR_LAST); }
    virtual void shelldmgSE() { playSE(SE_BOSS_CMN_DAMAGE_DEF);}
    virtual void shelldeadSE() { playSE(SE_BOSS_CMN_DAMAGE_LAST);}
    virtual void firedmgSE() { mpBossLife->isDmgSection() ? playSE(SE_BOSS_CMN_DAMAGE_FIRE_DEF) : playSE(SE_BOSS_CMN_DAMAGE_FIRE); }
    virtual void firedeadSE() { playSE(SE_BOSS_CMN_DAMAGE_FIRE_LAST); }
    virtual void quakedmgSE() { playSE(SE_BOSS_CMN_DAMAGE_STAR_DEF); }
    virtual void quakedeadSE() { playSE(SE_BOSS_CMN_DAMAGE_STAR_LAST);}
    virtual void fumiDeadVo() {}
    virtual void damageSVo() {}
    virtual void damageLVo() {}

    void getTenmetsuTimePress(bool a) {
        /// [Get weak function placement to work out]
        if (a) {
            tenmetsuReady();
        }
        mTenmetsuTime = getTenmetsuTime_Press();
        tenmetsuReady();
    }

    void playSE(ulong soundID) { mSndObject.startSound(soundID, m_d0, 0); }
    int getLife() const { return mpBossLife->mLife; }
    bool isDead() const { return mpBossLife->mLife <= 0; }

    void allocate();
    void fumiProc(dActor_c *actor);

    dHeapAllocator_c mAllocator;
    int mTenmetsuTime; ///< Timer for the flashing effect when the boss takes damage.
    dAudio::SndObjctEmy_c mSndObject;
    u16 m_d0;
    u32 mQuakeDamage;
    dBossLifeInf_c *mpBossLife;
};
