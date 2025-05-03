#pragma once
#include <game/bases/d_base.hpp>
#include <game/bases/d_resource.hpp>
#include <game/bases/d_SmallScore.hpp>

class dSmallScoreManager_c : public dBase_c {

public:
    dSmallScoreManager_c(); ///< @copydoc fBase_c::fBase_c
    virtual ~dSmallScoreManager_c(); ///< @copydoc fBase_c::~fBase_c

    int create();
    int execute();
    int draw();
    int doDelete();

    void CreateBlueNumber(const mVec3_c &, int, int);
    void CreateRedNumber(const mVec3_c &, int);
    void CreateSmallScore(const mVec3_c &, int, int);
    void CreateGoalScore(const mVec3_c &, int, int);
    void GoalScoreExecute(const mVec3_c &, int);

    private:
    u32 m_70;
    d2d::ResAccMultLoader_c mResLoader;
    dSmallScore_c mSmallScores[30];
    dSmallScore_c mSmallScores2[4];
    int mSmallScoresIdx;
    u32 m_4a14;

public:
    static dSmallScoreManager_c * m_instance;
};

