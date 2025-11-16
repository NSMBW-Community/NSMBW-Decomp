#include <game/bases/d_wipe_kuppa.hpp>
#include <game/bases/d_base_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>

dWipeKuppa_c *dWipeKuppa_c::m_instance;

dWipeKuppa_c::dWipeKuppa_c(nw4r::ut::Color color, mFaderBase_c::EStatus status) :
mFaderBase_c(mColor(color), status) {
    m_instance = this;
    mIsCreated = false;
}

dWipeKuppa_c::~dWipeKuppa_c() {
    m_instance = nullptr;
}

bool dWipeKuppa_c::createLayout() {
    static const char *AnmNameTbl[ANIM_NAME_COUNT] = {
        "wipeKuppa_05_inWindow.brlan",
        "wipeKuppa_05_outWindow.brlan"
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
        "W_kuppa_00"
    };

    if (mIsCreated) {
        return true;
    }

    bool res = mLyt.ReadResource("wipeKuppa/wipeKuppa.arc", false);
    if (!res) {
        return false;
    }

    mLyt.build("wipeKuppa_05.brlyt", 0);
    mLyt.AnimeResRegister(AnmNameTbl, ANIM_NAME_COUNT);
    mLyt.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ANIM_COUNT);
    mpRootPane = mLyt.getRootPane();
    mLyt.WPaneRegister(WPANE_NAME_DT, mpWnd, W_COUNT);

    mIsCreated = true;

    mLyt.AllAnimeEndSetup();

    mpRootPane->SetVisible(false);
    mLyt.mDrawOrder = m2d::DRAW_ORDER_WIPE;
    mAction = IDLE;

    return true;
}

typedef void (dWipeKuppa_c::*Proc)();

int dWipeKuppa_c::calc() {
    static const Proc Proc_tbl[] = {
        &dWipeKuppa_c::OpenSetup,
        &dWipeKuppa_c::AnimeEndCheck,
        &dWipeKuppa_c::CloseSetup
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

void dWipeKuppa_c::draw() {
    if (mIsCreated) {
        mLyt.entry();
    }
}

void dWipeKuppa_c::OpenSetup() {
    mLyt.AllAnimeEndSetup();
    mLyt.AnimeStartSetup(OUT, false);
    mAction = ANIME_END_CHECK;
}

void dWipeKuppa_c::AnimeEndCheck() {
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

void dWipeKuppa_c::CloseSetup() {
    mLyt.AllAnimeEndSetup();
    mLyt.AnimeStartSetup(IN, false);
    mAction = ANIME_END_CHECK;
}

void dWipeKuppa_c::setStatus(mFaderBase_c::EStatus status) {
    if (status == OPAQUE) {
        mStatus = OPAQUE;

        mpWnd[W_kuppa_00]->SetAlpha(255);
        mLyt.ReverseAnimeStartSetup(OUT, false);

        mpRootPane->SetVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    } else if (status == HIDDEN) {
        mStatus = HIDDEN;

        mpWnd[W_kuppa_00]->SetAlpha(0);
        mLyt.ReverseAnimeStartSetup(IN, false);

        mpRootPane->SetVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    }
}

bool dWipeKuppa_c::fadeIn() {
    bool res = mFaderBase_c::fadeIn();
    if (res) {
        mAction = OPEN_SETUP;
    }
    return res;
}

bool dWipeKuppa_c::fadeOut() {
    bool res = mFaderBase_c::fadeOut();
    if (res) {
        mAction = CLOSE_SETUP;
    }
    return res;
}
