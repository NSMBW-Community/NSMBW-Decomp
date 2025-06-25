#include <game/bases/d_2d.hpp>
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
    
    int setAlpha_patrolPane_patrol(nw4r::lyt::Pane* pane, void* obj) {
        u8* s = (u8*)obj;
        
        pane->setAlpha(s[0]);
        pane->setGlbAlpha(s[0]);
        
        return 0;
    }

    void Multi_c::entry() {
        m2d::Base_c::entry();
    }

    nw4r::lyt::Pane* Multi_c::getRootPane() {
        return mLayout.getRootPane();
    }
    
} // namespace d2d
