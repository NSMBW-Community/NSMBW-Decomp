#include <game/bases/d_wipe_kuppa.hpp>
#include <game/bases/d_base_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>

dWipeKuppa_c *dWipeKuppa_c::m_instance;

dWipeKuppa_c::dWipeKuppa_c(nw4r::ut::Color color, mFaderBase_c::EStatus stat) :
mFaderBase_c(mColor(color), stat) {
    m_instance = this;
    mIsCreated = false;
}

dWipeKuppa_c::~dWipeKuppa_c() {
    m_instance = nullptr;
}

bool dWipeKuppa_c::createLayout() {
    static const char *AnmNameTbl[] = {
        "wipeKuppa_05_inWindow.brlan",
        "wipeKuppa_05_outWindow.brlan"
    };

    static const char *GROUP_NAME_DT[] = {
        "A00_inWindow",
        "B00_outWindow"
    };
    static const int ANIME_INDEX_TBL[] = {0, 1};

    static const char *WPANE_NAME_DT[] = {
        "W_kuppa_00"
    };

    if (mIsCreated) {
        return true;
    }

    bool res = mLyt.ReadResource("wipeKuppa/wipeKuppa.arc", false);
    if (!res) {
        return false;
    }

    mLyt.build("wipeKuppa_05.brlyt", nullptr);
    mLyt.AnimeResRegister(AnmNameTbl, ARRAY_SIZE(AnmNameTbl));
    mLyt.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ARRAY_SIZE(GROUP_NAME_DT));
    mpRootPane = mLyt.getRootPane();
    mLyt.WPaneRegister(WPANE_NAME_DT, mpWnd, ARRAY_SIZE(mpWnd));

    mIsCreated = true;

    mLyt.AllAnimeEndSetup();

    mpRootPane->SetVisible(false);
    mLyt.mDrawOrder = 154;
    mAction = IDLE;

    return true;
}

typedef void (dWipeKuppa_c::*actMeth)();
const actMeth actMeths[] = {
    &dWipeKuppa_c::OpenSetup,
    &dWipeKuppa_c::AnimeEndCheck,
    &dWipeKuppa_c::CloseSetup
};

int dWipeKuppa_c::calc() {
    if (!mIsCreated) {
        return 1;
    }
    if (mAction != IDLE) {
        (this->*actMeths[mAction])();
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

void dWipeKuppa_c::setStatus(mFaderBase_c::EStatus stat) {
    if (stat == OPAQUE) {
        mStatus = OPAQUE;

        mpWnd[0]->SetAlpha(255);
        mLyt.ReverseAnimeStartSetup(OUT, false);

        mpRootPane->SetVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    } else if (stat == HIDDEN) {
        mStatus = HIDDEN;

        mpWnd[0]->SetAlpha(0);
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
