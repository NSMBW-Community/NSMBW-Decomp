#ifndef NW4R_UT_ROM_FONT_H
#define NW4R_UT_ROM_FONT_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_Font.h>

#include <revolution/OS.h>

namespace nw4r {
namespace ut {

class RomFont : public Font {
public:
    RomFont();
    virtual ~RomFont(); // at 0x8

    virtual int GetWidth() const;  // at 0xC
    virtual int GetHeight() const; // at 0x10

    virtual int GetAscent() const;      // at 0x14
    virtual int GetDescent() const;     // at 0x18
    virtual int GetBaselinePos() const; // at 0x1C

    virtual int GetCellHeight() const;   // at 0x20
    virtual int GetCellWidth() const;    // at 0x24
    virtual int GetMaxCharWidth() const; // at 0x28

    virtual Type GetType() const;              // at 0x2C
    virtual GXTexFmt GetTextureFormat() const; // at 0x30
    virtual int GetLineFeed() const;           // at 0x34

    virtual CharWidths GetDefaultCharWidths() const;              // at 0x38
    virtual void SetDefaultCharWidths(const CharWidths& rWidths); // at 0x3C

    virtual bool SetAlternateChar(u16 ch); // at 0x40
    virtual void SetLineFeed(int lf);      // at 0x44

    virtual int GetCharWidth(u16 ch) const;             // at 0x48
    virtual CharWidths GetCharWidths(u16 ch) const;     // at 0x4C
    virtual void GetGlyph(Glyph* pGlyph, u16 ch) const; // at 0x50
    virtual FontEncoding GetEncoding() const;           // at 0x54

    u32 GetRequireBufferSize();
    bool Load(void* pBuffer);

private:
    static const int CHAR_PTR_BUFFER_SIZE = 4;

private:
    void MakeCharPtr(char* pBuffer, u16 ch) const;
    u16 HandleUndefinedChar(u16 ch) const;

private:
    OSFontHeader* mFontHeader; // at 0x10
    CharWidths mDefaultWidths; // at 0x14
    u16 mAlternateChar;        // at 0x18

    static u16 mFontEncode;
};

} // namespace ut
} // namespace nw4r

#endif
