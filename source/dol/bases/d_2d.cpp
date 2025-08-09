#include <game/bases/d_2d.hpp>
#include <game/bases/d_game_com.hpp>
#include <game/bases/d_font_manager.hpp>
#include <game/mLib/m_allocator.hpp>
#include <game/mLib/m_heap.hpp>
#include <game/mLib/m_mtx.hpp>
#include <constants/sjis_constants.h>

mAllocator_c *d2d::ResAccMultLoader_c::ms_res_allocator;

void d2d::init() {}

void d2d::draw() {
    m2d::draw();
}

void d2d::drawBefore() {
    m2d::drawBefore(129);
}

void d2d::drawAfter() {
    m2d::drawAfter(128);
}

void d2d::drawBtween(u8 after, u8 before) {
    m2d::drawBtween(after, before);
}

DECL_WEAK
void DUMMY_ORDERING() {
    nw4r::lyt::ArcResourceAccessor r;
}

int d2d::setAlpha_patrolPane_patrol(nw4r::lyt::Pane *pane, void *alpha) {
    u8 *pAlpha = (u8 *) alpha;
    pane->SetAlpha(*pAlpha);
    pane->SetGlbAlpha(*pAlpha);
    return 0;
}

void d2d::setAlpha(m2d::Simple_c *layout, u8 alpha) {
    layout->patrolPane(setAlpha_patrolPane_patrol, nullptr, &alpha);
}

d2d::ResAccMult_c::ResAccMult_c() {
    creater();
}

d2d::ResAccMult_c::~ResAccMult_c() {
}

void *d2d::ResAccMult_c::getResource(unsigned long type, const char *name) {
    return mInternalAccessor.GetResource(type, name, nullptr);
}

void d2d::ResAccMult_c::creater() {
    setResAccessor(&mInternalAccessor);
}

void *d2d::ResAccMult_c::InternalAccessor::GetResource(ulong type, const char *name, size_t *size) {
    if (type == RES_TYPE_FONT) {
        return dFontMng_c::getResFontData(name);
    }
    return nw4r::lyt::ArcResourceAccessor::GetResource(type, name, size);
}

nw4r::ut::Font *d2d::ResAccMult_c::InternalAccessor::GetFont(const char *name) {
    return dFontMng_c::getFont(dFontMng_c::getResFontIndex(name));
}

d2d::ResAccMultLoader_c::ResAccMultLoader_c() : mpResource(nullptr) {}
d2d::ResAccMultLoader_c::~ResAccMultLoader_c() {}

bool d2d::ResAccMultLoader_c::requestEx(const char *name, int i) {
    if (mpResource != nullptr) {
        return true;
    }
    mpResource = mLoader.request(name, i, ms_res_allocator->mpHeap);
    if (mpResource != nullptr) {
        attach(mpResource, "arc");
        return true;
    } else {
        return false;
    }
}

bool d2d::ResAccMultLoader_c::request(const char *name) {
    return requestEx(name, 0);
}

void d2d::ResAccMultLoader_c::remove() {
    if (mpResource != nullptr) {
        detach();
        ms_res_allocator->free(mpResource);
        mpResource = nullptr;
    }
    mLoader.remove();
}

bool d2d::ResAccMultLoader_c::create(EGG::Heap *parent, ulong size) {
    EGG::Heap *heap = mHeap::createExpHeap(size, parent, D2D_HEAP_NAME, 0x20, mHeap::OPT_4);
    ms_res_allocator = new(heap, 4) mAllocator_c();
    ms_res_allocator->attach(heap, 0x20);
    return true;
}

d2d::Multi_c::Multi_c() {
    mpResAccessor = nullptr;
    mPos.x = 0.0f;
    mPos.y = 0.0f;
    mFlags = 0;
    if (dGameCom::GetAspectRatio() == EGG::Screen::TV_MODE_16_9) {
        mDrawInfo.SetLocationAdjustScale(mVec2_c(19.0f / 26.0f, 1.0f));
        mDrawInfo.SetLocationAdjust(true);
    }
    mClipSettings.mEnabled = false;
}

d2d::Multi_c::~Multi_c() {}

void d2d::Multi_c::entry() {
    m2d::Base_c::entry();
}

void d2d::Multi_c::calc() {
    calcBefore();
    mMtx_c mtx;
    PSMTXIdentity(mtx);
    mVec3_c pos(mPos.x, mPos.y, 0.0f);
    PSMTXTransApply(mtx, mtx, pos.x, pos.y, pos.z);
    mDrawInfo.SetViewMtx(mtx);
    calcAfter();
}

void d2d::Multi_c::calcBefore() {
    ulong option = 0;
    if (mFlags & 1) {
        option = 1;
    }
    mLayout.Animate(option);
}

void d2d::Multi_c::calcAfter() {
    mDrawInfo.SetViewRect(mLayout.GetLayoutRect());
    mLayout.CalculateMtx(mDrawInfo);
}

void d2d::Multi_c::draw() {
    nw4r::ut::Rect rect = mLayout.GetLayoutRect();
    float near = 0.0f;
    float far = 500.0f;
    EGG::Screen screen;
    bool isWide = dGameCom::GetAspectRatio() == EGG::Screen::TV_MODE_16_9;
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
    if (mClipSettings.mEnabled) {
        u32 x, y, w, h;
        GXGetScissor(&x, &y, &w, &h);
        GXSetScissor(
            mClipSettings.mPos.x, mClipSettings.mPos.y,
            mClipSettings.mSize.x, mClipSettings.mSize.y
        );
        mLayout.Draw(mDrawInfo);
        GXSetScissor(x, y, w, h);
    } else {
        mLayout.Draw(mDrawInfo);
    }
}

nw4r::lyt::Pane *d2d::Multi_c::getRootPane() {
    return mLayout.GetRootPane();
}

bool d2d::Multi_c::build(const char *name, ResAccMult_c *resAcc) {
    if (mLayout.GetRootPane() != nullptr) {
        return true;
    }
    if (resAcc == nullptr) {
        resAcc = mpResAccessor;
        if (resAcc == nullptr) {
            return false;
        }
    }
    void *lyt = resAcc->getResource(0, name);
    if (lyt == nullptr) {
        return false;
    }
    bool res = mLayout.Build(lyt, &resAcc->mInternalAccessor);
    if (res) {
        calc();
    }
    return res;
}

nw4r::lyt::Pane *d2d::Multi_c::findPaneByName(const char *name) {
    return mLayout.GetRootPane()->FindPaneByName(name, true);
}

nw4r::lyt::TextBox *d2d::Multi_c::findTextBoxByName(const char *name) {
    nw4r::lyt::Pane *pane = findPaneByName(name);
    if (pane != nullptr) {
        nw4r::lyt::TextBox *box = nw4r::ut::DynamicCast<nw4r::lyt::TextBox *>(pane);
        return box;
    }
    return nullptr;
}

nw4r::lyt::Picture *d2d::Multi_c::findPictureByName(const char *name) {
    nw4r::lyt::Pane *pane = findPaneByName(name);
    if (pane != nullptr) {
        nw4r::lyt::Picture *box = nw4r::ut::DynamicCast<nw4r::lyt::Picture *>(pane);
        return box;
    }
    return nullptr;
}

nw4r::lyt::Window *d2d::Multi_c::findWindowByName(const char *name) {
    nw4r::lyt::Pane *pane = findPaneByName(name);
    if (pane != nullptr) {
        nw4r::lyt::Window *box = nw4r::ut::DynamicCast<nw4r::lyt::Window *>(pane);
        return box;
    }
    return nullptr;
}
