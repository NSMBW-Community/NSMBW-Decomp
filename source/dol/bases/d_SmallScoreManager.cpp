#include <game/bases/d_SmallScoreManager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/framework/f_profile.hpp>

ACTOR_PROFILE(SMALL_SCORE_MANAGER, dSmallScoreManager_c, 0);

dSmallScoreManager_c *dSmallScoreManager_c::m_instance = nullptr;

template <>
dSmallScoreManager_c::GlobalData_t sGlobalData_c<dSmallScoreManager_c>::mData = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    dSmallScoreManager_c::c_SMALL_SCORE_SIZE_4_3_X, dSmallScoreManager_c::c_SMALL_SCORE_SIZE_4_3_Y,
    dSmallScoreManager_c::c_SMALL_SCORE_SIZE_16_9_X, dSmallScoreManager_c::c_SMALL_SCORE_SIZE_16_9_Y,
    dSmallScoreManager_c::c_SMALL_GOAL_SCORE_SIZE_4_3_X, dSmallScoreManager_c::c_SMALL_GOAL_SCORE_SIZE_4_3_Y,
    dSmallScoreManager_c::c_SMALL_GOAL_SCORE_SIZE_16_9_X, dSmallScoreManager_c::c_SMALL_GOAL_SCORE_SIZE_16_9_Y,
    dSmallScoreManager_c::c_SCISSOR_SIZE_X, dSmallScoreManager_c::c_SCISSOR_SIZE_Y
};

const float dSmallScoreManager_c::c_SMALL_SCORE_SIZE_4_3_X = 0.8f;
const float dSmallScoreManager_c::c_SMALL_SCORE_SIZE_4_3_Y = 0.8f;
const float dSmallScoreManager_c::c_SMALL_SCORE_SIZE_16_9_X = 1.0f;
const float dSmallScoreManager_c::c_SMALL_SCORE_SIZE_16_9_Y = 1.0f;
const float dSmallScoreManager_c::c_SMALL_GOAL_SCORE_SIZE_16_9_X = 1.1f;
const float dSmallScoreManager_c::c_SMALL_GOAL_SCORE_SIZE_16_9_Y = 1.1f;
const float dSmallScoreManager_c::c_SMALL_GOAL_SCORE_SIZE_4_3_X = 0.8f;
const float dSmallScoreManager_c::c_SMALL_GOAL_SCORE_SIZE_4_3_Y = 0.8f;
const float dSmallScoreManager_c::c_SCISSOR_SIZE_X = 640.0f;
const float dSmallScoreManager_c::c_SCISSOR_SIZE_Y = 352.0f;

dSmallScoreManager_c::dSmallScoreManager_c() {
    dSmallScoreManager_c::m_instance = this;
}

dSmallScoreManager_c::~dSmallScoreManager_c() {
    dSmallScoreManager_c::m_instance = nullptr;
}

