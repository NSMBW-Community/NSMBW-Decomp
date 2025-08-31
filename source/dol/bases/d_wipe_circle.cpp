#include <game/bases/d_wipe_circle.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>
#include <game/bases/d_fukidashi_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_s_stage.hpp>

dWipeCircle_c *dWipeCircle_c::m_instance;

dWipeCircle_c::dWipeCircle_c(nw4r::ut::Color color, mFaderBase_c::EStatus status) :
mFaderBase_c(mColor(color.rgba), status) {
    m_instance = this;
    mIsCreated = false;
}

dWipeCircle_c::~dWipeCircle_c() {
    m_instance = nullptr;
}

bool dWipeCircle_c::createLayout() {
    static const char *AnmNameTbl[ANIM_NAME_COUNT] = {
        "wipeCircle_00_inWindow.brlan",
        "wipeCircle_00_outWindow.brlan"
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
        "W_circle_00"
    };

    if (mIsCreated) {
        return true;
    }

    if (!mLyt.ReadResource("wipeCircle/wipeCircle.arc", false)) {
        return false;
    }

    mLyt.build("wipeCircle_00.brlyt", nullptr);
    mLyt.AnimeResRegister(AnmNameTbl, ANIM_NAME_COUNT);
    mLyt.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, ANIM_COUNT);
    mpRootPane = mLyt.getRootPane();
    mLyt.WPaneRegister(WPANE_NAME_DT, mpWnd, W_COUNT);

    mCenterPos.x = 0.0f;
    mCenterPos.y = 0.0f;
    mIsCreated = true;
    mHasTarget = false;
    mUseCenterPos = false;

    mLyt.AllAnimeEndSetup();

    mpRootPane->setVisible(false);
    mLyt.mDrawOrder = 154;
    mAction = IDLE;

    return true;
}

typedef void (dWipeCircle_c::*actMeth)();

