#include "egg/gfxe/eggFrustum.h"
#include "egg/gfxe/eggScreen.h"
#include "game/mLib/m_2d/animation.hpp"
#include "game/mLib/m_2d/simple.hpp"
#include "game/mLib/m_mtx.hpp"
#include "nw4r/lyt/lyt_group.h"
#include "nw4r/lyt/lyt_pane.h"
#include "revolution/MTX/mtx.h"
#include "revolution/SC/scapi.h"
#include <game/mLib/m_2d.hpp>
#include <lib/egg/core/eggExpHeap.h>
#include <nw4r/ut.h>
#include <revolution/GX.h>
#include <constants/sjis_constants.h>

nw4r::ut::List m2d::l_list;
mAllocator_c *m2d::l_allocator;

void m2d::reset() {
    nw4r::ut::List_Init(&m2d::l_list, 0);
}

bool m2d::create(EGG::Heap *parentHeap, size_t size) {
    nw4r::lyt::LytInit();
    EGG::ExpHeap *heap = EGG::ExpHeap::create(size, parentHeap, 4);
    heap->mpName = M2D_HEAP_NAME;
    l_allocator = new(heap, 4) mAllocator_c();
    l_allocator->attach(heap, 4);
    nw4r::lyt::Layout::SetAllocator(l_allocator);
    m2d::reset();
    return true;
}

void m2d::defaultSet() {
    GXSetCullMode(GX_CULL_NONE);
    GXSetZMode(0, GX_ALWAYS, 0);
    static const GXColor l_clearColor = { 0x00000000 };
    GXSetFog(GX_FOG_NONE, l_clearColor, 0.0f, 0.0f, 0.0f, 0.0f);
}

void m2d::draw() {
    defaultSet();

    m2d::Base_c *next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, nullptr);
    while (next != nullptr) {
        next->draw();
        next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, next);
    }
    m2d::reset();
}

void m2d::drawBefore(u8 before) {
    m2d::Base_c *next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, nullptr);
    while (next != nullptr) {
        if (next->mDrawOrder >= before) {
            break;
        }
        next->draw();
        next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, next);
    }
}

void m2d::drawAfter(u8 after) {
    m2d::Base_c *next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, nullptr);
    while (next != nullptr) {
        if (next->mDrawOrder > after) {
            next->draw();
        }
        next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, next);
    }
}

void m2d::drawBtween(u8 after, u8 before) {
    m2d::Base_c *next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, nullptr);
    while (next != nullptr) {
        if (next->mDrawOrder > before) {
            break;
        }
        if (next->mDrawOrder >= after) {
            next->draw();
        }
        next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, next);
    }
}

mAllocator_c *m2d::getAllocator() {
    return l_allocator;
}

// [There are a few float constants that must appear here.
// Looking at ss, this might be m2d::Anm_c, which doesn't appear in NSMBW.]
DECL_WEAK
void DUMMY_ORDERING() {
    float dummy_float = 1.0f;
    float dummy_float2 = -1.0f;
    u16 tmp = 1234;
    float dummy_float3 = (float) tmp;
}

nw4r::lyt::AnimTransform *m2d::Layout_c::CreateAnimTransform(const void *binary, nw4r::lyt::ResourceAccessor *accessor) {
    nw4r::lyt::AnimTransform *transform = nw4r::lyt::Layout::CreateAnimTransform(binary, accessor);
    if (transform != nullptr) {
        mAnimTransList.Erase(transform);
    }
    return transform;
}

bool m2d::Layout_c::bindAnimationAuto(const nw4r::lyt::AnimResource &res, nw4r::lyt::ResourceAccessor *resAccessor) {
    u16 groupNum = res.GetGroupNum();
    u16 shareInfoNum = res.GetAnimationShareInfoNum();
    if (groupNum == 0 || shareInfoNum != 0) {
        return BindAnimationAuto(res, resAccessor);
    } else if (GetRootPane() == nullptr) {
        return false;
    } else if (res.GetResourceBlock() == nullptr) {
        return false;
    } else {
        const nw4r::lyt::AnimationGroupRef *arr = res.GetGroupArray();
        for (int i = 0; i < groupNum; i++) {
            nw4r::lyt::Group *group = GetGroupContainer()->FindGroupByName(arr[i].GetName());
            u16 animNum = res.CalcAnimationNum(group, res.IsDescendingBind());
            nw4r::lyt::AnimTransform *animTransform = nw4r::lyt::Layout::CreateAnimTransform();
            animTransform->SetResource(res.GetResourceBlock(), resAccessor, animNum);
            nw4r::lyt::BindAnimation(group, animTransform, res.IsDescendingBind(), true);
        }
        return true;
    }
}

