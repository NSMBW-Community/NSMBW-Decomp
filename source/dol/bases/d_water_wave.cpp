#include <game/bases/d_water_wave.hpp>
#include <game/bases/d_a_player_manager.hpp>
#include <game/bases/d_bg.hpp>
#include <game/bases/d_bg_parameter.hpp>
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
        {1.0f, -2000, 14000 , 1.0f, 1900, 9000 },
        {1.0f, -900, 18000, 1.0f, -800, 10000 },
        {1.0f, -1500, 1300, 1.0f, 1400, 9000 },
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
    mAllocatorWave.createFrmHeap(-1, mHeap::g_gameHeaps[0], nullptr, 0x20);
    mResWave = dResMng_c::m_instance->getRes(l_bgpoly_arcdt[a], l_bgpoly_bressdt[a]);
    if (m_d308 == 1 || m_d308 == 2) {
        mResDeco = dResMng_c::m_instance->getRes(l_bgpoly_decarcdt[a], l_bgpoly_decbressdt[a]);
    }

    waveQuadPos[0].set(0.0f, 0.0f, 0.0f);
    waveQuadPos[1].set(8.0f, 0.0f, 0.0f);
    waveQuadPos[2].set(8.0f, -8.0f, 0.0f);
    waveQuadPos[3].set(0.0f, -8.0f, 0.0f);

    mWaveQuad.create(
        m_d308,
        mResWave.GetResTex(l_bgpoly_texdt[a]),
        mResWave.GetResTex(l_bgpoly_texdt2[a]),
        -1,
        133,
        &mAllocatorWave
    );
    GXColor color1, color2;
    nw4r::g3d::ResMdl mdl = mResWave.GetResMdl(l_bgpoly_modeldt[a]);
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
            mResDeco.GetResTex(l_bgpoly_decotexdt[0]),
            -1,
            133,
            &mAllocatorWave
        );
        mdl = mResDeco.GetResMdl(l_bgpoly_decarcdt[a]);
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
            mWaveDeco.mState[i] = 0;
            mWaveDeco.m_278[i] = dGameCom::rndF(2.5f) + 7.5f;
            mWaveDeco.mObjIndices[i] = (float) (i % 12);
            mWaveDeco.mColor1[i] = nw4r::ut::Color(color1);
            mWaveDeco.mColor2[i] = nw4r::ut::Color(color2);
        }
        for (int i = 0; i < 12; i++) {
            mWaveDeco.setTexObj(i, mResDeco.GetResTex(l_bgpoly_decotexdt[i]));
        }
    }

    mAllocatorWave.adjustFrmHeap();
}