int dWipeCircle_c::calc() {
    static const actMeth Proc_tbl[ACTION_COUNT] = {
        dWipeCircle_c::OpenSetup,
        dWipeCircle_c::AnimeEndCheck,
        dWipeCircle_c::CloseSetup
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

void dWipeCircle_c::draw() {
    if (mIsCreated) {
        mLyt.entry();
    }
}

// [Fakematch]
#pragma push
#pragma ppc_iro_level 2

void dWipeCircle_c::CenterPosSet() {
    mVec2_c centerPos(0.0f, 0.0f);
    mpRootPane->mPos = mVec3_c(0.0f, 0.0f, 0.0f);
    if (!mHasTarget) {
        return;
    }

    mHasTarget = false;
    if (mUseCenterPos) {
        mUseCenterPos = false;
        centerPos.x = mCenterPos.x;
        centerPos.y = mCenterPos.y;
    } else {
        int targetPlayerNo = daPyDemoMng_c::mspInstance->mPlNo;
        if (dScStage_c::m_KoopaJrEscape) {
            targetPlayerNo = 0;
        }
        if (targetPlayerNo < 0) {
            return;
        }

        dAcPy_c *targetPlayer = daPyMng_c::getPlayer(targetPlayerNo);
        if (targetPlayer == nullptr) {
            return;
        }

        static float PLAYER_OFFSET_Y_DATA_TBL[PLAYER_COUNT][POWERUP_COUNT] = {
            {
                dfukidashiInfo_c::c_MARIO_NORMAL_OFFSET_Y,
                dfukidashiInfo_c::c_MARIO_SUPER_OFFSET_Y,
                dfukidashiInfo_c::c_MARIO_FIRE_OFFSET_Y,
                dfukidashiInfo_c::c_MARIO_MAME_OFFSET_Y,
                dfukidashiInfo_c::c_MARIO_PROPEL_OFFSET_Y,
                dfukidashiInfo_c::c_MARIO_PENGUIN_OFFSET_Y,
                dfukidashiInfo_c::c_MARIO_ICE_OFFSET_Y
            },
            {
                dfukidashiInfo_c::c_LUIGI_NORMAL_OFFSET_Y,
                dfukidashiInfo_c::c_LUIGI_SUPER_OFFSET_Y,
                dfukidashiInfo_c::c_LUIGI_FIRE_OFFSET_Y,
                dfukidashiInfo_c::c_LUIGI_MAME_OFFSET_Y,
                dfukidashiInfo_c::c_LUIGI_PROPEL_OFFSET_Y,
                dfukidashiInfo_c::c_LUIGI_PENGUIN_OFFSET_Y,
                dfukidashiInfo_c::c_LUIGI_ICE_OFFSET_Y
            },
            {
                dfukidashiInfo_c::c_KINOPIO_NORMAL_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_SUPER_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_FIRE_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_MAME_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_PROPEL_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_PENGUIN_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_ICE_OFFSET_Y
            },
            {
                dfukidashiInfo_c::c_KINOPIO_NORMAL_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_SUPER_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_FIRE_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_MAME_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_PROPEL_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_PENGUIN_OFFSET_Y,
                dfukidashiInfo_c::c_KINOPIO_ICE_OFFSET_Y
            }
        };
        int targetPlayerType = daPyMng_c::getPlayerType(targetPlayerNo);
        int targetPlayerMode = daPyMng_c::getPlayerMode(targetPlayerType);

        mVec3_c targetPos;
        targetPos.x = targetPlayer->mPos.x;
        float offs = PLAYER_OFFSET_Y_DATA_TBL[targetPlayerType][targetPlayerMode];
        targetPos.y = targetPlayer->mPos.y + offs * 0.5f;
        targetPos.z = targetPlayer->mPos.z;
        dGameCom::getGlbPosToLyt(targetPos);
        centerPos.x = targetPos.x;
        centerPos.y = targetPos.y;
    }

    mpRootPane->mPos = mVec3_c(centerPos, 0.0f);
}

#pragma pop

void dWipeCircle_c::OpenSetup() {
    mLyt.AllAnimeEndSetup();
    mLyt.AnimeStartSetup(outWindow, false);
    mAction = ANIME_END_CHECK;
    CenterPosSet();
}

void dWipeCircle_c::AnimeEndCheck() {
    if (!mLyt.isAnime(-1)) {
        if (getStatus() == FADE_IN) {
            mStatus = HIDDEN;
            mFlag |= FADE_IN_COMPLETE;
            mpRootPane->setVisible(false);
        } else {
            mStatus = OPAQUE;
            mFlag |= FADE_OUT_COMPLETE;
        }
        mCenterPos.x = 0.0f;
        mCenterPos.y = 0.0f;
        mAction = IDLE;
    }
}

void dWipeCircle_c::CloseSetup() {
    mLyt.AllAnimeEndSetup();
    mLyt.AnimeStartSetup(inWindow, false);
    mAction = ANIME_END_CHECK;
    CenterPosSet();
}

void dWipeCircle_c::setStatus(mFaderBase_c::EStatus status) {
    if (status == OPAQUE) {
        mStatus = OPAQUE;
        mpWnd[W_circle_00]->mAlpha = 255;
        mLyt.ReverseAnimeStartSetup(outWindow, false);
        mpRootPane->setVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    } else if (status == HIDDEN) {
        mStatus = HIDDEN;
        mpWnd[W_circle_00]->mAlpha = 0;
        mLyt.ReverseAnimeStartSetup(inWindow, false);
        mpRootPane->setVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    }
}

bool dWipeCircle_c::fadeIn() {
    bool res = mFaderBase_c::fadeIn();
    if (res) {
        mAction = OPEN_SETUP;
    }
    return res;
}

bool dWipeCircle_c::fadeOut() {
    bool res = mFaderBase_c::fadeOut();
    if (res) {
        mAction = CLOSE_SETUP;
    }
    return res;
}
