#pragma once
#include <lib/nw4r/lyt/lyt_pane.hpp>
#include <_dummy_classes.hpp>

namespace nw4r {
namespace lyt {

class Picture : public Pane {
public:

    Picture(const res::Picture *, const ResBlockSet &);

    NW4R_UT_RUNTIME_TYPEINFO;
    
    virtual void DrawSelf(const DrawInfo &);
    virtual ut::Color GetVtxColor(unsigned long) const;
    virtual ut::Color SetVtxColor(unsigned long, ut::Color);
    virtual unsigned char GetVtxColorElement(unsigned long) const;
    virtual void SetVtxColorElement(unsigned long, unsigned char);

    virtual void Append(const TexMap &);
};

} // namespace lyt
} // namespace nw4r
