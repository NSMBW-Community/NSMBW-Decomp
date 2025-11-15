#include <game/bases/d_wipe_mario.hpp>
#include <game/bases/d_base_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>

dWipeMario_c *dWipeMario_c::m_instance;

dWipeMario_c::dWipeMario_c(nw4r::ut::Color color, mFaderBase_c::EStatus status) :
mFaderBase_c(mColor(color), status) {
    m_instance = this;
    mIsCreated = false;
}

dWipeMario_c::~dWipeMario_c() {
    m_instance = nullptr;
}

bool dWipeMario_c::createLayout() {
    static const char *AnmNameTbl[ANIM_NAME_COUNT] = {
        "wipeMario_02_inWindow.brlan",
        "wipeMario_02_outWindow.brlan"
    };

    static const char *GROUP_NAME_DT[ANIM_COUNT] = {
        "A00_inWindow",
        "B00_outWindow"
    };

    static const int ANIME_INDEX_TBL[ANIM_COUNT] = {
        inWindow,
        outWindow
    };

    static const char *WPANE_NAME_DT[W_COUNT] = {
        "W_Mario_00"
    };

    if (mIsCreated) {
        return true;
    }

    bool res = mLyt.ReadResource("wipeMario/wipeMario.arc", false);
    if (!res) {
        return false;
    }

    mLyt.build("wipeMario_02.brlyt", 0);
    mLyt.AnimeResRegister(AnmNameTbl, ANIM_NAME_COUNT);
    mLyt.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ANIM_COUNT);
    mpRootPane = mLyt.getRootPane();
    mLyt.WPaneRegister(WPANE_NAME_DT, mpWnd, W_COUNT);

    mIsCreated = true;

    mLyt.AllAnimeEndSetup();

    mpRootPane->SetVisible(false);
    mLyt.mDrawOrder = 154;
    mAction = IDLE;

    return true;
}

typedef void (dWipeMario_c::*Proc)();

int dWipeMario_c::calc() {
    static const Proc Proc_tbl[] = {
        &dWipeMario_c::OpenSetup,
        &dWipeMario_c::AnimeEndCheck,
        &dWipeMario_c::CloseSetup
    };

    if (!mIsCreated) {
        return 1;
    }
    if (mAction != IDLE) {
        (this->*Proc_tbl[mAction])();
        mLyt.AnimePlay();
        mLyt.calc();
    }
    return 1;
}

void dWipeMario_c::draw() {
    if (mIsCreated) {
        mLyt.entry();
    }
}

void dWipeMario_c::OpenSetup() {
    mLyt.AllAnimeEndSetup();
    mLyt.AnimeStartSetup(OUT, false);
    mAction = ANIME_END_CHECK;
}

void dWipeMario_c::AnimeEndCheck() {
    if (!mLyt.isAnime(-1)) {
        if (getStatus() == FADE_IN) {
            mStatus = HIDDEN;
            mFlag |= FADE_IN_COMPLETE;
            mpRootPane->SetVisible(false);
        } else {
            mStatus = OPAQUE;
            mFlag |= FADE_OUT_COMPLETE;
        }
        mAction = IDLE;
    }
}

void dWipeMario_c::CloseSetup() {
    mLyt.AllAnimeEndSetup();
    mLyt.AnimeStartSetup(IN, false);
    mAction = ANIME_END_CHECK;
}

void dWipeMario_c::setStatus(mFaderBase_c::EStatus status) {
    if (status == OPAQUE) {
        mStatus = OPAQUE;

        mpWnd[W_Mario_00]->SetAlpha(255);
        mLyt.ReverseAnimeStartSetup(OUT, false);

        mpRootPane->SetVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    } else if (status == HIDDEN) {
        mStatus = HIDDEN;

        mpWnd[W_Mario_00]->SetAlpha(0);
        mLyt.ReverseAnimeStartSetup(IN, false);

        mpRootPane->SetVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    }
}

bool dWipeMario_c::fadeIn() {
    bool res = mFaderBase_c::fadeIn();
    if (res) {
        mAction = OPEN_SETUP;
    }
    return res;
}

bool dWipeMario_c::fadeOut() {
    bool res = mFaderBase_c::fadeOut();
    if (res) {
        mAction = CLOSE_SETUP;
    }
    return res;
}
