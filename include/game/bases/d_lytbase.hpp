#pragma once
#include <game/bases/d_2d.hpp>
#include <game/bases/d_lyttextBox.hpp>
#include <game/mLib/m_2d.hpp>
#include <nw4r/lyt.h>

class LytBase_c : public d2d::Multi_c {
public:
    LytBase_c();
    ~LytBase_c();
    virtual bool build(const char *, d2d::ResAccMult_c *);

    LytTextBox_c *findTextBox(const char *);
    void allocStringBuffer(nw4r::lyt::Pane *);

    bool ReadResourceEx(const char *, int, bool);
    bool ReadResource(const char *, bool);
    bool ReadResource2(const char *, int);
    bool ReadResource3(const char *, int); ///< @unofficial Not in Shield version.

    void NPaneRegister(const char **, nw4r::lyt::Pane **, int);
    void WPaneRegister(const char **, nw4r::lyt::Window **, int);
    void PPaneRegister(const char **, nw4r::lyt::Picture **, int);
    void TPaneRegister(const char **, LytTextBox_c **, int);
    void TPaneNameRegister(const char **, const int *, int, int);
    void AnimeResRegister(const char **, int);
    void GroupRegister(const char **, const int *, int);

    void AnimeStartBaseSetup(int);
    void AnimeStartSetup(int, bool);
    void LoopAnimeStartSetup(int);
    void ReverseAnimeStartSetup(int, bool);
    void AnimeEndSetup(int);
    void AllAnimeEndSetup();

    void AnimePlay();
    bool isAnime(int);
    bool isAllAnime();

    void FUN_800c9770(void *, float *); ///< @unofficial

    bool doDelete();

private:
    d2d::ResAccMultLoader_c mResAccessorLoader;

    m2d::AnmResV2_c *mpAnimRes;
    m2d::AnmGroup_c *mpAnimGroup;

    bool *mpEnabledAnims;
    int mAnimCount;

    int mGroupCount;

    int mLastStartedAnimNum;
};
