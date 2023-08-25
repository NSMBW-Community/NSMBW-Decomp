#include <game/bases/d_wipe_dokan.hpp>
#include <game/bases/d_base_actor.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_a_py_mng.hpp>
#include <game/bases/d_a_py_demo_mng.hpp>

dWipeDokan_c *dWipeDokan_c::m_instance;

dWipeDokan_c::dWipeDokan_c(nw4r::ut::Color color, mFaderBase_c::EStatus stat) :
mFaderBase_c(mColor(color.rgba), stat) {
    m_instance = this;
    mIsCreated = false;
}

dWipeDokan_c::~dWipeDokan_c() {
    m_instance = nullptr;
}

bool dWipeDokan_c::createLayout() {
    static const char *animeNames[] = {
        "wipeDokan_02_inBlackDown.brlan",
        "wipeDokan_02_outBlackUp.brlan",
        "wipeDokan_02_outBlackDown.brlan",
        "wipeDokan_02_inBlackUp.brlan"
    };

    static const int groupIdxs[] = {0, 1, 2, 3};
    static const char *groupNames[] = {
        "A00_wipeDokan",
        "A00_wipeDokan",
        "A00_wipeDokan",
        "A00_wipeDokan"
    };

    static const char *picNames[] = {
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
    mLyt.AnimeResRegister(animeNames, ARRAY_SIZE(animeNames));
    mLyt.GroupRegister(groupNames, groupIdxs, ARRAY_SIZE(groupNames));
    mpRootPane = mLyt.getRootPane();
    mLyt.PPaneRegister(picNames, mpPic, ARRAY_SIZE(mpPic));

    mIsCreated = true;

    mLyt.AllAnimeEndSetup();

    mpRootPane->setVisible(false);
    mLyt.mDrawOrder = 154;
    mAction = IDLE;

    return true;
}

typedef void (dWipeDokan_c::*actMeth)();
const actMeth actMeths[] = {
    &dWipeDokan_c::OpenSetup,
    &dWipeDokan_c::AnimeEndCheck,
    &dWipeDokan_c::CloseSetup
};

int dWipeDokan_c::calc() {
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
            mpRootPane->setVisible(false);
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
    dBaseActor_c *player = daPyMng_c::getPlayer(actPlayer);
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

void dWipeDokan_c::setStatus(mFaderBase_c::EStatus stat) {
    if (stat == OPAQUE) {
        mStatus = OPAQUE;

        mpPic[0]->setFieldB8(255);
        mLyt.ReverseAnimeStartSetup(OUT_UP, false);

        mpRootPane->setVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    } else if (stat == HIDDEN) {
        mStatus = HIDDEN;

        mpPic[0]->setFieldB8(0);
        mLyt.ReverseAnimeStartSetup(IN_DOWN, false);

        mpRootPane->setVisible(true);
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
