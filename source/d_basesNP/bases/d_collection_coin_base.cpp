#include <game/bases/d_collection_coin_base.hpp>
#include <game/framework/f_profile.hpp>
#include <game/framework/f_profile_name.hpp>
#include <game/bases/d_game_com.hpp>

CUSTOM_BASE_PROFILE(COLLECTION_COIN_BASE, dCollectionCoinBase_c, fProf::COLLECTION_COIN_BASE, fProf::DRAW_ORDER::COLLECTION_COIN_BASE);

dCollectionCoinBase_c::dCollectionCoinBase_c() : mIsCreated(false) {}

dCollectionCoinBase_c::~dCollectionCoinBase_c() {}

int dCollectionCoinBase_c::create() {
    if (mIsCreated) {
        return SUCCEEDED;
    }

    if (!createLayout()) {
        return NOT_READY;
    }

    mpRootPane->SetVisible(true);
    mLayout.mDrawOrder = m2d::DRAW_ORDER_COLLECTION_COIN_BASE;
    mIsCreated = true;
    mIsVisible = false;

    return SUCCEEDED;
}

bool dCollectionCoinBase_c::createLayout() {

    static const char *N_PANE_NAME_TBL[] = {
        "N_coinDateAll_00",
        "N_coinBarPos_00", "N_coinBarPos_01", "N_coinBarPos_02",
        "N_coinBarPos_03", "N_coinBarPos_04", "N_coinBarPos_05",
        "N_coinBarPos_06", "N_coinBarPos_07", "N_coinBarPos_08",
        "N_coinBarPos_09", "N_coinBarPos_10", "N_coinBarPos_11"
    };

    if (mLayout.mpResAccessor == nullptr) {
        return false;
    }

    mLayout.build("worldCollectionCoinBase_01.brlyt", nullptr);
    mpRootPane = mLayout.getRootPane();
    mLayout.NPaneRegister(N_PANE_NAME_TBL, mpNullPanes, ARRAY_SIZE(N_PANE_NAME_TBL));
    return true;
}

int dCollectionCoinBase_c::preExecute() {
    if (dBase_c::preExecute() == NOT_READY) {
        return NOT_READY;
    }

    return !dGameCom::isGameStop(-1);
}

int dCollectionCoinBase_c::execute() {
    if (mIsCreated && mIsVisible) {
        mpRootPane->SetVisible(true);
        mLayout.calc();
        setDatePos();
    } else {
        mpRootPane->SetVisible(false);
    }

    return SUCCEEDED;
}

int dCollectionCoinBase_c::draw() {
    if (mIsCreated && mIsVisible) {
        mLayout.entry();
    }

    return SUCCEEDED;
}

int dCollectionCoinBase_c::doDelete() {
    return mLayout.doDelete();
}

void dCollectionCoinBase_c::setDatePos() {
    for (int i = 0; i < STAR_COIN_MENU_LEVEL_COUNT; i++) {
        nw4r::lyt::Pane *N_coinBarPos = mpNullPanes[N_coinBarPos_00+i];
        nw4r::lyt::Pane *rootPane = mpDates[i]->mpRootPane;

        nw4r::math::MTX34 mtx = N_coinBarPos->GetGlobalMtx();
        float transZ = 0.0f;
        float scaleX = mtx[0][0];
        float scaleY = mtx[1][1];
        float transY = mtx[1][3];
        float transX = mtx[0][3];

        nw4r::math::VEC3 trans(transX, transY, transZ);
        rootPane->SetTranslate(trans);

        nw4r::math::VEC2 scale(scaleX, scaleY);
        rootPane->SetScale(scale);

        u8 alpha = mpNullPanes[N_coinDateAll_00]->GetAlpha();
        mpDates[i]->N_coinBarDate_00->SetAlpha(alpha);
    }
}
