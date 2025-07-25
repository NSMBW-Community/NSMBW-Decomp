#include <game/bases/d_2d.hpp>
#include <lib/nw4r/lyt/lyt_pane.hpp>
#include <lib/nw4r/lyt/lyt_window.hpp>
#include <lib/nw4r/ut/RuntimeTypeInfo.hpp>
#include <_dummy_classes.hpp>

namespace d2d {
    
    void init() {}
    
    void draw() {
        m2d::draw();
    }
    
    void drawBefore() {
        m2d::drawBefore(0x81);
    }
    
    void drawAfter() {
        m2d::drawAfter(0x80);
    }
    
    void drawBtween(unsigned char a, unsigned char b) {
        m2d::drawBtween(a, b);
    }
    
    int setAlpha_patrolPane_patrol(nw4r::lyt::Pane *pane, void *obj) {
        u8 *s = (u8 *)obj;
        
        pane->setAlpha(s[0]);
        pane->setGlbAlpha(s[0]);
        
        return 0;
    }
    
    /*void* ResAccMult_c::getResource(unsigned long a, const char* b) {
        // mInternalAccessor->GetResource(a, b, 0);
    }*/

    void Multi_c::entry() {
        m2d::Base_c::entry();
    }

    void Multi_c::calcBefore() {
        bool flag = false;
        if (mFlags & 1) {
            flag = true;
        }
        mLayout.Animate(flag);
    }

    void Multi_c::calcAfter() {
        nw4r::ut::Rect rect = mLayout.GetLayoutRect();
    }

    nw4r::lyt::Pane *Multi_c::getRootPane() {
        return mLayout.getRootPane();
    }

    nw4r::lyt::Pane *Multi_c::findPaneByName(const char *name) {
        return mLayout.getRootPane()->FindPaneByName(name, true);
    }

    nw4r::lyt::Window *Multi_c::findWindowByName(const char* name) {
        bool found;
        nw4r::lyt::Window *window;
        nw4r::ut::detail::RuntimeTypeInfo *rtti;
        const nw4r::ut::detail::RuntimeTypeInfo *typeInfo;

        window = (nw4r::lyt::Window *)findPaneByName(name);
        if (window == 0) {
            goto bbb;
        }
        typeInfo = &window->typeInfo;

        if (window != 0) {
            rtti = (nw4r::ut::detail::RuntimeTypeInfo *)window->GetRuntimeTypeInfo();
            for (; rtti != 0; rtti = (nw4r::ut::detail::RuntimeTypeInfo *)rtti->mParentTypeInfo) {
                if (rtti == typeInfo) {
                    found = true;
                    goto aaa;
                }
            }
            found = false;

        aaa:
            if (found) {
                return window;
            }
        }
    bbb:
        return 0;
    }
    
} // namespace d2d