void dWaterWave_c::waveDecoExecute() {
    dBgParameter_c *bgParam = dBgParameter_c::getInstance();

    float scale = 8.0f;

    int count = 20;
    if (m_d284 < 20.0f) {
        count = m_d284;
    }

    int i = 0;
    if (count > 0) {
        for (; i < count; i++) {
            int frameIdx = (i + dScStage_c::m_exeFrame) % 5;
            s8 state = mWaveDeco.mState[i];
            switch (state) {
                case 0: {
                    mWaveDeco.mVertices[i].x = mPos.x + scale + (m_d284 * 2.0f - 2.0 * scale) * dGameCom::rndF(1.0f);
                    mWaveDeco.mVertices[i].y = -2.5f;
                    mWaveDeco.mVertices[i].z = 0.0f;
                    mWaveDeco.mState[i] = 1;
                    break;
                }
                case 1:
                    mVec3_c copy;
                    copy = *mWaveDeco.getVertex(i);
                    mVec3_c *quad = mWaveQuad.getQuad(i);
                    if (quad != nullptr) {
                        copy.y = quad->y - 5.0f;
                    }
                    if (copy.x < bgParam->xStart()) {
                        if (copy.x + bgParam->xSize() < mPos.x + m_d284 * 8.0f) {
                            copy.x += bgParam->xSize() - 2.0f * scale;
                        }
                    } else {
                        if (copy.x > bgParam->xStart() + bgParam->xSize() && copy.x - bgParam->xSize() > mPos.x) {
                            copy.x -= bgParam->xSize() - 2.0f * scale;
                        }
                    }
                    mWaveDeco.mVertices[i] = copy;
                    if (frameIdx == 0) {
                        float nextIndex = mWaveDeco.mObjIndices[i] + 1;
                        mWaveDeco.mObjIndices[i] = nextIndex;
                        if (mWaveDeco.mObjIndices[i] > 11) {
                            mWaveDeco.mVertices[i].x = mPos.x + scale + (m_d284 * 8.0f - 2.0f * scale) * dGameCom::rndF(1.0f);
                            mWaveDeco.mVertices[i].y = -5.0f;
                            mWaveDeco.mVertices[i].z = 0.0f;
                            mWaveDeco.mObjIndices[i] = 0;
                        }
                    }
                    break;
            }
            int r1Min, g1Min, b1Min;
            int r1Mid, g1Mid, b1Mid;
            int r1Max, g1Max, b1Max;
            int r2Min, g2Min, b2Min;
            int r2Mid, g2Mid, b2Mid;
            int r2Max, g2Max, b2Max;

            bool pastMid = false;

            r1Min = 255; g1Min = 255; b1Min = 255; // #FFFFFF
            r1Mid = 255; g1Mid = 255; b1Mid = 0;   // #FFFF00
            r1Max = 255; g1Max = 135; b1Max = 0;   // #FF8700

            r2Min = 255; g2Min = 255; b2Min = 255; // #FFFFFF
            r2Mid = 255; g2Mid = 0; b2Mid = 0;     // #FF0000
            r2Max = 200; g2Max = 0; b2Max = 0;     // #C80000

            float tFull = (mWaveDeco.mObjIndices[i] * 4 + frameIdx) / 48.0f;
            if (m_d308 == 2) {
                r1Min = 255; g1Min = 255; b1Min = 255; // #FFFFFF
                r1Mid = 255; g1Mid = 0; b1Mid = 255;   // #FF00FF
                r1Max = 100; g1Max = 0; b1Max = 100;   // #640064

                r2Min = 255; g2Min = 0; b2Min = 255;   // #FF00FF
                r2Mid = 100; g2Mid = 0; b2Mid = 100;   // #640064
                r2Max = 50; g2Max = 0; b2Max = 50;     // #320032
            }

            float tScaled;
            if (tFull < 0.5f) {
                tScaled = tFull * 2.0f;
            } else {
                tScaled = (tFull - 0.5f) * 2.0f;
                pastMid = true;
            }
            GXColor c1, c2;
            if (!pastMid) {
                c1.r = r1Min + (int) (tScaled * (r1Mid - r1Min));
                c1.g = g1Min + (int) (tScaled * (g1Mid - g1Min));
                c1.b = b1Min + (int) (tScaled * (b1Mid - b1Min));
                c1.a = 255;
                c2.r = r2Min + (int) (tScaled * (r2Mid - r2Min));
                c2.g = g2Min + (int) (tScaled * (g2Mid - g2Min));
                c2.b = b2Min + (int) (tScaled * (b2Mid - b2Min));
                c2.a = 255;
            } else {
                c1.r = r1Mid + (int) (tScaled * (r1Max - r1Mid));
                c1.g = g1Mid + (int) (tScaled * (g1Max - g1Mid));
                c1.b = b1Mid + (int) (tScaled * (b1Max - b1Mid));
                c1.a = 255;
                c2.r = r2Mid + (int) (tScaled * (r2Max - r2Mid));
                c2.g = g2Mid + (int) (tScaled * (g2Max - g2Mid));
                c2.b = b2Mid + (int) (tScaled * (b2Max - b2Mid));
                c2.a = 255;
            }
            mWaveDeco.mColor1[i] = nw4r::ut::Color(c1);
            mWaveDeco.mColor2[i] = nw4r::ut::Color(c2);
        }
    }

    mPos2 = mPos;
    mPos2.x = 0.0f;
    mPos2.z = mPos.z + 8.0f;

    mMtxDeco.trans(mPos2);
    mMtxDeco.concat(mMtx_c::createScale(1.0f));

    mWaveDeco.setLocalMtx(&mMtxDeco);
}

