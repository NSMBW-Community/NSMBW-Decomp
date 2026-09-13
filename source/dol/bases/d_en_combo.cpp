#include <game/bases/d_en_combo.hpp>
#include <game/bases/d_a_player_base.hpp>
#include <game/bases/d_score_manager.hpp>

int dEnCombo_c::calcPlFumiCnt(dActor_c *actor) {
    daPlBase_c *pl = (daPlBase_c *) actor;
    pl->calcTreadCount(8);
    return pl->mTreadCount;
}

int dEnCombo_c::calcPlStarCnt(dActor_c *actor) {
    daPlBase_c *pl = (daPlBase_c *) actor;
    pl->calcStarCount(8);
    return pl->getStarCount();
}

int dEnCombo_c::calcPlComboCnt(dActor_c *actor) {
    daPlBase_c *pl = (daPlBase_c *) actor;
    pl->calcComboCount(8);
    return pl->mPlComboCount;
}

int dEnCombo_c::getDamageScore() const {
    static const int cs_damage_score[] = {-1, 1, 4};
    return cs_damage_score[mType];
}

int dEnCombo_c::getQuakeScore(int count) const {
    if (count > 8) {
        count = 8;
    }
    return count;
}

int dEnCombo_c::getComboScore(int count) {
    static const int cs_combo_score_regular[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
    static const int cs_combo_score_short[] = { 4, 4, 5, 6, 7, 8, 8, 8, 8 };

    if (count < 0) {
        return -1;
    }

    if (count >= ARRAY_SIZE(cs_combo_score_regular)) {
        count = ARRAY_SIZE(cs_combo_score_regular) - 1;
    }

    int score = -1;
    switch (mType) {
        case COMBO_NONE:
            break;
        case COMBO_REGULAR:
            score = cs_combo_score_regular[count];
            break;
        case COMBO_SHORT:
            score = cs_combo_score_short[count];
            break;
    }
    return score;
}

void dEnCombo_c::setScore(dActor_c *actor, int a, int b) const {
    if (a < 0) {
        return;
    }

    switch (mType) {
        case COMBO_NONE:
            break;
        case COMBO_REGULAR:
            dScoreMng_c::getInstance()->ScoreSet(actor, a, b);
            break;
        case COMBO_SHORT:
            dScoreMng_c::getInstance()->ScoreSet2(actor, a, b);
            break;
    }
}
