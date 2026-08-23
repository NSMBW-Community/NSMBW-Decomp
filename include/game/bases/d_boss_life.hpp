#pragma once

/// @brief Interface for managing a boss's hit points, damage values, and damage state.
/// @ingroup bases
class dBossLifeInf_c {
public:
    /// @brief Constructs a new boss life manager.
    /// @param life Initial hit point total for the boss.
    dBossLifeInf_c(int life) : mLife(life) {}
    virtual ~dBossLifeInf_c() {} ///< Destroys the boss life manager.

    virtual bool isNonDamage() const = 0; ///< Checks whether the boss has not yet been damaged.
    virtual bool isOneDamage() const = 0; ///< Checks whether the boss has been damaged once.
    virtual bool isTwoDamage() const = 0; ///< Checks whether the boss has been damaged twice.

    /**
     * @brief Checks whether the current HP is at a damage section boundary.
     * @return @p true if the implementation considers the current HP to be at a
     * damage-section boundary; otherwise @p false .
     * @details The default implementation always returns @p false . Implementations that divide
     * boss HP into sections can override this method to expose those boundaries.
     */
    virtual bool isDmgSection() const { return false; }

    virtual int getDamage_Fire() const = 0; ///< Gets the damage dealt by fireball attacks.
    virtual int getDamage_Fumi() const = 0; ///< Gets the damage dealt by stomp (Fumi) attacks.
    virtual int getDamage_HipAtk() const = 0; ///< Gets the damage dealt by ground pound (Hip) attacks.
    virtual int getDamage_Star() const = 0; ///< Gets the damage dealt by Star invincibility attacks.
    virtual int getDamage_PenguinSlide() const = 0; ///< Gets the damage dealt by Penguin slide attacks.
    virtual int getDamage_BlockHit() const = 0; ///< Gets the damage dealt by block hit attacks.
    virtual int getDamage_Shell() const = 0; ///< Gets the damage dealt by shell attacks.
    virtual int getDamage_Quake() const = 0; ///< Gets the damage dealt by multiplayer ground pound (Quake) attacks.

    /// @brief Post-damage callback invoked when a boss survives an attack.
    /// @param dmg The amount of damage applied.
    virtual void damageRev(int dmg) {}

    /// @brief Common update logic for all the @p update methods.
    /// @param dmg The amount of damage applied.
    /// @return The updated HP total.
    int updateCommon(int dmg) {
        mLife -= dmg;
        if (mLife > 0) {
            damageRev(dmg);
        }
        return mLife;
    }

    /// @brief Applies fireball damage to the boss.
    /// @return The updated HP total.
    int updateFire() { return updateCommon(getDamage_Fire()); }

    /// @brief Applies stomp (Fumi) damage to the boss.
    /// @return The updated HP total.
    int updateFumi() { return updateCommon(getDamage_Fumi()); }

    /// @brief Applies ground pound (Hip) damage to the boss.
    /// @return The updated HP total.
    int updateHipAtk() { return updateCommon(getDamage_HipAtk()); }

    /// @brief Applies Star damage to the boss.
    /// @return The updated HP total.
    int updateStar() { return updateCommon(getDamage_Star()); }

    /// @brief Applies Penguin slide damage to the boss.
    /// @return The updated HP total.
    int updatePenguinSlide() { return updateCommon(getDamage_PenguinSlide()); }

    /// @brief Applies block hit damage to the boss.
    /// @return The updated HP total.
    int updateBlockHit() { return updateCommon(getDamage_BlockHit()); }

    /// @brief Applies shell damage to the boss.
    /// @return The updated HP total.
    int updateShell() { return updateCommon(getDamage_Shell()); }

    /// @brief Applies multiplayer ground pound (Quake) damage to the boss.
    /// @return The updated HP total.
    int updateQuake() { return updateCommon(getDamage_Quake()); }

    int mLife; ///< The current hit point total.
};

/**
 * @brief Standard boss life manager implementation, using 6-HP sections.
 * @ingroup bases
 * @details This class provides a section-based implementation of dBossLifeInf_c.
 * Boss HP is divided into sections of 6 HP, with the current section determining the
 * boss's damage state.
 */
class dBossLife_Common_c : public dBossLifeInf_c {
public:
    dBossLife_Common_c(int num) : dBossLifeInf_c(num) {} ///< @copydoc dBossLifeInf_c::dBossLifeInf_c
    virtual ~dBossLife_Common_c() {} ///< @copydoc dBossLifeInf_c::~dBossLifeInf_c

    /// @brief Gets the current section (zero-indexed).
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

    /// @copybrief dBossLifeInf_c::isDmgSection
    virtual bool isDmgSection() const override { return mLife % 6 == 0; }

    /**
     * @copybrief dBossLifeInf_c::damageRev
     * @param dmg The amount of damage applied.
     * @details If the boss survives, the damage was at least 6 HP, and the resulting HP lies
     * partway through a section, HP is rounded up to the next multiple of 6. This behavior
     * is used to preserve complete 6-HP sections after heavy attacks.
     */
    virtual void damageRev(int dmg) override {
        int sections = mLife / 6;
        if (mLife - sections * 6 != 0 && dmg >= 6) {
            mLife = (sections + 1) * 6;
        };
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
