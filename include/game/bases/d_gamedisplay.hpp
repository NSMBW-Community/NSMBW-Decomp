#pragma once

#include <types.h>

#include <game/bases/d_base.hpp>
#include <game/bases/d_lytbase.hpp>
#include <game/mLib/m_mtx.hpp>
#include <game/sLib/s_FStateMgr.hpp>
#include <game/sLib/s_StateMethodUsr_FI.hpp>
#include <game/sLib/s_State.hpp>
#include <constants/game_constants.h>

#include <lib/nw4r/lyt.h>

namespace EGG {

class Effect {
public:
    enum ERecursive {};

    Effect();
    virtual ~Effect();
    virtual void create();
    virtual void fade();
    virtual void followFade();
    virtual void kill();
    virtual void setDisableCalc(bool);
    virtual void setDisableDraw(bool);
    virtual void setDisableCalcDraw(bool);
    virtual void setLife(unsigned short, EGG::Effect::ERecursive);
    virtual void setEmitRatio(float, EGG::Effect::ERecursive);
    virtual void setEmitInterval(unsigned short, EGG::Effect::ERecursive);
    virtual void setEmitEmitDiv(unsigned short, EGG::Effect::ERecursive);
    virtual void setInitVelocityRandom(signed char, EGG::Effect::ERecursive);
    virtual void setPowerYAxis(float, EGG::Effect::ERecursive);
    virtual void setPowerRadiationDir(float, EGG::Effect::ERecursive);
    virtual void setPowerSpecDir(float, EGG::Effect::ERecursive);
    virtual void setPowerSpecDirAdd(float, EGG::Effect::ERecursive);
    virtual void setSpecDir(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setSpecDirAdd(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setVelocity(const nw4r::math::VEC3&);
    virtual void setColor(unsigned char, unsigned char, unsigned char, unsigned char, EGG::Effect::ERecursive);
    virtual void setDefaultParticleSize(nw4r::math::VEC2&, EGG::Effect::ERecursive);
    virtual void setParticleScale(nw4r::math::VEC2&, EGG::Effect::ERecursive);
    virtual void setDefaultParticleRotate(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setParticleRotate(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setEmitterSize(const nw4r::math::VEC3&, bool, EGG::Effect::ERecursive);
    virtual void setLocalScale(const nw4r::math::VEC3&, EGG::Effect::ERecursive);
    virtual void setDynamicsScale(const nw4r::math::VEC3&, const nw4r::math::VEC2*);
    virtual void setScale(float);
    virtual void setScale(const nw4r::math::VEC3&);
    virtual void setPos(const nw4r::math::VEC3&);
    virtual void setMtx(const nw4r::math::MTX34&);
    virtual void setPtclAnim(int, bool);
    virtual void update();
    virtual void getEffect() const;
    virtual void getRootEmitter() const;
    virtual void reset();

    u8 mPad[0x7c];
};

} // namespace EGG
namespace mEf {

class effect_c : public EGG::Effect {
public:
    effect_c() {}

    virtual void createEffect(const char *, int);
    virtual void createEffect(const char *, ulong, const mVec3_c *, const mAng3_c *, const mVec3_c *);
    virtual void createEffect(const char *, ulong, const mMtx_c *);
    // virtual void vfa8();
    // virtual void vfac();
    virtual void follow(const mVec3_c *, const mAng3_c *, const mVec3_c *);
    virtual void follow(const mMtx_c *);

    u8 mPad[0x92];
    mAng mAng;
};

class levelEffect_c : public effect_c {
public:
    levelEffect_c() : m_114(0), m_118(0), m_11c(0), m_11d(0), m_120(0), m_124(0) {}
    virtual ~levelEffect_c() { cleanup(); }

    // [tmp, so that vtable doesn't generate in this TU]
    virtual void createEffect(const char *, int);

    void cleanup();

    u32 m_114, m_118;
    u8 m_11c, m_11d;
    u32 m_120, m_124;
};

void createEffect(const char *, unsigned long, const mVec3_c *, const mAng3_c *, const mVec3_c *);

}; // namespace mEf
class PauseManager_c {
public:
    u8 m_00[0x1D]; // TODO
    u8 mDisablePause;
    static bool m_OtasukeAfter;
    static PauseManager_c *m_instance;
};
class dActorCreateMng_c {
public:
    u8 m_00[0xBCB]; // TODO
    u8 m_bcb;
    static dActorCreateMng_c *m_instance;
};
class dStageTimer_c {
public:
    char mPad1[4];
    u32 mPreciseTime;
    void setTimer(short time);
    static dStageTimer_c *m_instance;
};

struct mRect_c {
    float left, top, right, bottom;
};

class dGameDisplay_c : public dBase_c {

    /// @brief The picture panes used in the layout.
    /// @unofficial
    enum P_PANE_e {
        P_collectOff_00,
        P_collection_00,
        P_collectOff_01,
        P_collection_01,
        P_collectOff_02,
        P_collection_02,
        P_marioIcon_00,
        P_luijiIcon_00,
        P_kinoB_00,
        P_kinoY_00,
        P_COUNT
    };

    /// @brief The text boxes used in the layout.
    /// @unofficial
    enum T_PANE_e {
        T_left_00,
        T_x_01,
        T_left_01,
        T_x_02,
        T_left_02,
        T_x_03,
        T_left_03,
        T_x_04,
        T_coin_00,
        T_time_00,
        T_score_00,
        T_COUNT
    };

    /// @brief The null panes used in the layout.
    /// @unofficial
    enum N_PANE_e {
        N_otasukeInfo_00,
        N_otasukeChu_00,
        N_left_00,
        N_coin_00,
        N_collection_00,
        N_score_00,
        N_areaZanki_00,
        N_areaCoin_00,
        N_areaScore_00,
        N_marioIcon_00,
        N_luigiIcon_00,
        N_kinoB_00,
        N_kinoY_00,
        N_coin_01,
        N_time_00,
        N_proportionL_00,
        N_proportionR_00,
        N_coin1st_00,
        N_coin2nd_00,
        N_coin3rd_00,
        N_COUNT
    };

public:
    dGameDisplay_c();

    virtual int create();
    virtual int execute();
    virtual int doDelete();
    virtual int draw();
    virtual ~dGameDisplay_c();

private:
    void AlphaEnterAndExit();
    void AreaCheck();
    void AreaSetup(int, int);
    bool createLayout();
    void Effect1UP(int);
    void EffectCollectionCoinClear();
    void EffectCollectionCoinGet(int);
    void GrayColorSet(int);
    bool NormalSettle();
    bool OtasukeSettle();
    void OtehonPosChange();
    void RestCoinAnimeCheck();
    void RestCoinAnimeSetup();
    void RestDispSetup();
    void ReturnGrayColorSet(int);
    void setCoinNum(int);
    void setCollect();
    void setPlayNum(int *);
    void setScore(int);
    void setTime(int);
    void fn_801585c0();

    LytBase_c mLayout;
    mEf::levelEffect_c mEffect;
    GXColorS10 mColorBackup[3][PLAYER_COUNT];

    sFStateMgr_c<dGameDisplay_c, sStateMethodUsr_FI_c> mStateMgr;

    STATE_FUNC_DECLARE(dGameDisplay_c, ProcGoalEnd);
    STATE_FUNC_DECLARE(dGameDisplay_c, ProcGoalSettleUp);
    STATE_FUNC_DECLARE(dGameDisplay_c, ProcMainGame);
    STATE_FUNC_DECLARE(dGameDisplay_c, ProcMainPause);

    int mPlayNum[PLAYER_COUNT];
    int mCoins;
    int mTimer;
    u32 m_3E4;
    int mScore;
    int m_3EC[3];  // one for each star coin
    u32 m_3F8;
    u32 m_3FC;
    int m_400;
    int m_404;
    int m_408;
    u32 m_40C;
    u32 m_410;
    int m_414;
    int mAreaAlpha[3];

    u32 m_424[PLAYER_COUNT];

    int m_434;
    int m_438;
    u32 m_43C;
    int m_440;
    u32 m_444;

    u8 m_448;
    u8 m_449;
    u8 m_44A;
    bool mHasLoadedLayout;
    u8 m_44C;
    u8 m_44D[3];
    u8 m_450;
    u8 m_451;
    u8 m_452;
    u8 m_453;
    u8 m_454[PLAYER_COUNT];
    mRect_c m_458[3];
    u32 m_488;
    u32 m_48C;

    nw4r::lyt::Pane *mpRootPane; ///< The root pane of the view.

    nw4r::lyt::Picture *mpPicturePanes[P_COUNT]; ///< The picture panes of the view.
    LytTextBox_c *mpTextBoxes[T_COUNT]; ///< The textboxes of the view.
    nw4r::lyt::Pane *mpNullPanes[N_COUNT]; ///< The null panes of the view.

    float m_538;
    float m_53C;
    float m_540;

    mVec3_c m_544;
    mVec3_c m_550;
    mVec3_c m_55C;
    mVec3_c m_568;
    mVec3_c m_574;
    mVec3_c m_580;
    mVec3_c m_58C;
    mVec3_c m_598;

public:
    static dGameDisplay_c *m_instance;
    static const int c_COINNUM_DIGIT;
    static const int c_PLAYNUM_DIGIT;
    static const int c_TIME_DIGIT;
    static const int c_SCORE_DIGIT;
};