void dWaterWave_c::waveExecute() {
    float scale1 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].f1;
    float scale2 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].f2;

    dBgParameter_c *bgParam = dBgParameter_c::getInstance();
    dScStage_c *stage = dScStage_c::getInstance();

    int aInc = 0;
    int bInc = 0;
    int waveCounter = 0;
    int waveCounter2 = 0;
    int waveIndex = 0;
    int quadI;
    int i;

    mVec3_c waveQuadPos[4];
    mVec3_c t;

    short ang2 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].s2;
    short ang1 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].s1;
    short ang4 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].s4;
    short ang3 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].s3;

    if (m_d30a == 0 && mPos.y < bgParam->yStart() - bgParam->ySize() - 16.0f) {
        return;
    }

    if (m_d308 == 1) {
        scale1 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[1].f1;
        scale2 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[1].f2;
        ang1 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[1].s1;
        ang2 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[1].s2;
        ang3 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[1].s3;
        ang4 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[1].s4;
        if (m_d30a == 2) {
            scale1 = 2.0f;
            ang1 = 10000;
            ang2 = 5000;
            ang4 = 1000;
        }
        if ((dActor_c::mExecStop & 1) == 0) {
            short delta = mWaveQuad.mAngle1;
            if (m_d30a == 0) {
                delta += 5;
            } else {
                delta += 2;
            }
            if (delta > 0x400) {
                delta -= 0x400;
            }
            mWaveQuad.mAngle1 = delta;
        }
    } else if (m_d308 == 2) {
        scale1 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[2].f1;
        scale2 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[2].f2;
        ang1 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[2].s1;
        ang2 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[2].s2;
        ang3 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[2].s3;
        ang4 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[2].s4;
        if ((dActor_c::mExecStop & 1) == 0) {
            mAng delta = mWaveQuad.mAngle1 + mAng(7);
            if (delta > 0x400) {
                delta -= 0x400;
            }
            mWaveQuad.mAngle1 = delta;
        }
    } else if (m_d308 == 4) {
        scale1 = 1.5f;
        // scale2 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].f2;
        ang1 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].s1;
        ang2 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].s2;
        ang3 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].s3;
        ang4 = sGlobalData_c<dWaterWave_c>::m_member.mStructs[0].s4;
        if ((dActor_c::mExecStop & 1) == 0) {
            mAng delta = mWaveQuad.mAngle1 - mAng(7);
            if (delta < -0x400) {
                delta += 0x400;
            }
            mWaveQuad.mAngle1 = delta;
        }
    } else {
        if ((dActor_c::mExecStop & 1) == 0) {
            mAng delta = mWaveQuad.mAngle1 + mAng(-4);
            if (delta < -0x400) {
                delta += 0x400;
            }
            mWaveQuad.mAngle1 = delta;
            mAng delta2 = mWaveQuad.mAngle2 + mAng(2);
            if (delta2 > 0x400) {
                delta2 -= 0x400;
            }
            mWaveQuad.mAngle2 = delta2;
        }
        if (
            stage->mCurrWorld == WORLD_4 && stage->mCurrCourse == STAGE_TOWER && stage->mCurrFile != 1 ||
            stage->mCurrWorld == WORLD_4 && stage->mCurrCourse == STAGE_CASTLE && stage->mCurrFile != 1
        ) {
            scale1 = 0.5f;
            scale2 = 0.5f;
            ang1 = 14000;
            ang2 = 9000;
            ang3 = -1400;
            ang4 = 1500;
        }
        if (
            stage->mCurrWorld == WORLD_5 && stage->mCurrCourse == STAGE_1
        ) {
            scale1 = 0.5f;
            scale2 = 0.5f;
            ang1 = 20000;
            ang2 = 15000;
            ang3 = -1400;
            ang4 = 1500;
        }
        if (
            stage->mCurrWorld == WORLD_6 && stage->mCurrCourse == STAGE_3
        ) {
            scale1 = 0.8f;
            scale2 = 0.8f;
            ang1 = 14000;
            ang2 = 9000;
            ang3 = -1400;
            ang4 = 1500;
        }
    }
    if (m_d30a == 1) {
        scale1 = 40.0f;
        scale2 = -4.0f;
        ang1 = 1900;
        ang2 = 7000;
        ang3 = 120;
        ang4 = 900;
    } else if (m_d30a == 0 && (m_d308 == 1 || m_d308 == 2) && (dActor_c::mExecStop & 1) == 0) {
        waveDecoExecute();
    }
    m_d2ec++;
    int someVal = m_d2ec >> 2 & 7;
    if (m_d2e8 == 0) {
        t.x = dBg_c::m_bg_p->getLeft();
    } else {
        t.x = mPos.x;
    }
    t.y = mPos.y;
    t.z = mPos.z + 4.0f;
    mMtxWave.trans(t.x, t.y, t.z);
    if (m_d309 != 0) {
        float tmp = m_d284 * 8.0f;
        mMtx_c sc = mMtx_c::createScale((tmp + 4.0f) / tmp, 1.0f, 1.0f);
        mMtxWave.concat(sc);
    }
    mWaveQuad.setLocalMtx(&mMtxWave);
    m_d2f0 = (dBgParameter_c::getInstance()->xStart() - dBg_c::m_bg_p->getLeft()) / 8.0f;
    if ((dActor_c::mExecStop & 1) == 0) {
        m_d300 += ang3;
        m_d304 += ang4;
    }

    float someval = 0.001f;
    i = 0;
    float f9;
    float f11;
    float tmp;
    float f31;
    float f10;
    float z;
    float f8;
    for (; i < 20; i++) {
        z = 1850.0f;
        f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[3];
        if (dBg_c::m_bg_p->mWaveActive[i] == 0) {
            continue;
        }
        f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[0];
        f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[1];
        f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[2];
        f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[0];
        switch (mSub[i].m_a34) {
            case 0:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[2];
                break;
            case 1:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[5];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[6];
                break;
            case 2:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[8];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[9];
                break;
            case 3:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[11];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[12];
                break;
            case 10:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[15];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[16];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[3];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[4];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[5];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[13];
                break;
            case 11:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[19];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[20];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[6];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[7];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[8];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[17];
                break;
            case 4:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[23];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[24];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[21];
                break;
            case 5:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[27];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[28];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[25];
                break;
            case 6:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[31];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[32];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[29];
                break;
            case 7:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[35];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[36];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[33];
                break;
            case 8:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[39];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[40];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[37];
                break;
            case 9:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[43];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[44];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[41];
                break;
            case 13:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[47];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[48];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[45];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[49];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[50];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[51];
                break;
            case 14:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[54];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[55];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[52];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[60];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[61];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[62];
                break;
            case 15:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[58];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[59];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[56];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[60];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[61];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[62];
                break;
            case 16:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[65];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[66];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[63];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[71];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[72];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[73];
                break;
            case 17:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[69];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[70];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[67];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[71];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[72];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[73];
                break;
            case 18:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[76];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[77];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[74];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[82];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[83];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[84];
                break;
            case 19:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[80];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[81];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[78];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[82];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[83];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[84];
                break;
            case 20:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[87];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[88];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[85];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[89];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[90];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[91];
                break;
            case 21:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[94];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[95];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[92];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[100];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[101];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[102];
                break;
            case 22:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[98];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[99];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[96];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[100];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[101];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[102];
                break;
            case 23:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[105];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[106];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[103];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[111];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[112];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[113];
                break;
            case 24:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[109];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[110];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[107];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[111];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[112];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[113];
                break;
            case 25:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[116];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[117];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[114];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[122];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[123];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[124];
                break;
            case 26:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[120];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[121];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[118];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[122];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[123];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[124];
                break;
            case 27:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[127];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[128];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[125];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[129];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[130];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[131];
                break;
            case 28:
                z = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[134];
                f8 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[135];
                f11 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[132];
                f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[136];
                f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[137];
                f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[138];
                break;
        }

        if (mSub[i].m_a34 >= 4 && mSub[i].m_a34 < 10) {
            f9 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[9];
            f31 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[10];
            f10 = sGlobalData_c<dWaterWave_c>::m_member.mFloats[11];
        }
        if (m_d30a == 0 || (dActor_c::mExecStop & 1) == 0 || (dBg_c::m_bg_p->mWaveType[i] < 10 || dBg_c::m_bg_p->mWaveType[i] > 12)) {
            switch (mSub[i].m_968) {
                case 0:
                    if (dBg_c::m_bg_p->mWaveActive[i] != 0 && dBg_c::m_bg_p->mWaveActive[i] < 99) {
                        if (
                            (dBg_c::m_bg_p->mWaveType[i] >= 10 && dBg_c::m_bg_p->mWaveType[i] < 13) ||
                            (
                                dBg_c::m_bg_p->mWaveX[i] > t.x &&
                                dBg_c::m_bg_p->mWaveX[i] < t.x + m_d284 * 8.0f &&
                                t.y < dBg_c::m_bg_p->mWaveY[i] + 8.0f &&
                                t.y > dBg_c::m_bg_p->mWaveY[i] - 8.0f
                            )
                        ) {
                            mSub[i].m_964 = (dBg_c::m_bg_p->mWaveX[i] - t.x) / 8.0f;
                            mSub[i].m_a34 = dBg_c::m_bg_p->mWaveType[i];
                            for (int j = 0; j < 200; j++) {
                                mSub[i].m_96c[j] = 0;
                                mSub[i].m_00[j] = 0.0f;
                            }
                            dBg_c::m_bg_p->mWaveActive[i] = 99;
                            mSub[i].m_968++;
                        }
                    }
                    break;
                case 1: {
                    float *ptr = &mSub[i].m_640;
                    sLib::addCalc(ptr, 0.0f, f9, f31, f10);
                    if (*ptr < 0.001f) {
                        *ptr = 1.0f;
                        mSub[i].m_968++;
                    }
                    break;
                }
                case 2:
                    int count = 0;
                    for (int j = 0; j < ARRAY_SIZE(mSub[i].m_96c); j++) {
                        if (mSub[i].m_96c[j] == 2) {
                            count++;
                        }
                    }

                    if (count >= f11) {
                        mSub[i].m_968 = 0;
                        dBg_c::m_bg_p->mWaveActive[i] = 0;
                    }
                    break;
            }
            for (int j = 0; j < f11; j++) {
                float t = j / f11;
                float aaa = t * t * t;
                tmp = (1.0f - aaa) * ((j / f11) * 0.7f + 0.3f);

                switch (mSub[i].m_96c[j]) {
                    case 0:
                        if (j / f11 > 1.0f - mSub[i].m_00[j]) {
                            mSub[i].m_644[j] = 0;
                            mSub[i].m_320[j] = 1.0f;
                            mSub[i].m_96c[j]++;
                        }
                        break;
                    case 1:
                        if (mSub[i].m_a34 == 11 && mSub[i].m_00[j] < -0.5f) {
                            break;
                        }
                        mSub[i].m_00[j] = nw4r::math::SinIdx(mSub[i].m_644[j]);
                        mSub[i].m_644[j] += z;
                        mSub[i].m_00[j] *= tmp;
                        mSub[i].m_00[j] *= mSub[i].m_320[j];

                        if (mSub[i].m_a34 != 11) {
                            if (mSub[i].m_00[j] < 0.0f) {
                                mSub[i].m_00[j] *= 4.0f;
                            }
                        } else if (mSub[i].m_00[j] < 0.0f) {
                            mSub[i].m_00[j] *= 0.5f;
                        }

                        sLib::addCalc(&mSub[i].m_00[j], 0.0f, 0.1f, f8, 0.0001f);
                        if (mSub[i].m_00[j] < someval) {
                            mSub[i].m_00[j] = 0.0f;
                            mSub[i].m_644[j] = 0;
                            mSub[i].m_96c[j]++;
                        }
                        break;
                }
            }
        }
    }
    for (quadI = 0; quadI <= m_d2fc; quadI++) {
        waveQuadPos[0].set(quadI * 8.0f, 0.0f, 0.0f);
        waveQuadPos[1].set((quadI + 1) * 8.0f, 0.0f, 0.0f);
        waveQuadPos[2].set((quadI + 1) * 8.0f, -8.0f, 0.0f);
        waveQuadPos[3].set(quadI * 8.0f, -8.0f, 0.0f);

        float sinA = nw4r::math::SinIdx(m_d300 + aInc);
        waveQuadPos[0].y += (sinA + 1.0f) * scale1;

        float sinB = nw4r::math::SinIdx(m_d304 + bInc);
        waveQuadPos[0].y += (sinB + 1.0f) * scale2;

        aInc += ang1;
        bInc += ang2;

        for (waveIndex = 0; waveIndex < 20; waveIndex++) {
            if (dBg_c::m_bg_p->mWaveActive[waveIndex] == 0) {
                continue;
            }

            int a34 = mSub[waveIndex].m_a34;

            float something1, something2;
            something2 = 20.0f;
            something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[0];
            switch (a34) {
                case 0:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[1]; break;
                case 1:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[4]; break;
                case 2:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[7]; break;
                case 3:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[10]; break;
                case 4:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[14];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[13]; break;
                case 5:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[18];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[17]; break;
                case 6:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[22];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[21]; break;
                case 7:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[26];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[25]; break;
                case 8:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[30];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[29]; break;
                case 9:  something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[34];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[33]; break;
                case 10: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[38];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[37]; break;
                case 11: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[42];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[41]; break;
                case 13: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[46];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[45]; break;
                case 14: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[53];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[52]; break;
                case 15: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[57];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[56]; break;
                case 16: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[64];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[63]; break;
                case 17: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[68];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[67]; break;
                case 18: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[75];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[74]; break;
                case 19: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[79];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[78]; break;
                case 20: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[86];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[85]; break;
                case 21: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[93];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[92]; break;
                case 22: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[97];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[96]; break;
                case 23: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[104];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[103]; break;
                case 24: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[108];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[107]; break;
                case 25: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[115];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[114]; break;
                case 26: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[119];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[118]; break;
                case 27: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[126];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[125]; break;
                case 28: something2 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[133];
                         something1 = sGlobalData_c<dWaterWave_c>::m_member.mFloats2[132]; break;
                default: break;
            }

            int idk = mSub[waveIndex].m_968;

            if (idk == 0) {
                continue;
            }

            if (quadI >= mSub[waveIndex].m_964 && quadI < mSub[waveIndex].m_964 + something1) {
                waveQuadPos[0].y += something2 * mSub[waveIndex].m_00[quadI - idk];
            }

            if (quadI > mSub[waveIndex].m_964 - something1 && quadI < mSub[waveIndex].m_964) {
                waveQuadPos[0].y += something2 * mSub[waveIndex].m_00[quadI - idk];
            }
        }

        if (quadI < m_d2fc) {
            mWaveQuad.setOfs(waveQuadPos, quadI);
        }
        if (quadI != 0) {
            mWaveQuad.setOfs(waveQuadPos, 1, quadI - 1);
        }

        if (m_d30a == 1 || m_d30a == 2) {
            for (int p = 0; p < PLAYER_COUNT; p++) {
                dAcPy_c *player = daPyMng_c::getPlayer(p);
                if (player == nullptr) {
                    continue;
                }

                if (
                    t.x + waveQuadPos[0].x > player->mPos.x - 4.0f &&
                    t.x + waveQuadPos[0].x < player->mPos.x + 4.0f &&
                    player->mPos.y < t.y + waveQuadPos[0].y - 10.0f &&
                    !player->isDemoType(daPlBase_c::DEMO_TYPE_4)
                ) {
                    player->setDamage(nullptr, daPlBase_c::DAMAGE_7);
                }
            }

            if (t.x + waveQuadPos[0].x > bgParam->xStart() && t.x + waveQuadPos[0].x < bgParam->xStart() + bgParam->xSize()) {
                if (waveCounter < 80) {
                    dBg_c::m_bg_p->mWaveRelated[waveCounter2] = t.y + waveQuadPos[0].y - 10.0f;
                }
                waveCounter2++;
                waveCounter++;
            }
        }
    }
    m_d2f8 = someVal;
}

void dWaterWave_c::draw() {
    if (!m_d30a) {
        dBgParameter_c *bgParam = dBgParameter_c::getInstance();
        if (mPos.y < bgParam->yStart() - bgParam->ySize() - 16.0f) {
            return;
        }
    }
    if (m_d308 == 1 || m_d308 == 2) {
        mWaveDeco.entry();
    }
    mWaveQuad.entry();
}

void dWaterWave_c::doDelete() {
    mWaveQuad.remove();
    if (m_d308 == 1 || m_d308 == 2) {
        mWaveDeco.remove();
    }
}
