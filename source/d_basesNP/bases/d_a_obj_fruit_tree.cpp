#include <game/bases/d_a_obj_fruit_tree.hpp>
#include <game/mLib/m_allocator_dummy_heap.hpp>
#include <game/bases/d_res_mng.hpp>
#include <game/bases/d_s_stage.hpp>

ACTOR_PROFILE(OBJ_FRUITTREE, daObjFruitTree_c, 0);

int daObjFruitTree_c::create() {

    u32 param = mParam;

    nw4r::g3d::ResMdl model;
    nw4r::g3d::ResAnmChr anim;
    nw4r::g3d::ResFile res;

    mHeapAllocator.createFrmHeap(-1, mHeap::g_gameHeaps[0], nullptr, 0x20);

    int style = ACTOR_PARAM_LOCAL(param, Style);

    if (style == STYLE_REGULAR) {
        res = dResMng_c::m_instance->getRes("fruits_kusa", "g3d/fruits_kusa.brres");
    } else {
        res = dResMng_c::m_instance->getRes("fruits_kusa_gake", "g3d/fruits_kusa_gake.brres");
    }

    mResFile = res;

    if (style == STYLE_REGULAR) {
        switch (ACTOR_PARAM_LOCAL(param, Size)) {
            case SIZE_SMALL:
                model = mResFile.GetResMdl("fruits_kusa_S");
                break;

            case SIZE_MEDIUM:
                model = mResFile.GetResMdl("fruits_kusa_M");
                break;

            case SIZE_LARGE:
                model = mResFile.GetResMdl("fruits_kusa_L");
                break;

            case SIZE_EXTRA_LARGE:
                model = mResFile.GetResMdl("fruits_kusa_LL");
                break;
        }
    } else {
        switch (ACTOR_PARAM_LOCAL(param, Size)) {
            case SIZE_SMALL:
                model = mResFile.GetResMdl("fruits_kusa_gake_S");
                break;

            case SIZE_MEDIUM:
                model = mResFile.GetResMdl("fruits_kusa_gake_M");
                break;

            case SIZE_LARGE:
                model = mResFile.GetResMdl("fruits_kusa_gake_L");
                break;

            case SIZE_EXTRA_LARGE:
                model = mResFile.GetResMdl("fruits_kusa_gake_LL");
                break;
        }
    }

    // [Bug: the actor's bounding box is not updated based on the bush size]
    mBushModel.create(model, &mHeapAllocator, 0x20, 1, nullptr);
    dActor_c::setSoftLight_MapObj(mBushModel);

    if (style == STYLE_REGULAR) {
        switch (ACTOR_PARAM_LOCAL(param, Size)) {
            case SIZE_SMALL:
                anim = mResFile.GetResAnmChr("fruits_kusa_S");
                break;

            case SIZE_MEDIUM:
                anim = mResFile.GetResAnmChr("fruits_kusa_M");
                break;

            case SIZE_LARGE:
                anim = mResFile.GetResAnmChr("fruits_kusa_L");
                break;

            case SIZE_EXTRA_LARGE:
                anim = mResFile.GetResAnmChr("fruits_kusa_LL");
                break;
        }
    } else {
        switch (ACTOR_PARAM_LOCAL(param, Size)) {
            case SIZE_SMALL:
                anim = mResFile.GetResAnmChr("fruits_kusa_gake_S");
                break;

            case SIZE_MEDIUM:
                anim = mResFile.GetResAnmChr("fruits_kusa_gake_M");
                break;

            case SIZE_LARGE:
                anim = mResFile.GetResAnmChr("fruits_kusa_gake_L");
                break;

            case SIZE_EXTRA_LARGE:
                anim = mResFile.GetResAnmChr("fruits_kusa_gake_LL");
                break;
        }
    }

    mBushAnim.create(model, anim, &mHeapAllocator, nullptr);
    mBushAnim.setAnm(mBushModel, anim, m3d::FORWARD_LOOP);
    mBushModel.setAnm(mBushAnim, 1.0f);
    mBushAnim.setFrame(dScStage_c::m_exeFrame % (u32)mBushAnim.mFrameMax);
    mHeapAllocator.adjustFrmHeap();
    execute();

    return SUCCEEDED;
}

int daObjFruitTree_c::execute() {
    mBushModel.play();
    return SUCCEEDED;
}

int daObjFruitTree_c::draw() {
    mVec3_c pos = mPos;
    mAng3_c rot = mAngle;
    changePosAngle(&pos, &rot, 1);

    if (mLayer == 0) {
        pos.z = -200.0f;
    } else {
        pos.z = -3700.0f;
    }

    mMatrix.trans(pos.x, pos.y, pos.z);
    mMatrix.YrotM(rot.y);
    mMatrix.XrotM(rot.x);
    mMatrix.ZrotM(rot.z);

    mBushModel.setLocalMtx(&mMatrix);
    mBushModel.setScale(mScale);
    mBushModel.calc(false);
    mBushModel.entry();

    return SUCCEEDED;
}

int daObjFruitTree_c::doDelete() {
    if (mHeapAllocator.mpHeap != mAllocatorDummyHeap_c::getInstance()) {
        mBushModel.remove();
    }

    return SUCCEEDED;
}