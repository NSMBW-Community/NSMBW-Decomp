#include <game/bases/d_wipe_dokan.hpp>
#include <game/bases/d_base_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>

dWipeDokan_c *dWipeDokan_c::m_instance;

dWipeDokan_c::dWipeDokan_c(nw4r::ut::Color color, mFaderBase_c::EStatus status) :
mFaderBase_c(mColor(color), status) {
    m_instance = this;
    mIsCreated = false;
}

dWipeDokan_c::~dWipeDokan_c() {
    m_instance = nullptr;
}

bool dWipeDokan_c::createLayout() {
    static const char *AnmNameTbl[ANIM_NAME_COUNT] = {
        "wipeDokan_02_inBlackDown.brlan",
        "wipeDokan_02_outBlackUp.brlan",
        "wipeDokan_02_outBlackDown.brlan",
        "wipeDokan_02_inBlackUp.brlan"
    };

    static const char *GROUP_NAME_DT[ANIM_COUNT] = {
        "A00_wipeDokan",
        "A00_wipeDokan",
        "A00_wipeDokan",
        "A00_wipeDokan"
    };

    static const int ANIME_INDEX_TBL[ANIM_COUNT] = {
        inBlackDown,
        outBlackUp,
        outBlackDown,
        inBlackUp
    };

    static const char *PPANE_NAME_DT[P_COUNT] = {
        "P_baseBlack_00"
    };

    if (mIsCreated) {
        return true;
    }

    bool res = mLyt.ReadResource("wipeDokan/wipeDokan.arc", false);
    if (!res) {
        return false;
    }

    mLyt.build("wipeDokan_02.brlyt", 0);
    mLyt.AnimeResRegister(AnmNameTbl, ANIM_NAME_COUNT);
    mLyt.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ANIM_COUNT);
    mpRootPane = mLyt.getRootPane();
    mLyt.PPaneRegister(PPANE_NAME_DT, mpPic, P_COUNT);

    mIsCreated = true;

    mLyt.AllAnimeEndSetup();

    mpRootPane->SetVisible(false);
    mLyt.mDrawOrder = m2d::DRAW_ORDER_WIPE;
    mAction = IDLE;

    return true;
}

typedef void (dWipeDokan_c::*Proc)();

int dWipeDokan_c::calc() {
    static const Proc Proc_tbl[] = {
        &dWipeDokan_c::OpenSetup,
        &dWipeDokan_c::AnimeEndCheck,
        &dWipeDokan_c::CloseSetup
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

void dWipeDokan_c::draw() {
    if (mIsCreated) {
        mLyt.entry();
    }
}

void dWipeDokan_c::OpenSetup() {
    mLyt.AllAnimeEndSetup();
    if (MuKiDecision()) {
        mLyt.AnimeStartSetup(OUT_DOWN, false);
    } else {
        mLyt.AnimeStartSetup(OUT_UP, false);
    }
    mAction = ANIME_END_CHECK;
}

void dWipeDokan_c::AnimeEndCheck() {
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

bool dWipeDokan_c::MuKiDecision() {
    int actPlayer = daPyDemoMng_c::mspInstance->mPlNo;
    if (actPlayer < 0) {
        return false;
    }
    dAcPy_c *player = daPyMng_c::getPlayer(actPlayer);
    if (player == nullptr) {
        return false;
    }
    return dGameCom::getDispCenterY() < player->mPos.y;
}

void dWipeDokan_c::CloseSetup() {
    mLyt.AllAnimeEndSetup();
    if (MuKiDecision()) {
        mLyt.AnimeStartSetup(IN_UP, false);
    } else {
        mLyt.AnimeStartSetup(IN_DOWN, false);
    }
    mAction = ANIME_END_CHECK;
}

void dWipeDokan_c::setStatus(mFaderBase_c::EStatus status) {
    if (status == OPAQUE) {
        mStatus = OPAQUE;

        mpPic[P_baseBlack_00]->SetAlpha(255);
        mLyt.ReverseAnimeStartSetup(OUT_UP, false);

        mpRootPane->SetVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    } else if (status == HIDDEN) {
        mStatus = HIDDEN;

        mpPic[P_baseBlack_00]->SetAlpha(0);
        mLyt.ReverseAnimeStartSetup(IN_DOWN, false);

        mpRootPane->SetVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    }
}

bool dWipeDokan_c::fadeIn() {
    bool res = mFaderBase_c::fadeIn();
    if (res) {
        mAction = OPEN_SETUP;
    }
    return res;
}

bool dWipeDokan_c::fadeOut() {
    bool res = mFaderBase_c::fadeOut();
    if (res) {
        mAction = CLOSE_SETUP;
    }
    return res;
}