bool m2d::ResAccIf_c::attach(void *resource, const char *rootDir) {
    if (mpResource != nullptr) {
        return false;
    }
    mpResource = resource;
    return mpResAccessor->Attach(mpResource, rootDir);
}

void m2d::ResAccIf_c::detach() {
    mpResource = nullptr;
    mpResAccessor->Detach();
}

void *m2d::ResAccIf_c::getResource(ulong type, const char *name) {
    return mpResAccessor->GetResource(type, name, nullptr);
}

void m2d::FrameCtrl_c::play() {
    mPrevFrame = mCurrFrame;
    float frame = mCurrFrame;
    if (mFlags & 2) {
        if (frame >= mRate) {
            frame -= mRate;
        } else if (mFlags & 1) {
            frame = 0;
        } else {
            frame += mEndFrame - mRate;
        }
    } else {
        frame += mRate;
        if (mFlags & 1) {
            if (frame > getLastActiveFrame()) {
                frame = getLastActiveFrame();
            }
        } else if (frame >= mEndFrame) {
            frame -= mEndFrame;
        }
    }
    mCurrFrame = frame;
}

void m2d::FrameCtrl_c::set(float endFrame, u8 flags, float rate, float currFrame) {
    if (currFrame < 0.0f) {
        currFrame = 0.0f;
    }
    mEndFrame = endFrame;
    mCurrFrame = currFrame;
    setRate(rate);
    mFlags = flags;
    mPrevFrame = mCurrFrame;
}

void m2d::FrameCtrl_c::setFrame(float frame) {
    mCurrFrame = frame;
    mPrevFrame = frame;
}

void m2d::FrameCtrl_c::setRate(float rate) {
    mRate = rate;
}

bool m2d::FrameCtrl_c::isStop() const {
    switch (mFlags) {
        case 1:
            return mCurrFrame >= getLastActiveFrame();
        case 3:
            return mCurrFrame <= 0.0f;
        default:
            return false;
    }
}

void m2d::Base_c::entry() {
    m2d::Base_c *next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, nullptr);
    while (next != nullptr) {
        if (next->mDrawOrder > mDrawOrder) {
            nw4r::ut::List_Insert(&m2d::l_list, next, this);
            return;
        }
        next = (m2d::Base_c *) nw4r::ut::List_GetNext(&m2d::l_list, next);
    }
    nw4r::ut::List_Append(&m2d::l_list, this);
}

m2d::Simple_c::Simple_c() {
    mpResAcc = nullptr;
    mPos.set(0.0f, 0.0f, 0.0f);
    mFlags = 0;
    if (SCGetAspectRatio() == SC_ASPECT_WIDE) {
        mDrawInfo.SetLocationAdjustScale(nw4r::math::VEC2(19.0f / 26.0f, 1.0f));
        mDrawInfo.SetLocationAdjust(true);
    }
}

void m2d::Simple_c::calc() {
    calcBefore();
    mMtx_c mtx;
    PSMTXIdentity(mtx);
    mVec3_c pos = mPos;
    PSMTXTransApply(mtx, mtx, pos.x, pos.y, pos.z);
    mDrawInfo.SetViewMtx(mtx);
    calcAfter();
}

void m2d::Simple_c::calcBefore() {
    u32 option = 0;
    if (mFlags & 1) {
        option = 1;
    }
    mLayout.Animate(option);
}

void m2d::Simple_c::calcAfter() {
    mDrawInfo.SetViewRect(mLayout.GetLayoutRect());
    mLayout.CalculateMtx(mDrawInfo);
}

