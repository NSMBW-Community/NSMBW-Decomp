#pragma once

#include <game/bases/d_actor.hpp>

/// @brief Computes and awards score bonuses for enemy combos (star chains, tread chains, etc).
/// @ingroup bases
class dEnCombo_c {
public:
    /// @unofficial
    enum ComboType_e {
        COMBO_NONE,
        COMBO_REGULAR,
        COMBO_SHORT
    };

    dEnCombo_c(ComboType_e type) : mType(type) {} ///< Constructs a combo of the given type.

    int getComboScore(int); ///< Returns the score bonus for the given combo count.
    int getQuakeScore(int) const; ///< Returns the score bonus for the given ground pound count.
    int getDamageScore() const; ///< Returns the score bonus for a single damage hit.

    /// @brief Awards the given combo's score bonus to @p actor, if any.
    /// @param actor The actor to award the score to.
    /// @param a The score bonus, or a negative value to skip awarding score.
    /// @param b The score bonus's on-screen vertical offset.
    void setScore(dActor_c *actor, int a, int b) const;

    static int calcPlStarCnt(dActor_c *actor); ///< Returns the player's current star chain count.
    static int calcPlComboCnt(dActor_c *actor); ///< Returns the player's current combo chain count.
    static int calcPlFumiCnt(dActor_c *actor); ///< Returns the player's current tread chain count.

    ComboType_e mType; ///< This combo's type.
};
