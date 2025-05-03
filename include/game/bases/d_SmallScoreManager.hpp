#pragma once
#include <game/bases/d_base.hpp>
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
    u32 m_70;
    d2d::ResAccMultLoader_c mResLoader;
    dSmallScore_c mSmallScores[30];
    dSmallScore_c mSmallScores2[4];
    int mSmallScoresIdx;
    int m_4a14;

    /// [Some unused class - but needed here because it has a static initializer.]
    /// @unofficial
    class InitializedUnkClass {
        public:
            u8 unk[12];
            float a, b, c, d, e, f, g, h, i, j;

            static class _init {
                public:
                    _init() {
                        if (dSmallScoreManager_c::msIsInitialized) {
                            return;
                        }
                        dSmallScoreManager_c::msInitializedUnkClass.a = dSmallScoreManager_c::c_SMALL_SCALE_X;
                        dSmallScoreManager_c::msInitializedUnkClass.b = dSmallScoreManager_c::c_SMALL_SCALE_Y;
                        dSmallScoreManager_c::msInitializedUnkClass.c = dSmallScoreManager_c::c_NORMAL_SCALE_X;
                        dSmallScoreManager_c::msInitializedUnkClass.d = dSmallScoreManager_c::c_NORMAL_SCALE_Y;
                        dSmallScoreManager_c::msInitializedUnkClass.e = dSmallScoreManager_c::c_SMALL_SCALE_2_X;
                        dSmallScoreManager_c::msInitializedUnkClass.f = dSmallScoreManager_c::c_SMALL_SCALE_2_Y;
                        dSmallScoreManager_c::msInitializedUnkClass.g = dSmallScoreManager_c::c_BIG_SCALE_X;
                        dSmallScoreManager_c::msInitializedUnkClass.h = dSmallScoreManager_c::c_BIG_SCALE_Y;
                        dSmallScoreManager_c::msInitializedUnkClass.i = dSmallScoreManager_c::c_SCISSOR_SIZE_X;
                        dSmallScoreManager_c::msInitializedUnkClass.j = dSmallScoreManager_c::c_SCISSOR_SIZE_Y;
                        dSmallScoreManager_c::msIsInitialized = 1;
                    }
                } _initializer;
    };
    /// @unofficial
    static char msIsInitialized; // [This might also be an auto-generated guard variable]
    /// @unofficial
    static InitializedUnkClass msInitializedUnkClass;

public:
    static dSmallScoreManager_c * m_instance;

    static float c_SMALL_SCALE_X; ///< @unofficial
    static float c_SMALL_SCALE_Y; ///< @unofficial
    static float c_NORMAL_SCALE_X; ///< @unofficial
    static float c_NORMAL_SCALE_Y; ///< @unofficial
    static float c_SMALL_SCALE_2_X; ///< @unofficial
    static float c_SMALL_SCALE_2_Y; ///< @unofficial
    static float c_BIG_SCALE_X; ///< @unofficial
    static float c_BIG_SCALE_Y; ///< @unofficial
    static float c_SCISSOR_SIZE_X;
    static float c_SCISSOR_SIZE_Y;
};