void m2d::Simple_c::draw() {
    nw4r::ut::Rect rect = mLayout.GetLayoutRect();
    float near = 0.0f;
    float far = 500.0f;
    EGG::Screen screen;
    bool isWide = EGG::Screen::sTVMode == EGG::Screen::TV_MODE_16_9;
    float w_16_9 = EGG::Screen::sTVModeInfo[EGG::Screen::TV_MODE_16_9].width;
    float w_4_3 = EGG::Screen::sTVModeInfo[EGG::Screen::TV_MODE_4_3].width;
    float left = isWide ? w_16_9 * rect.left / w_4_3 : rect.left;
    float right = isWide ? w_16_9 * rect.right / w_4_3 : rect.right;
    screen.mProjType = EGG::Frustum::PROJ_ORTHO;
    screen.ResetOrthographic(rect.top, rect.bottom, left, right, near, far);
    if (isWide) {
        screen.mScale = nw4r::math::VEC3(w_4_3 / w_16_9, 1.0f, 1.0f);
    }
    screen.SetProjectionGX();
    mLayout.Draw(mDrawInfo);
}

bool m2d::Simple_c::build(const char *lytName, m2d::ResAccIf_c *resAcc) {
    if (mLayout.GetRootPane() != nullptr) {
        return true;
    }
    if (resAcc == nullptr) {
        resAcc = mpResAcc;
        if (resAcc == nullptr) {
            return false;
        }
    }
    void *res = resAcc->getResource(0, lytName);
    if (res == nullptr) {
        return false;
    }
    bool result = mLayout.Build(res, resAcc->getResAccessor());
    if (result) {
        calc();
    }
    return result;
}

int m2d::Simple_c::patrolPane_local(nw4r::lyt::Pane *pane, patrolPaneFunc1 func1, patrolPaneFunc2 func2, void *arg) {
    int ret = 0;
    if (func2 == nullptr || func2(pane) == 1) {
        if (ret = func1(pane, arg), ret != 0) {
            return ret;
        }
    }
    nw4r::lyt::PaneList &list = pane->GetChildList();
    for (
        nw4r::lyt::PaneList::RevIterator it = list.GetEndReverseIter();
        it != list.GetBeginReverseIter();
        ++it
    ) {
        ret = patrolPane_local(&*it, func1, func2, arg);
        if (ret != 0) {
            return ret;
        }
    }
    return ret;
}

bool m2d::Simple_c::patrolPane(patrolPaneFunc1 func1, patrolPaneFunc2 func2, void *arg) {
    nw4r::lyt::Pane *pane = mLayout.GetRootPane();
    int ret = patrolPane_local(pane, func1, func2, arg);
    return ret != 2;
}

bool m2d::AnmResV2_c::create(const char *name, m2d::ResAccIf_c *resAcc, m2d::Layout_c *layout, bool useOverride) {
    mAnimResource.Set(resAcc->getResource(0, name));
    if (useOverride) {
        layout->bindAnimationAuto(mAnimResource, resAcc->getResAccessor());
    } else {
        layout->BindAnimationAuto(mAnimResource, resAcc->getResAccessor());
    }
    mGroupNum = mAnimResource.GetGroupNum();
    mGroupAnim = new(getAllocator()->mpHeap, 4) GroupAnimTransform_c[mGroupNum];
    const nw4r::lyt::AnimationGroupRef *groupArray = mAnimResource.GetGroupArray();
    for (int i = 0; i < mGroupNum; i++) {
        nw4r::lyt::AnimTransform *transform = nullptr;
        nw4r::lyt::Group *group = layout->GetGroupContainer()->FindGroupByName(groupArray->GetName());
        mGroupAnim[i].mpGroup = group;
        nw4r::lyt::detail::PaneLinkList &list = group->GetPaneList();
        for (
            nw4r::lyt::detail::PaneLinkList::Iterator it = list.GetBeginIter();
            it != list.GetEndIter();
            ++it
        ) {
            nw4r::lyt::AnimationLink *link = findAnmLink(it->mTarget, false);
            if (link != nullptr) {
                transform = link->GetAnimTransform();
                break;
            }
        }
        mGroupAnim[i].mpAnimTransform = transform;
        groupArray++;
    }
    return true;
}

