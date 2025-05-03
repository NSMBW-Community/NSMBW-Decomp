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

    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores2); i++) {
        if (! mSmallScores2[i].createLayout(&mResLoader)) {
            return fBase_c::NOT_READY;
        }
    }

    mVec2_c clip = mVec2_c(640.0f, 352.0f);

    for (int i = 0; i < ARRAY_SIZE(mSmallScores); i++) {
        mSmallScores[i].setClipScale(clip);
    }

    for (int i = 0; i < ARRAY_SIZE(mSmallScores2); i++) {
        mSmallScores2[i].setClipScale(clip);
    }

    mSmallScoresIdx = 0;
    m_4a14 = 0;
    return fBase_c::SUCCEEDED;
}

int dSmallScoreManager_c::execute() {
    if (dGameCom::isGameStop(dGameCom::GAME_STOP_ANY)) {
        return fBase_c::SUCCEEDED;
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores); i++) {
        mSmallScores[i].execute();
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores2); i++) {
        mSmallScores2[i].execute();
    }

    m_4a14 = 0;
    return fBase_c::SUCCEEDED;
}

int dSmallScoreManager_c::draw() {
    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores); i++) {
        mSmallScores[i].draw();
    }

    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores2); i++) {
        mSmallScores2[i].draw();
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

    for (int i = 0; i < (int)ARRAY_SIZE(mSmallScores2); i++) {
        if (! mSmallScores2[i].doDelete()) {
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
    for (; m_4a14 < 4; m_4a14++) {
        int i = m_4a14;

        if (mSmallScores2[i].mIsGoalScore) {
            continue;
        }

        mSmallScores2[i].CreateSmallScore(pos, popup_type, player_type);
        mSmallScores2[i].mIsGoalScore = true;

        if (dGameCom::GetAspectRatio() == 0) {
            mSmallScores2[i].mScale.x = 0.8f;
            mSmallScores2[i].mScale.y = 0.8f;
        } else {
            mSmallScores2[i].mScale.x = 1.1f;
            mSmallScores2[i].mScale.y = 1.1f;
        }

        break;
    }
}

void dSmallScoreManager_c::GoalScoreExecute(const mVec3_c & pos, int player_type) {
    int i = 0;
    if (mSmallScores2[0].mIsGoalScore && (player_type == mSmallScores2[0].mPlayerType)) {
        i = 0;
    } else if (mSmallScores2[1].mIsGoalScore && (player_type == mSmallScores2[1].mPlayerType)) {
        i = 1;
    } else if (mSmallScores2[2].mIsGoalScore && (player_type == mSmallScores2[2].mPlayerType)) {
        i = 2;
    } else if (mSmallScores2[3].mIsGoalScore && (player_type == mSmallScores2[3].mPlayerType)) {
        i = 3;
    }

    if (mSmallScores2[i].mIsGoalScore) {
        mSmallScores2[i].PosSet(pos);
    }
}
