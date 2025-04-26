#include <game/bases/d_wipe_mario.hpp>
#include <game/bases/d_base_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>

dWipeMario_c *dWipeMario_c::m_instance;

dWipeMario_c::dWipeMario_c(nw4r::ut::Color color, mFaderBase_c::EStatus stat) :
mFaderBase_c(mColor(color.rgba), stat) {
    m_instance = this;
    mIsCreated = false;
}

dWipeMario_c::~dWipeMario_c() {
    m_instance = nullptr;
}

bool dWipeMario_c::createLayout() {
    static const char *AnmNameTbl[] = {
        "wipeMario_02_inWindow.brlan",
        "wipeMario_02_outWindow.brlan"
    };

    static const char *GROUP_NAME_DT[] = {
        "A00_inWindow",
        "B00_outWindow"
    };
    static const int ANIME_INDEX_TBL[] = {0, 1};

    static const char *WPANE_NAME_DT[] = {
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
    mLyt.AnimeResRegister(AnmNameTbl, ARRAY_SIZE(AnmNameTbl));
    mLyt.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ARRAY_SIZE(GROUP_NAME_DT));
    mpRootPane = mLyt.getRootPane();
    mLyt.WPaneRegister(WPANE_NAME_DT, mpWnd, ARRAY_SIZE(mpWnd));

    mIsCreated = true;

    mLyt.AllAnimeEndSetup();

    mpRootPane->setVisible(false);
    mLyt.mDrawOrder = 154;
    mAction = IDLE;

    return true;
}

typedef void (dWipeMario_c::*actMeth)();
const actMeth actMeths[] = {
    &dWipeMario_c::OpenSetup,
    &dWipeMario_c::AnimeEndCheck,
    &dWipeMario_c::CloseSetup
};

int dWipeMario_c::calc() {
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
            mpRootPane->setVisible(false);
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

void dWipeMario_c::setStatus(mFaderBase_c::EStatus stat) {
    if (stat == OPAQUE) {
        mStatus = OPAQUE;

        mpWnd[0]->setAlpha(255);
        mLyt.ReverseAnimeStartSetup(OUT, false);

        mpRootPane->setVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    } else if (stat == HIDDEN) {
        mStatus = HIDDEN;

        mpWnd[0]->setAlpha(0);
        mLyt.ReverseAnimeStartSetup(IN, false);

        mpRootPane->setVisible(true);
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