bool m2d::AnmResV2_c::remove() {
    delete[] mGroupAnim;
    mGroupAnim = nullptr;
    return true;
}

m2d::GroupAnimTransform_c *m2d::AnmResV2_c::getGroupAnimTransform(const char *name) {
    GroupAnimTransform_c *res = nullptr;
    for (int i = 0; i < mGroupNum; i++) {
        if (strcmp(name, mGroupAnim[i].mpGroup->GetName()) == 0) {
            res = &mGroupAnim[i];
            break;
        }
    }
    return res;
}

void m2d::AnmResV2_c::setAnmEnable(nw4r::lyt::Group *group, bool enable) {
    for (
        nw4r::lyt::detail::PaneLinkList::Iterator it = group->GetPaneList().GetBeginIter();
        it != group->GetPaneList().GetEndIter();
        ++it
    ) {
        nw4r::lyt::AnimationLink *link = findAnmLink(it->mTarget, false);
        if (link != nullptr) {
            it->mTarget->SetAnimationEnable(link->GetAnimTransform(), enable, false);
        }
    }
}

void m2d::AnmResV2_c::updateFrame(nw4r::lyt::Group *group, float frame) {
    for (
        nw4r::lyt::detail::PaneLinkList::Iterator it = group->GetPaneList().GetBeginIter();
        it != group->GetPaneList().GetEndIter();
        ++it
    ) {
        nw4r::lyt::Pane *pane = it->mTarget;
        nw4r::lyt::AnimationLink *link = findAnmLink(pane, false);
        if (link != nullptr) {
            setAnmFrame(pane, frame);
        }
    }
}

nw4r::lyt::AnimationLink *m2d::AnmResV2_c::findAnmLink(nw4r::lyt::Pane *pane, bool recursive) {
    nw4r::lyt::AnimationLink *res = pane->FindAnimationLinkSelf(mAnimResource);
    if (res != nullptr) {
        return res;
    }
    u8 matNum = pane->GetMaterialNum();
    for (u8 i = 0; i < matNum; i++) {
        res = pane->GetMaterial(i)->FindAnimationLink(mAnimResource);
        if (res != nullptr) {
            return res;
        }
    }
    if (recursive) {
        for (
            nw4r::lyt::PaneList::Iterator it = pane->GetChildList().GetBeginIter();
            it != pane->GetChildList().GetEndIter();
            ++it
        ) {
            res = findAnmLink(&*it, true);
            if (res != nullptr) {
                return res;
            }
        }
    }
    return nullptr;
}

void m2d::AnmResV2_c::setAnmFrame(nw4r::lyt::Pane *pane, float frame) {
    nw4r::lyt::AnimationLink *res = pane->FindAnimationLinkSelf(mAnimResource);
    if (res != nullptr) {
        res->GetAnimTransform()->SetFrame(frame);
    }
    u8 matNum = pane->GetMaterialNum();
    for (u8 i = 0; i < matNum; i++) {
        res = pane->GetMaterial(i)->FindAnimationLink(mAnimResource);
        if (res != nullptr) {
            res->GetAnimTransform()->SetFrame(frame);
        }
    }
}

bool m2d::AnmGroupBase_c::create(AnmResV2_c *anmRes, const char *name) {
    mpAnmRes = anmRes;
    mpGroupAnim = mpAnmRes->getGroupAnimTransform(name);
    if (mpGroupAnim != nullptr) {
        bool noLoop = true;
        if (mpGroupAnim->mpAnimTransform->IsLoopData()) {
            noLoop = false;
        }
        mpFrameCtrl->set(mpGroupAnim->mpAnimTransform->GetFrameSize(), noLoop, 1.0f, -1.0f);
        updateFrame();
    }
    return true;
}

void m2d::AnmGroupBase_c::setAnmEnable(bool enable) {
    mpAnmRes->setAnmEnable(mpGroupAnim->mpGroup, enable);
    if (enable) {
        mFlags |= 1;
    } else {
        mFlags &= ~1;
    }
}

void m2d::AnmGroupBase_c::updateFrame() {
    mpAnmRes->updateFrame(mpGroupAnim->mpGroup, mpFrameCtrl->getFrame());
}
