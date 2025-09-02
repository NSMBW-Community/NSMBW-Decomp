#pragma once
#include <game/bases/d_2d.hpp>
#include <game/bases/d_lyttextBox.hpp>
#include <game/bases/d_tag_processor.hpp>
#include <game/mLib/m_2d.hpp>
#include <nw4r/lyt.h>

class LytBase_c : public d2d::Multi_c {
public:
    LytBase_c();
    ~LytBase_c();
    virtual bool build(const char *name, d2d::ResAccMult_c *resAcc);

    LytTextBox_c *findTextBox(const char *name);
    void allocStringBuffer(nw4r::lyt::Pane *pane);

    bool ReadResourceEx(const char *name, int i, bool isLocalized);
    bool ReadResource(const char *name, bool isLocalized);
    bool ReadResource2(const char *name, int i);
    bool ReadResource3(const char *name, int i); ///< @unofficial [Not in Shield version].

    void NPaneRegister(const char **paneNames, nw4r::lyt::Pane **panes, int count);
    void WPaneRegister(const char **windowPaneNames, nw4r::lyt::Window **panes, int count);
    void PPaneRegister(const char **picPaneNames, nw4r::lyt::Picture **panes, int count);
    void TPaneRegister(const char **textboxNames, LytTextBox_c **panes, int count);
    void TPaneNameRegister(const char **textboxNames, const int *messageIDs, int messageGroup, int count);
    void AnimeResRegister(const char **animeNames, int count);
    void GroupRegister(const char **groupNames, const int *animeIdxs, int count);

    void AnimeStartBaseSetup(int animeIdx);
    void AnimeStartSetup(int animeIdx, bool b);
    void LoopAnimeStartSetup(int animeIdx);
    void ReverseAnimeStartSetup(int animeIdx, bool b);
    void AnimeEndSetup(int animeIdx);
    void AllAnimeEndSetup();

    void AnimePlay();
    bool isAnime(int);
    bool isAllAnime();

    void FUN_800c9770(const nw4r::lyt::Pane *, d2d::ClipSettings &); ///< @unofficial

    bool doDelete();

    m2d::AnmGroup_c &getAnmGroup(int index) const { return mpAnimGroup[index]; }

private:
    d2d::ResAccMultLoader_c mResAccessorLoader;

    m2d::AnmResV2_c *mpAnimRes;
    m2d::AnmGroup_c *mpAnimGroup;

    bool *mpEnabledAnims;
    int mAnimCount;
    int mGroupCount;

    int mLastStartedAnimNum;

public:
    static TagProcessor_c s_TagPrc;
};
