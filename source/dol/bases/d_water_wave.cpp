#include <game/bases/d_water_wave.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/mLib/m_heap.hpp>

static const char *l_bgpoly_arcdt[] = {
    "obj_waterhalf",
    "obj_magma",
    "obj_poisonwater",
    "obj_magmawave",
    "obj_quicksand"
};

static const char *l_bgpoly_bressdt[] = {
    "g3d/obj_waterhalf.brres",
    "g3d/obj_magma.brres",
    "g3d/obj_poisonwater.brres",
    "g3d/obj_magmawave.brres",
    "g3d/obj_quicksand.brres"
};

static const char *l_bgpoly_texdt[] = {
    "wmy_waterhalf_top01",
    "wmy_objmagma01",
    "wmy_objpoison01",
    "wmy_objmagmawave01",
    "wmy_quicksand00"
};

static const char *l_bgpoly_texdt2[] = {
    "wmy_waterhalf_top02",
    "wmy_objmagma03",
    "wmy_objpoison03",
    "wmy_objmagmawave02",
    "wmy_quicksand_gr"
};

static const char *l_bgpoly_modeldt[] = {
    "obj_waterhalf_top",
    "obj_magma_top",
    "obj_poisonwater_top",
    "obj_magmawave_top",
    "obj_quicksand_top"
};

static const char *l_bgpoly_decarcdt[] = {
    "obj_magmadeco",
    "obj_magmadeco",
    "obj_magmadeco",
    "obj_magmadeco",
    "obj_magmadeco"
};

static const char *l_bgpoly_decbressdt[] = {
    "g3d/obj_magmadeco.brres",
    "g3d/obj_magmadeco.brres",
    "g3d/obj_magmadeco.brres",
    "g3d/obj_magmadeco.brres",
    "g3d/obj_magmadeco.brres"
};

static const char *l_bgpoly_decotexdt[] = {
    "wmy_magmadeco.0",
    "wmy_magmadeco.1",
    "wmy_magmadeco.2",
    "wmy_magmadeco.2",
    "wmy_magmadeco.3",
    "wmy_magmadeco.4",
    "wmy_magmadeco.5",
    "wmy_magmadeco.6",
    "wmy_magmadeco.7",
    "wmy_magmadeco.8",
    "wmy_magmadeco.9",
    "wmy_magmadeco.10",
    "wmy_magmadeco.11"
};

void dWaterWave_c::wave_tex_set(mVec3_c *pos, float f, int i, u8 a) {
    dScStage_c *stage = dScStage_c::m_instance;

    mPos = *pos;
    m_d30a = 0;
    u8 idx = i;

    if (
        stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_3 ||
        stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_CASTLE && stage->mCurrFile == 3 ||
        stage->mCurrWorld == WORLD_2 && stage->mCurrCourse == STAGE_CASTLE && stage->mCurrFile == 0
    ) {
        idx = 3;
    }
    if (
        stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_2
    ) {
        m_d30a = 1;
    }
    if (
        stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_CASTLE && stage->mCurrFile == 3
    ) {
        m_d30a = 2;
    }
    mAllocator1.createFrmHeap(-1, mHeap::g_gameHeaps[0], nullptr, 0x20);
    mRes1 = dResMng_c::m_instance->getRes(l_bgpoly_arcdt[idx], l_bgpoly_bressdt[idx]);
    mRes2 = dResMng_c::m_instance->getRes(l_bgpoly_decarcdt[idx], l_bgpoly_decbressdt[idx]);
    mWaveQuad.create(
        m_d308,
        mRes1.GetResTex(l_bgpoly_texdt[idx]),
        mRes1.GetResTex(l_bgpoly_texdt2[idx]),
        -1,
        133,
        &mAllocator1
    );
    nw4r::g3d::ResMat mat = mRes1.GetResMdl(l_bgpoly_modeldt[idx]).GetResMat(0);
    GXColor color;
    mat.GetResMatTevColor().GXGetTevColor(GX_TEVREG0, &color);
    mWaveQuad.mColor1 = color;
    mat.GetResMatTevColor().GXGetTevColor(GX_TEVREG1, &color);
    mWaveQuad.mColor2 = color;
    if (m_d308 == 0) {
        mWaveQuad.mColor1 = (GXColor){255, 255, 255, 255};
        mWaveQuad.mColor2 = (GXColor){0, 255, 255, 255};
    }
    m_d2fc = m_d284;
    if (m_d2fc > 1100) {
        m_d2fc = 1100;
    }
    mWaveQuad.mCount = m_d2fc;
    mVec3_c someVec(0.0f, 0.0f, 0.0f);
    mVec3_c v;
    for (int i = 0; i < m_d2fc; i++) {
        mWaveQuad.setOfs(&v, i);
        v.x += 8;
    }
    for (int i = 0; i < ARRAY_SIZE(mSub); i++) {
        mSub[i].m_964 = 0;
        mSub[i].m_968 = 0;
        mSub[i].m_a34 = 0;
        mSub[i].m_640 = 1.0f;
        for (int j = 0; j < ARRAY_SIZE(mSub[i].m_00); j++) {
            mSub[i].m_00[j] = 0;
            mSub[i].m_320[j] = 1.0f;
            mSub[i].m_644[j] = 1.0f;
            mSub[i].m_96c[j] = 0;
        }
    }
    mVec3_c v2;
    if (m_d2e8 == 0) {
        v2.x = dBg_c::m_bg_p->mLoopOffset;
    } else {
        v2.x = mPos.x;
    }
    v2.y = mPos.y;
    v2.z = mPos.z;
    mMtx_c m = mMtx_c::createTrans(v2);
    mWaveQuad.setLocalMtx(&m);

    if (m_d308 == 1 || m_d308 == 2) {
        mWaveDeco.create(
            mRes2.GetResTex(l_bgpoly_decotexdt[0]),
            -1,
            133,
            &mAllocator1
        );
        nw4r::g3d::ResMat mat = mRes2.GetResMdl(l_bgpoly_decarcdt[idx]).GetResMat(0);
        GXColor color1;
        mat.GetResMatTevColor().GXGetTevColor(GX_TEVREG0, &color1);
        GXColor color2;
        mat.GetResMatTevColor().GXGetTevColor(GX_TEVREG1, &color2);
        mMtx.trans(v2.x, 0.0f, 0.0f);
        mWaveDeco.setLocalMtx(&mMtx);
        for (int i = 0; i < 20; i++) {
            mWaveDeco.mVertices[i] = someVec;
            mWaveDeco.m_2c8[i] = 0;
            mWaveDeco.m_278[i] = dGameCom::rndF(2.5f) + 7.5f;
            mWaveDeco.mObjIndices[i] = i / 12;
            mWaveDeco.m_2f0[i] = color1;
            mWaveDeco.m_340[i] = color2;
        }
        for (int i = 0; i < 12; i++) {
            mWaveDeco.setTexObj(i, mRes2.GetResTex(l_bgpoly_decotexdt[i]));
        }
    }

    mAllocator1.adjustFrmHeap();
}

void dWaterWave_c::waveDecoExecute() {

}

void dWaterWave_c::waveExecute() {

}

void dWaterWave_c::draw() {

}

void dWaterWave_c::doDelete() {

}