int dSmallScoreManager_c::create() {
    if (!mResLoader.request("Layout/pointGet/pointGet.arc")) {
        return NOT_READY;
    }

    for (int i = 0; i < SMALL_SCORE_COUNT; i++) {
        if (!mSmallScores[i].createLayout(&mResLoader)) {
            return NOT_READY;
        }
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (!mGoalScores[i].createLayout(&mResLoader)) {
            return NOT_READY;
        }
    }

    mVec2_c scissorSize = mVec2_c(c_SCISSOR_SIZE_X, c_SCISSOR_SIZE_Y);

    for (int i = 0; i < SMALL_SCORE_COUNT; i++) {
        mSmallScores[i].setClipScale(scissorSize);
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {
        mGoalScores[i].setClipScale(scissorSize);
    }

    mSmallScoresIdx = 0;
    mGoalScoresIdx = 0;
    return SUCCEEDED;
}

int dSmallScoreManager_c::execute() {
    if (dGameCom::isGameStop(dGameCom::GAME_STOP_ANY)) {
        return SUCCEEDED;
    }

    for (int i = 0; i < SMALL_SCORE_COUNT; i++) {
        mSmallScores[i].execute();
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {
        mGoalScores[i].execute();
    }

    mGoalScoresIdx = 0;
    return SUCCEEDED;
}

int dSmallScoreManager_c::draw() {
    for (int i = 0; i < SMALL_SCORE_COUNT; i++) {
        mSmallScores[i].draw();
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {
        mGoalScores[i].draw();
    }

    return SUCCEEDED;
}

int dSmallScoreManager_c::doDelete() {
    if (!mResLoader.remove()) {
        return NOT_READY;
    }

    for (int i = 0; i < SMALL_SCORE_COUNT; i++) {
        if (!mSmallScores[i].doDelete()) {
            return NOT_READY;
        }
    }

    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (!mGoalScores[i].doDelete()) {
            return NOT_READY;
        }
    }

    return SUCCEEDED;
}

void dSmallScoreManager_c::CreateBlueNumber(const mVec3_c &pos, int popupType, int playerType) {
    mSmallScores[mSmallScoresIdx].mHasBlueColor = true;
    CreateSmallScore(pos, popupType, playerType);
}

void dSmallScoreManager_c::CreateRedNumber(const mVec3_c &pos, int playerType) {
    mSmallScores[mSmallScoresIdx].mEnableBigSmallAnim = true;
    CreateSmallScore(pos, dSmallScore_c::POPUP_TYPE_8, playerType);
}


void dSmallScoreManager_c::CreateSmallScore(const mVec3_c &pos, int popupType, int playerType) {
    mSmallScores[mSmallScoresIdx].CreateSmallScore(pos, popupType, playerType);

    if (dGameCom::GetAspectRatio() == 0) {
        mSmallScores[mSmallScoresIdx].mScale.x = c_SMALL_SCORE_SIZE_4_3_X;
        mSmallScores[mSmallScoresIdx].mScale.y = c_SMALL_SCORE_SIZE_4_3_Y;
    } else {
        mSmallScores[mSmallScoresIdx].mScale.x = c_SMALL_SCORE_SIZE_16_9_X;
        mSmallScores[mSmallScoresIdx].mScale.y = c_SMALL_SCORE_SIZE_16_9_Y;
    }

    if (++mSmallScoresIdx >= SMALL_SCORE_COUNT) {
        mSmallScoresIdx = 0;
    }
}

void dSmallScoreManager_c::CreateGoalScore(const mVec3_c &pos, int popupType, int playerType) {
    while (mGoalScoresIdx < PLAYER_COUNT) {
        int i = mGoalScoresIdx;

        if (!mGoalScores[i].mIsGoalScore) {
            mGoalScores[i].CreateSmallScore(pos, popupType, playerType);
            mGoalScores[i].mIsGoalScore = true;

            if (dGameCom::GetAspectRatio() == 0) {
                mGoalScores[i].mScale.x = c_SMALL_GOAL_SCORE_SIZE_4_3_X;
                mGoalScores[i].mScale.y = c_SMALL_GOAL_SCORE_SIZE_4_3_Y;
            } else {
                mGoalScores[i].mScale.x = c_SMALL_GOAL_SCORE_SIZE_16_9_X;
                mGoalScores[i].mScale.y = c_SMALL_GOAL_SCORE_SIZE_16_9_Y;
            }
            break;
        }
        mGoalScoresIdx++;
    }
}

void dSmallScoreManager_c::GoalScoreExecute(const mVec3_c &pos, int playerType) {
    int scoreIndex = 0;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        if (mGoalScores[i].mIsGoalScore && playerType == mGoalScores[i].mPlayerType) {
            scoreIndex = i;
            break;
        }
    }

    if (mGoalScores[scoreIndex].mIsGoalScore) {
        mGoalScores[scoreIndex].PosSet(pos);
    }
}
