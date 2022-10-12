#pragma once
#include <lib/nw4r/lyt/lyt_draw_info.hpp>
#include <lib/nw4r/lyt/lyt_pane.hpp>
#include <_dummy_classes.hpp>

namespace nw4r {
namespace lyt {

class TextBox : public Pane {
public:

    TextBox(const res::TextBox *, const ResBlockSet &);

    virtual int *GetRuntimeTypeInfo() const { return &typeInfo; }
    virtual void DrawSelf(const DrawInfo &);
    virtual ut::Color GetVtxColor(unsigned long) const;
    virtual ut::Color SetVtxColor(unsigned long, ut::Color);
    virtual unsigned char GetVtxColorElement(unsigned long) const;
    virtual void SetVtxColorElement(unsigned long, unsigned char);
    virtual void LoadMtx(const DrawInfo &);

    virtual void AllocStringBuffer(unsigned short);
    virtual void FreeStringBuffer();
    virtual unsigned short SetString(const wchar_t *, unsigned short);
    virtual unsigned short SetString(const wchar_t *, unsigned short, unsigned short);

    ut::Font *GetFont() const;
    void SetFont(const ut::Font *);
    
private:
    static int typeInfo;
};

} // namespace lyt
} // namespace nw4r