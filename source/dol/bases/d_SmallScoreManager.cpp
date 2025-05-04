#include <game/bases/d_SmallScoreManager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/framework/f_profile.hpp>

ACTOR_PROFILE(SMALL_SCORE_MANAGER, dSmallScoreManager_c, 0);

dSmallScoreManager_c *dSmallScoreManager_c::m_instance = nullptr;

template <>
dSmallScoreManager_c::GlobalData_s dGlobalData_c<dSmallScoreManager_c>::data = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    dSmallScoreManager_c::c_SMALL_SCALE_X, dSmallScoreManager_c::c_SMALL_SCALE_Y,
    dSmallScoreManager_c::c_NORMAL_SCALE_X, dSmallScoreManager_c::c_NORMAL_SCALE_Y,
    dSmallScoreManager_c::c_SMALL_SCALE_2_X, dSmallScoreManager_c::c_SMALL_SCALE_2_Y,
    dSmallScoreManager_c::c_BIG_SCALE_X, dSmallScoreManager_c::c_BIG_SCALE_Y,
    dSmallScoreManager_c::c_SCISSOR_SIZE_X, dSmallScoreManager_c::c_SCISSOR_SIZE_Y
};

const float dSmallScoreManager_c::c_SMALL_SCALE_X = 0.8f;
const float dSmallScoreManager_c::c_SMALL_SCALE_Y = 0.8f;
const float dSmallScoreManager_c::c_NORMAL_SCALE_X = 1.0f;
const float dSmallScoreManager_c::c_NORMAL_SCALE_Y = 1.0f;
const float dSmallScoreManager_c::c_BIG_SCALE_X = 1.1f;
const float dSmallScoreManager_c::c_BIG_SCALE_Y = 1.1f;
const float dSmallScoreManager_c::c_SMALL_SCALE_2_X = 0.8f;
const float dSmallScoreManager_c::c_SMALL_SCALE_2_Y = 0.8f;
const float dSmallScoreManager_c::c_SCISSOR_SIZE_X = 640.0f;
const float dSmallScoreManager_c::c_SCISSOR_SIZE_Y = 352.0f;

dSmallScoreManager_c::dSmallScoreManager_c() {
    dSmallScoreManager_c::m_instance = this;
}

dSmallScoreManager_c::~dSmallScoreManager_c() {
    dSmallScoreManager_c::m_instance = nullptr;
}

int dSmallScoreManager_c::create() {
    if (! mResLoader.request("Layout/pointGet/pointGet.arc")) {
        return fBase_c::NOT_READY;
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores); i++) {
        if (! mSmallScores[i].createLayout(&mResLoader)) {
            return fBase_c::NOT_READY;
        }
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mGoalScores); i++) {
        if (! mGoalScores[i].createLayout(&mResLoader)) {
            return fBase_c::NOT_READY;
        }
    }

    mVec2_c clip = mVec2_c(640.0f, 352.0f);

    for (int i = 0; i < ARRAY_SIZE(mSmallScores); i++) {
        mSmallScores[i].setClipScale(clip);
    }

    for (int i = 0; i < ARRAY_SIZE(mGoalScores); i++) {
        mGoalScores[i].setClipScale(clip);
    }

    mSmallScoresIdx = 0;
    mGoalScoresIdx = 0;
    return fBase_c::SUCCEEDED;
}

int dSmallScoreManager_c::execute() {
    if (dGameCom::isGameStop(dGameCom::GAME_STOP_ANY)) {
        return fBase_c::SUCCEEDED;
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores); i++) {
        mSmallScores[i].execute();
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mGoalScores); i++) {
        mGoalScores[i].execute();
    }

    mGoalScoresIdx = 0;
    return fBase_c::SUCCEEDED;
}

int dSmallScoreManager_c::draw() {
    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores); i++) {
        mSmallScores[i].draw();
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mGoalScores); i++) {
        mGoalScores[i].draw();
    }

    return fBase_c::SUCCEEDED;
}

int dSmallScoreManager_c::doDelete() {
    if (! mResLoader.remove()) {
        return fBase_c::NOT_READY;
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores); i++) {
        if (! mSmallScores[i].doDelete()) {
            return fBase_c::NOT_READY;
        }
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mGoalScores); i++) {
        if (! mGoalScores[i].doDelete()) {
            return fBase_c::NOT_READY;
        }
    }

    return fBase_c::SUCCEEDED;
}

void dSmallScoreManager_c::CreateBlueNumber(const mVec3_c & pos, int popup_type, int player_type) {
    mSmallScores[mSmallScoresIdx].mHasBlueColor = true;
    CreateSmallScore(pos, popup_type, player_type);
}

void dSmallScoreManager_c::CreateRedNumber(const mVec3_c & pos, int player_type) {
    mSmallScores[mSmallScoresIdx].mEnableBigSmallAnim = true;
    CreateSmallScore(pos, 15, player_type);
}


void dSmallScoreManager_c::CreateSmallScore(const mVec3_c &pos, int popup_type, int player_type) {
    mSmallScores[mSmallScoresIdx].CreateSmallScore(pos, popup_type, player_type);

    if (dGameCom::GetAspectRatio() == 0) {
        mSmallScores[mSmallScoresIdx].mScale.x = 0.8f;
        mSmallScores[mSmallScoresIdx].mScale.y = 0.8f;
    } else {
        mSmallScores[mSmallScoresIdx].mScale.x = 1.0f;
        mSmallScores[mSmallScoresIdx].mScale.y = 1.0f;
    }

    mSmallScoresIdx++;
    if (mSmallScoresIdx >= (int)ARRAY_SIZE(mSmallScores)) {
        mSmallScoresIdx = 0;
    }
}

void dSmallScoreManager_c::CreateGoalScore(const mVec3_c & pos, int popup_type, int player_type) {
    for (; mGoalScoresIdx < 4; mGoalScoresIdx++) {
        int i = mGoalScoresIdx;

        if (mGoalScores[i].mIsGoalScore) {
            continue;
        }

        mGoalScores[i].CreateSmallScore(pos, popup_type, player_type);
        mGoalScores[i].mIsGoalScore = true;

        if (dGameCom::GetAspectRatio() == 0) {
            mGoalScores[i].mScale.x = 0.8f;
            mGoalScores[i].mScale.y = 0.8f;
        } else {
            mGoalScores[i].mScale.x = 1.1f;
            mGoalScores[i].mScale.y = 1.1f;
        }

        break;
    }
}

void dSmallScoreManager_c::GoalScoreExecute(const mVec3_c & pos, int player_type) {
    int i = 0;
    for (int j = 0; j < ARRAY_SIZE(mGoalScores); j++) {
        if (mGoalScores[j].mIsGoalScore && (player_type == mGoalScores[j].mPlayerType)) {
            i = j;
            break;
        }
    }

    if (mGoalScores[i].mIsGoalScore) {
        mGoalScores[i].PosSet(pos);
    }
}
