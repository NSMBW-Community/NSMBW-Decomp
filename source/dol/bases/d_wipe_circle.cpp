#include <game/bases/d_wipe_circle.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_a_player_demo_manager.hpp>
#include <game/bases/d_fukidashi_manager.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_s_stage.hpp>

dWipeCircle_c *dWipeCircle_c::m_instance;

dWipeCircle_c::dWipeCircle_c(nw4r::ut::Color color, mFaderBase_c::EStatus stat) :
mFaderBase_c(mColor(color.rgba), stat) {
    m_instance = this;
    mIsCreated = false;
}

dWipeCircle_c::~dWipeCircle_c() {
    m_instance = nullptr;
}

bool dWipeCircle_c::createLayout() {
    static const char *AnmNameTbl[] = {
        "wipeCircle_00_inWindow.brlan",
        "wipeCircle_00_outWindow.brlan"
    };

    static const char *GROUP_NAME_DT[] = {
        "A00_inWindow",
        "B00_outWindow"
    };
    static const int ANIME_INDEX_TBL[] = {0, 1};

    static const char *WPANE_NAME_DT[] = {
        "W_circle_00"
    };

    if (mIsCreated) {
        return true;
    }

    bool res = mLyt.ReadResource("wipeCircle/wipeCircle.arc", false);
    if (!res) {
        return false;
    }

    mLyt.build("wipeCircle_00.brlyt", 0);
    mLyt.AnimeResRegister(AnmNameTbl, 2);
    mLyt.GroupRegister(GROUP_NAME_DT, ANIME_INDEX_TBL, 2);
    mpRootPane = mLyt.getRootPane();
    mLyt.WPaneRegister(WPANE_NAME_DT, mpWnd, 1);

    mCenterPos.x = 0.0;
    mCenterPos.y = 0.0;
    mIsCreated = true;
    mField_1c1 = false;
    mField_1c2 = false;

    mLyt.AllAnimeEndSetup();

    mpRootPane->setVisible(false);
    mLyt.mDrawOrder = 154;
    mAction = 3;

    return true;
}

typedef void (dWipeCircle_c::*actMeth)();

int dWipeCircle_c::calc() {
    static const actMeth Proc_tbl[] = {
        dWipeCircle_c::OpenSetup,
        dWipeCircle_c::AnimeEndCheck,
        dWipeCircle_c::CloseSetup
    };
    if (!mIsCreated) {
        return 1;
    }
    if (mAction != 3) {
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
    mVec2_c newPos(0.0f, 0.0f);
    mpRootPane->mPos = mVec3_c(0.0f, 0.0f, 0.0f);
    if (!mField_1c1) {
        return;
    }
    mField_1c1 = false;
    if (mField_1c2) {
        mField_1c2 = false;
        newPos.x = mCenterPos.x;
        newPos.y = mCenterPos.y;
    } else {
        int plNo = daPyDemoMng_c::mspInstance->mPlNo;
        if (dScStage_c::m_KoopaJrEscape) {
            plNo = 0;
        }
        if (plNo < 0) {
            return;
        }
        dBaseActor_c *player = daPyMng_c::getPlayer(plNo);
        if (player == nullptr) {
            return;
        }
        static float PLAYER_OFFSET_Y_DATA_TBL[4][7] = {
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
        int plType = daPyMng_c::getPlayerType(plNo);
        int plMode = daPyMng_c::getPlayerMode(plType);

        mVec3_c pos;
        pos.x = player->mPos.x;
        float offs = PLAYER_OFFSET_Y_DATA_TBL[plType][plMode];
        pos.y = player->mPos.y + offs * 0.5f;
        pos.z = player->mPos.z;
        dGameCom::getGlbPosToLyt(pos);
        newPos.x = pos.x;
        newPos.y = pos.y;
    }
    mpRootPane->mPos = mVec3_c(newPos, 0.0f);
}

#pragma pop

void dWipeCircle_c::OpenSetup() {
    mLyt.AllAnimeEndSetup();
    mLyt.AnimeStartSetup(1, false);
    mAction = 1;
    CenterPosSet();
}

void dWipeCircle_c::AnimeEndCheck() {
    if (!mLyt.isAnime(-1)) {
        if (getStatus() == FADE_IN) {
            mStatus = HIDDEN;
            mFlag |= 1;
            mpRootPane->mFlags &= ~1;
        } else {
            mStatus = OPAQUE;
            mFlag |= 2;
        }
        mCenterPos.x = 0;
        mCenterPos.y = 0;
        mAction = 3;
    }
}

void dWipeCircle_c::CloseSetup() {
    mLyt.AllAnimeEndSetup();
    mLyt.AnimeStartSetup(0, false);
    mAction = 1;
    CenterPosSet();
}

void dWipeCircle_c::setStatus(mFaderBase_c::EStatus stat) {
    if (stat == OPAQUE) {
        mStatus = OPAQUE;
        mpWnd[0]->mAlpha = -1;
        mLyt.ReverseAnimeStartSetup(1, false);
        mpRootPane->setVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    } else if (stat == HIDDEN) {
        mStatus = HIDDEN;
        mpWnd[0]->mAlpha = 0;
        mLyt.ReverseAnimeStartSetup(0, false);
        mpRootPane->setVisible(true);
        mLyt.AnimePlay();
        mLyt.calc();
    }
}

bool dWipeCircle_c::fadeIn() {
    bool res = mFaderBase_c::fadeIn();
    if (res) {
        mAction = 0;
    }
    return res;
}

bool dWipeCircle_c::fadeOut() {
    bool res = mFaderBase_c::fadeOut();
    if (res) {
        mAction = 2;
    }
    return res;
}
