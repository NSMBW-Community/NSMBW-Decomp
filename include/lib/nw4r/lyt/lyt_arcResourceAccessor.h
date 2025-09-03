#ifndef NW4R_LYT_ARC_RESOURCE_ACCESSOR_H
#define NW4R_LYT_ARC_RESOURCE_ACCESSOR_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_resourceAccessor.h>

#include <nw4r/ut.h>

#include <revolution/ARC.h>

namespace nw4r {
namespace lyt {

/******************************************************************************
 *
 * FontRefLink
 *
 ******************************************************************************/
class FontRefLink {
public:
    const char* GetFontName() const {
        return mFontName;
    }

    ut::Font* GetFont() const {
        return mpFont;
    }

public:
    NW4R_UT_LINKLIST_NODE_DECL(); // at 0x0

protected:
    static const int FONTNAMEBUF_MAX = 128;

protected:
    char mFontName[FONTNAMEBUF_MAX]; // at 0x8
    ut::Font* mpFont;                // at 0x88
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(FontRefLink);

/******************************************************************************
 *
 * ArcResourceAccessor
 *
 ******************************************************************************/
class ArcResourceAccessor : public ResourceAccessor {
public:
    static const ulong RES_TYPE_ANIMATION = 'anim';
    static const ulong RES_TYPE_LAYOUT = 'blyt';
    static const ulong RES_TYPE_FONT = 'font';
    static const ulong RES_TYPE_TEXTURE = 'timg';

public:
    ArcResourceAccessor();

    virtual void* GetResource(ulong type, const char* pName,
                              size_t* pSize); // at 0xC

    virtual ut::Font* GetFont(const char* pName); // at 0x10

    bool Attach(void* pArchive, const char* pRootDir);
    void *Detach();

    bool IsAttached() const {
        return mArcBuf != NULL;
    }

protected:
    static const int ROOTPATH_MAX = 128;

protected:
    ARCHandle mArcHandle;           // at 0x4
    void* mArcBuf;                  // at 0x20
    FontRefLinkList mFontList;      // at 0x24
    char mResRootDir[ROOTPATH_MAX]; // at 0x30
};

} // namespace lyt
} // namespace nw4r

#endif
