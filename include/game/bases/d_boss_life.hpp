#pragma once

/// @brief Interface for tracking the life of a boss enemy.
class dBossLifeInf_c {
public:
    dBossLifeInf_c(int life) : mLife(life) {}

    virtual ~dBossLifeInf_c() {}
    virtual bool isNonDamage() const = 0;
    virtual bool isOneDamage() const = 0;
    virtual bool isTwoDamage() const = 0;
    virtual bool isDmgSection() const { return false; }
    virtual int getDamage_Fire() const = 0;
    virtual int getDamage_Fumi() const = 0;
    virtual int getDamage_HipAtk() const = 0;
    virtual int getDamage_Star() const = 0;
    virtual int getDamage_PenguinSlide() const = 0;
    virtual int getDamage_BlockHit() const = 0;
    virtual int getDamage_Shell() const = 0;
    virtual int getDamage_Quake() const = 0;
    virtual void damageRev(int) {}

    int updateCommon(int dmg) {
        mLife -= dmg;
        if (mLife > 0) {
            damageRev(dmg);
        }
        return mLife;
    }

    int updateFire() { return updateCommon(getDamage_Fire()); }
    int updateFumi() { return updateCommon(getDamage_Fumi()); }
    int updateHipAtk() { return updateCommon(getDamage_HipAtk()); }
    int updateStar() { return updateCommon(getDamage_Star()); }
    int updatePenguinSlide() { return updateCommon(getDamage_PenguinSlide()); }
    int updateBlockHit() { return updateCommon(getDamage_BlockHit()); }
    int updateShell() { return updateCommon(getDamage_Shell()); }
    int updateQuake() { return updateCommon(getDamage_Quake()); }

    int mLife;
};

/// @brief A base implementation of dBossLifeInf_c.
/// @details The boss's life is divided into sections of 6 HP each.
class dBossLife_Common_c : public dBossLifeInf_c {
public:
    dBossLife_Common_c(int num) : dBossLifeInf_c(num) {}
    virtual ~dBossLife_Common_c() {}

    int getSection() const { return (mLife - 1) / 6; }

    virtual bool isNonDamage() const override {
        return getSection() > 1;
    }
    virtual bool isOneDamage() const override {
        return getSection() > 0 && getSection() < 2;
    }
    virtual bool isTwoDamage() const override {
        return getSection() <= 0;
    }

    virtual bool isDmgSection() const override { return mLife % 6 == 0; }
    virtual void damageRev(int dmg) override {
        int sections = mLife / 6;
        if (mLife - sections * 6 != 0 && dmg >= 6) { mLife = (sections + 1) * 6; };
    }
    virtual int getDamage_Fire() const override { return 1; }
    virtual int getDamage_Fumi() const override { return 6; }
    virtual int getDamage_HipAtk() const override { return 6; }
    virtual int getDamage_Star() const override { return 6; }
    virtual int getDamage_PenguinSlide() const override { return 6; }
    virtual int getDamage_BlockHit() const override { return 6; }
    virtual int getDamage_Shell() const override { return 6; }
    virtual int getDamage_Quake() const override { return 6; }
};
