#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_global.hpp>
#include <game/bases/d_resource.hpp>
#include <game/bases/d_SmallScore.hpp>

class dSmallScoreManager_c : public dBase_c {

public:
    dSmallScoreManager_c(); ///< @copydoc fBase_c::fBase_c
    virtual ~dSmallScoreManager_c(); ///< @copydoc fBase_c::~fBase_c

    virtual int create();
    virtual int execute();
    virtual int draw();
    virtual int doDelete();

    void CreateBlueNumber(const mVec3_c &, int, int);
    void CreateRedNumber(const mVec3_c &, int);
    void CreateSmallScore(const mVec3_c &, int, int);
    void CreateGoalScore(const mVec3_c &, int, int);
    void GoalScoreExecute(const mVec3_c &, int);

private:
    u8 m_70[4]; // unused
    d2d::ResAccMultLoader_c mResLoader;
    dSmallScore_c mSmallScores[30];
    dSmallScore_c mGoalScores[4];
    int mSmallScoresIdx;
    int mGoalScoresIdx;

public:
    static dSmallScoreManager_c * m_instance;

    /// @unofficial
    struct GlobalData_s {
        u8 mUnk[12];
        float mSmallScaleX, mSmallScaleY;
        float mNormalScaleX, mNormalScaleY;
        float mSmallScale2X, mSmallScale2Y;
        float mBigScaleX, mBigScaleY;
        float mScissorSizeX, mScissorSizeY;
    };

    static const float c_SMALL_SCALE_X; ///< @unofficial
    static const float c_SMALL_SCALE_Y; ///< @unofficial
    static const float c_NORMAL_SCALE_X; ///< @unofficial
    static const float c_NORMAL_SCALE_Y; ///< @unofficial
    static const float c_SMALL_SCALE_2_X; ///< @unofficial
    static const float c_SMALL_SCALE_2_Y; ///< @unofficial
    static const float c_BIG_SCALE_X; ///< @unofficial
    static const float c_BIG_SCALE_Y; ///< @unofficial
    static const float c_SCISSOR_SIZE_X;
    static const float c_SCISSOR_SIZE_Y;
};
