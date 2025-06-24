#include <game/bases/d_2d.hpp>
#include <_dummy_classes.hpp>

namespace d2d {
    
    void init() {}
    
    void draw() {
        m2d::draw();
    }
    
    void drawBefore() {
        m2d::drawBefore((unsigned char)0x81);
    }
    
    void drawAfter() {
        m2d::drawAfter((unsigned char)0x80);
    }
    
    void drawBtween(unsigned char a, unsigned char b) {
        m2d::drawBtween(a, b);
    }
    
    
    int setAlpha_patrolPane_patrol(nw4r::lyt::Pane* pane, void* obj) {
        struct unkstruct {
            u8 _00;
        };
        unkstruct* s = (unkstruct*)obj;
        
        pane->setAlpha(s->_00);
        pane->setField_b9(s->_00);
        
        return 0;
    }
    
} // namespace d2d
