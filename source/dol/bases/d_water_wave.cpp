#include <game/bases/d_water_wave.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_s_stage.hpp>
#include <game/mLib/m_heap.hpp>
#include <game/sLib/s_GlobalData.hpp>

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

template<>
const dWaterWave_c::GlobalData_t sGlobalData_c<dWaterWave_c>::m_member = {
    {
        {255, 255, 255, 255},
        {255, 255, 0, 255},
        {255, 255, 135, 255},
        {255, 255, 255, 255},
        {255, 0, 0, 255},
        {200, 0, 0, 255},
        {255, 255, 255, 255},
        {255, 0, 255, 255},
        {100, 0, 100, 255},
        {50, 0, 50, 255},
    },
    {
        0.25f, 0.04f, 0.001f, 0.1f,
        0.002f, 0.001f, 0.1f, 0.002f,
        0.001f, 0.25f, 0.08f, 0.001f
    },
    {
        {1.0f, -2000, 14000 },
        {1.0f, 1900, 9000 },
        {1.0f, -900, 18000 },
        {1.0f, -800, 10000 },
        {1.0f, -1500, 1300 },
        {1.0f, 1400, 9000 },
    },
    {
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
    },
    { 255, 255, 255, 255 },
    { 0, 255, 255, 255 }
};

void dWaterWave_c::wave_tex_set(mVec3_c *pos, float f, int i, u8 a) {
    dScStage_c *stage = dScStage_c::getInstance();

    mVec3_c waveQuadPos[4];

    m_d308 = a;
    mPos = *pos;
    m_d284 = f / 8.0f;
    m_d309 = (u8) f % 8;
    m_d2e8 = i;
    m_d30a = 0;

    if (
        stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_3 ||
        stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_CASTLE && stage->mCurrFile == 3 ||
        stage->mCurrWorld == WORLD_2 && stage->mCurrCourse == STAGE_CASTLE && stage->mCurrFile == 0
    ) {
        a = 3;
    }
    if (
        stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_3
    ) {
        m_d30a = 1;
    }
    if (
        stage->mCurrWorld == WORLD_8 && stage->mCurrCourse == STAGE_CASTLE && stage->mCurrFile == 3
    ) {
        m_d30a = 2;
    }
    mAllocator1.createFrmHeap(-1, mHeap::g_gameHeaps[0], nullptr, 0x20);
    mRes1 = dResMng_c::m_instance->getRes(l_bgpoly_arcdt[a], l_bgpoly_bressdt[a]);
    if (m_d308 == 1 || m_d308 == 2) {
        mRes2 = dResMng_c::m_instance->getRes(l_bgpoly_decarcdt[a], l_bgpoly_decbressdt[a]);
    }

    waveQuadPos[0].set(0.0f, 0.0f, 0.0f);
    waveQuadPos[1].set(8.0f, 0.0f, 0.0f);
    waveQuadPos[2].set(8.0f, -8.0f, 0.0f);
    waveQuadPos[3].set(0.0f, -8.0f, 0.0f);

    mWaveQuad.create(
        m_d308,
        mRes1.GetResTex(l_bgpoly_texdt[a]),
        mRes1.GetResTex(l_bgpoly_texdt2[a]),
        -1,
        133,
        &mAllocator1
    );
    GXColor color1, color2;
    nw4r::g3d::ResMdl mdl = mRes1.GetResMdl(l_bgpoly_modeldt[a]);
    nw4r::g3d::ResMat mat = mdl.GetResMat(0);
    nw4r::g3d::ResMatTevColor col = mat.GetResMatTevColor();
    col.GXGetTevColor(GX_TEVREG0, &color1);
    color1.a = 255;
    mWaveQuad.mColor1 = nw4r::ut::Color(color1);
    col.GXGetTevColor(GX_TEVREG1, &color1);
    color1.a = 255;
    mWaveQuad.mColor2 = nw4r::ut::Color(color1);
    if (m_d308 == 0) {
        GXColor col1 = sGlobalData_c<dWaterWave_c>::m_member.mWaterTexColor1;
        GXColor col2 = sGlobalData_c<dWaterWave_c>::m_member.mWaterTexColor2;
        mWaveQuad.mColor1 = nw4r::ut::Color(col1);
        mWaveQuad.mColor2 = nw4r::ut::Color(col2);
    }
    mVec3_c v2;
    mVec3_c v1(0.0f, 0.0f, 0.0f);
    m_d2fc = m_d284;
    if (m_d2fc > 1100) {
        m_d2fc = 1100;
    }
    mWaveQuad.mCount = m_d2fc;
    for (int i = 0; i < m_d2fc; i++) {
        mWaveQuad.setOfs(waveQuadPos, i);
        waveQuadPos[0].x += 8.0f;
        waveQuadPos[1].x += 8.0f;
        waveQuadPos[2].x += 8.0f;
        waveQuadPos[3].x += 8.0f;
    }
    for (int i = 0; i < 20; i++) {
        mSub[i].m_964 = 0;
        mSub[i].m_968 = 0;
        mSub[i].m_a34 = 0;
        mSub[i].m_640 = 1.0f;
        for (int j = 0; j < 200; j++) {
            mSub[i].m_00[j] = 0.0f;
            mSub[i].m_320[j] = 1.0f;
            mSub[i].m_644[j] = 0;
            mSub[i].m_96c[j] = 0;
        }
    }
    if (m_d2e8 == 0) {
        v2.x = dBg_c::m_bg_p->getLeft();
    } else {
        v2.x = mPos.x;
    }
    v2.y = mPos.y;
    v2.z = mPos.z;
    mMtxWave.trans(v2.x, v2.y, v2.z);
    mWaveQuad.setLocalMtx(&mMtxWave);

    if (m_d308 == 1 || m_d308 == 2) {
        mWaveDeco.create(
            mRes2.GetResTex(l_bgpoly_decotexdt[0]),
            -1,
            133,
            &mAllocator1
        );
        mdl = mRes2.GetResMdl(l_bgpoly_decarcdt[a]);
        mat = mdl.GetResMat(0);
        col = mat.GetResMatTevColor();
        col.GXGetTevColor(GX_TEVREG0, &color1);
        color1.a = 255;
        col.GXGetTevColor(GX_TEVREG1, &color2);
        color2.a = 255;
        mMtxDeco.trans(v2.x, 0.0f, 0.0f);
        mWaveDeco.setLocalMtx(&mMtxDeco);
        for (int i = 0; i < 20; i++) {
            mWaveDeco.mVertices[i].set(v1.x, v1.y, v1.z);
            mWaveDeco.m_2c8[i] = 0;
            mWaveDeco.m_278[i] = dGameCom::rndF(2.5f) + 7.5f;
            mWaveDeco.mObjIndices[i] = (float) (i % 12);
            mWaveDeco.m_2f0[i] = nw4r::ut::Color(color1);
            mWaveDeco.m_340[i] = nw4r::ut::Color(color2);
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
