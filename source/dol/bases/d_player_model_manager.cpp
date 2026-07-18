#include <game/bases/d_player_model_manager.hpp>

dPyMdlBase_HIO_c dPyMdlMng_c::m_hio = dPyMdlBase_HIO_c();

dPyMdlMng_c::dPyMdlMng_c(dPyMdlMng_c::ModelType_e modelType)
    : mpMdl(nullptr), m_0c(0)
{
    construct(modelType);
}

void dPyMdlMng_c::construct(u8 modelType) {
    switch (modelType) {
        case TYPE_MARIO1:
        case TYPE_MARIO2:
            mpMdl = new(mHeap::g_gameHeaps[0], 4) dMarioMdl_c(modelType);
            break;
        case TYPE_TOAD1:
        case TYPE_TOAD2:
        case TYPE_TOAD3:
            mpMdl = new(mHeap::g_gameHeaps[0], 4) dKinopioMdl_c(modelType);
            break;
        case TYPE_YOSHI:
            mpMdl = new(mHeap::g_gameHeaps[0], 4) dYoshiMdl_c(modelType);
            break;
    }
}


void dPyMdlMng_c::create(u8 a, u8 b, dPyMdlMng_c::SceneType_e c) {
    mpMdl->create(a, b, c);
}

dPyMdlMng_c::~dPyMdlMng_c() {
    if (mpMdl) {
        delete mpMdl;
    }
}

void dPyMdlMng_c::play() {
    mpMdl->play();
}

void dPyMdlMng_c::calc(mMtx_c &mtx) {
    m_0c = 1;
    float scale = m_hio.m_08[mpMdl->mPlayerMode];

    mMtx_c x;
    PSMTXScale(x, scale, scale, scale);
    PSMTXConcat(mtx, x, mtx);

    mpMdl->calc(mtx);
}

void dPyMdlMng_c::calc(mVec3_c pos, mAng3_c rot, mVec3_c scale) {
    m_0c = 1;
    scale *= m_hio.m_08[mpMdl->mPlayerMode];
    mpMdl->calc(pos, rot, scale);
}

void dPyMdlMng_c::calc2() {
    mpMdl->calc2();
}

void dPyMdlMng_c::draw() {
    if (m_0c) {
        mpMdl->draw();
    }
}
