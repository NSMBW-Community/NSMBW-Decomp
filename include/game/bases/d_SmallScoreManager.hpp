#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_global.hpp>
#include <game/bases/d_resource.hpp>
#include <game/bases/d_SmallScore.hpp>
#include <constants/game_constants.h>

/// @brief A manager for the small score popups.
/// @details See also dSmallScore_c.
/// @ingroup bases
class dSmallScoreManager_c : public dBase_c {

public:
    dSmallScoreManager_c(); ///< @copydoc fBase_c::fBase_c
    virtual ~dSmallScoreManager_c(); ///< @copydoc fBase_c::~fBase_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    void CreateBlueNumber(const mVec3_c &pos, int popupType, int playerType);
    void CreateRedNumber(const mVec3_c &pos, int playerType);
    void CreateSmallScore(const mVec3_c &pos, int popupType, int playerType);
    void CreateGoalScore(const mVec3_c &pos, int popupType, int playerType);
    void GoalScoreExecute(const mVec3_c &pos, int playerType);

private:
    u8 m_70[4]; ///< @unused
    d2d::ResAccMultLoader_c mResLoader; ///< The resource loader for the layout.
    dSmallScore_c mSmallScores[SMALL_SCORE_COUNT]; ///< The regular small score popups.
    dSmallScore_c mGoalScores[PLAYER_COUNT]; ///< The goal score popups.
    int mSmallScoresIdx; ///< The next free index in ::mSmallScores.
    int mGoalScoresIdx; ///< The next free index in ::mGoalScores.

public:
    static dSmallScoreManager_c *m_instance; ///< The instance of the small score manager.

    /// @unofficial
    struct GlobalData_s {
        u8 mUnk[12];
        float mSmallScoreSize_4_3_X, mSmallScoreSize_4_3_Y;
        float mSmallScoreSize_16_9_X, mSmallScoreSize_16_9_Y;
        float mSmallGoalScoreSize_16_9_X, mSmallGoalScoreSize_16_9_Y;
        float mSmallGoalScoreSize_4_3_X, mSmallGoalScoreSize_4_3_Y;
        float mScissorSizeX, mScissorSizeY;
    };

    static const float c_SMALL_SCORE_SIZE_4_3_X;
    static const float c_SMALL_SCORE_SIZE_4_3_Y;
    static const float c_SMALL_SCORE_SIZE_16_9_X;
    static const float c_SMALL_SCORE_SIZE_16_9_Y;
    static const float c_SMALL_GOAL_SCORE_SIZE_4_3_X;
    static const float c_SMALL_GOAL_SCORE_SIZE_4_3_Y;
    static const float c_SMALL_GOAL_SCORE_SIZE_16_9_X;
    static const float c_SMALL_GOAL_SCORE_SIZE_16_9_Y;
    static const float c_SCISSOR_SIZE_X;
    static const float c_SCISSOR_SIZE_Y;
};
