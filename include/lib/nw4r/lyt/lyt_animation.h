#ifndef NW4R_LYT_ANIMATION_H
#define NW4R_LYT_ANIMATION_H
#include <nw4r/types_nw4r.h>

#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_resources.h>
#include <nw4r/lyt/lyt_group.h>
#include <nw4r/lyt/lyt_types.h>

#include <nw4r/ut.h>

namespace nw4r {
namespace lyt {

// Forward declarations
class Pane;
class Material;
class ResourceAccessor;

namespace res {

/******************************************************************************
 *
 * AnimationBlock
 *
 ******************************************************************************/
struct AnimationBlock {
    DataBlockHeader blockHeader; // at 0x0
    u16 frameSize;               // at 0x8
    u8 loop;                     // at 0xA
    u8 PADDING_0xB;              // at 0xB
    u16 fileNum;                 // at 0xC
    u16 animContNum;             // at 0xE
    ulong animContOffsetsOffset;   // at 0x10
};

struct AnimationTagBlock {
    DataBlockHeader blockHeader; // at 0x00
    u16 tagOrder;                // at 0x08
    u16 groupNum;                // at 0x0A
    ulong nameOffset;              // at 0x0C
    ulong groupsOffset;            // at 0x10
    u16 startFrame;              // at 0x14
    u16 endFrame;                // at 0x16
    u8 flag;                     // at 0x18
    u8 padding[3];               // at 0x19
};

struct AnimationShareBlock {
    DataBlockHeader blockHeader; // at 0x00
    ulong animShareInfoOffset;     // at 0x04
    u16 shareNum;                // at 0x0C
    u8 padding[2];               // at 0x0E
};

/******************************************************************************
 *
 * AnimationInfo
 *
 ******************************************************************************/
struct AnimationInfo {
    ulong kind;                  // at 0x0
    u8 num;                    // at 0x4
    u8 PADDING_0x5[0x8 - 0x5]; // at 0x5

    static const ulong SIGNATURE_ANMPANESRT = 'RLPA';
    static const ulong SIGNATURE_ANMPANEVIS = 'RLVI';
    static const ulong SIGNATURE_ANMVTXCLR = 'RLVC';

    static const ulong SIGNATURE_ANMMATCLR = 'RLMC';
    static const ulong SIGNATURE_ANMTEXSRT = 'RLTS';
    static const ulong SIGNATURE_ANMTEXPAT = 'RLTP';
    static const ulong SIGNATURE_ANMINDTEXSRT = 'RLIM';
};

} // namespace res

/******************************************************************************
 *
 * AnimTransform
 *
 ******************************************************************************/
class AnimTransform {
public:
    AnimTransform();
    virtual ~AnimTransform(); // at 0x8

    virtual void SetResource(const res::AnimationBlock* pBlock,
                             ResourceAccessor* pAccessor) = 0; // at 0xC
    virtual void SetResource(
        const res::AnimationBlock *pRes, ResourceAccessor *pResAccessor,
        u16 animNum
    ) = 0;                                              // at 0x10

    virtual void Bind(Pane* pPane, bool recursive) = 0; // at 0x14
    virtual void Bind(Material* pMaterial) = 0;         // at 0x18

    virtual void Animate(ulong idx, Pane* pPane) = 0;         // at 0x1C
    virtual void Animate(ulong idx, Material* pMaterial) = 0; // at 0x20

    u16 GetFrameSize() const;

    bool IsLoopData() const;

    f32 GetFrame() const {
        return mFrame;
    }
    void SetFrame(f32 frame) {
        mFrame = frame;
    }

    f32 GetFrameMax() const {
        return static_cast<f32>(GetFrameSize());
    }

public:
    NW4R_UT_LINKLIST_NODE_DECL(); // at 0x4

protected:
    const res::AnimationBlock* mpRes; // at 0xC
    f32 mFrame;                       // at 0x10
};

NW4R_UT_LINKLIST_TYPEDEF_DECL(AnimTransform);

class AnimResource {
public:
    AnimResource();
    AnimResource(const void *anmResBuf) {
        Set(anmResBuf);
    }
    void Set(const void *anmResBuf);
    void Init();
    u16 GetGroupNum() const;
    const AnimationGroupRef *GetGroupArray() const;
    bool IsDescendingBind() const;
    u16 GetAnimationShareInfoNum() const;
    const AnimationShareInfo *GetAnimationShareInfoArray() const;
    u16 CalcAnimationNum(Pane *pPane, bool bRecursive) const;
    u16 CalcAnimationNum(Group *pGroup, bool bRecursive) const;
    u16 CalcAnimationNum(res::Group *pGroup, bool bRecursive) const;

    const res::AnimationBlock *GetResourceBlock() const {
        return mpResBlock;
    }
    const res::AnimationTagBlock *GetTagBlock() const {
        return mpTagBlock;
    }
    const res::AnimationShareBlock *GetShareBlock() const {
        return mpShareBlock;
    }

private:
    const res::BinaryFileHeader *mpFileHeader;    // at 0x00
    const res::AnimationBlock *mpResBlock;        // at 0x04
    const res::AnimationTagBlock *mpTagBlock;     // at 0x08
    const res::AnimationShareBlock *mpShareBlock; // at 0x0C
};

/******************************************************************************
 *
 * AnimTransformBasic
 *
 ******************************************************************************/
class AnimTransformBasic : public AnimTransform {
public:
    AnimTransformBasic();
    virtual ~AnimTransformBasic(); // at 0x8

    virtual void SetResource(const res::AnimationBlock* pBlock,
                             ResourceAccessor* pAccessor); // at 0xC
    virtual void SetResource(const res::AnimationBlock *pRes, ResourceAccessor *pResAccessor, u16 animNum); // at 0x10

    virtual void Bind(Pane* pPane, bool recursive); // at 0x14
    virtual void Bind(Material* pMaterial);         // at 0x18

    virtual void Animate(ulong idx, Pane* pPane);         // at 0x1C
    virtual void Animate(ulong idx, Material* pMaterial); // at 0x20

protected:
    void** mpFileResAry;         // at 0x14
    AnimationLink* mAnimLinkAry; // at 0x18
    u16 mAnimLinkNum;            // at 0x1C
};

/******************************************************************************
 *
 * Functions
 *
 ******************************************************************************/
namespace detail {

AnimationLink* FindAnimationLink(AnimationLinkList* pAnimList,
                                 AnimTransform* pAnimTrans);

} // namespace detail

/******************************************************************************
 *
 * AnimTargetPane
 *
 ******************************************************************************/
enum AnimTargetPane {
    ANIMTARGET_PANE_TRANSX,
    ANIMTARGET_PANE_TRANSY,
    ANIMTARGET_PANE_TRANSZ,

    ANIMTARGET_PANE_ROTX,
    ANIMTARGET_PANE_ROTY,
    ANIMTARGET_PANE_ROTZ,

    ANIMTARGET_PANE_SCALEX,
    ANIMTARGET_PANE_SCALEY,

    ANIMTARGET_PANE_SIZEX,
    ANIMTARGET_PANE_SIZEY,

    ANIMTARGET_PANE_MAX,

    ANIMTARGET_PANE_COLOR_ALPHA = 16,
    ANIMTARGET_PANE_COLOR_MAX,
};

/******************************************************************************
 *
 * AnimTargetVtxColor
 *
 ******************************************************************************/
enum AnimTargetVtxColor {
    ANIMTARGET_VERTEXCOLOR_LT_RED,
    ANIMTARGET_VERTEXCOLOR_LT_GREEN,
    ANIMTARGET_VERTEXCOLOR_LT_BLUE,
    ANIMTARGET_VERTEXCOLOR_LT_ALPHA,

    ANIMTARGET_VERTEXCOLOR_RT_RED,
    ANIMTARGET_VERTEXCOLOR_RT_GREEN,
    ANIMTARGET_VERTEXCOLOR_RT_BLUE,
    ANIMTARGET_VERTEXCOLOR_RT_ALPHA,

    ANIMTARGET_VERTEXCOLOR_LB_RED,
    ANIMTARGET_VERTEXCOLOR_LB_GREEN,
    ANIMTARGET_VERTEXCOLOR_LB_BLUE,
    ANIMTARGET_VERTEXCOLOR_LB_ALPHA,

    ANIMTARGET_VERTEXCOLOR_RB_RED,
    ANIMTARGET_VERTEXCOLOR_RB_GREEN,
    ANIMTARGET_VERTEXCOLOR_RB_BLUE,
    ANIMTARGET_VERTEXCOLOR_RB_ALPHA,

    ANIMTARGET_VERTEXCOLOR_MAX
};

/******************************************************************************
 *
 * AnimTargetMatColor
 *
 ******************************************************************************/
enum AnimTargetMatColor {
    ANIMTARGET_MATCOLOR_MATR,
    ANIMTARGET_MATCOLOR_MATG,
    ANIMTARGET_MATCOLOR_MATB,
    ANIMTARGET_MATCOLOR_MATA,

    ANIMTARGET_MATCOLOR_TEV0R,
    ANIMTARGET_MATCOLOR_TEV0G,
    ANIMTARGET_MATCOLOR_TEV0B,
    ANIMTARGET_MATCOLOR_TEV0A,

    ANIMTARGET_MATCOLOR_TEV1R,
    ANIMTARGET_MATCOLOR_TEV1G,
    ANIMTARGET_MATCOLOR_TEV1B,
    ANIMTARGET_MATCOLOR_TEV1A,

    ANIMTARGET_MATCOLOR_TEV2R,
    ANIMTARGET_MATCOLOR_TEV2G,
    ANIMTARGET_MATCOLOR_TEV2B,
    ANIMTARGET_MATCOLOR_TEV2A,

    ANIMTARGET_MATCOLOR_TEVK0R,
    ANIMTARGET_MATCOLOR_TEVK0G,
    ANIMTARGET_MATCOLOR_TEVK0B,
    ANIMTARGET_MATCOLOR_TEVK0A,

    ANIMTARGET_MATCOLOR_TEVK1R,
    ANIMTARGET_MATCOLOR_TEVK1G,
    ANIMTARGET_MATCOLOR_TEVK1B,
    ANIMTARGET_MATCOLOR_TEVK1A,

    ANIMTARGET_MATCOLOR_TEVK2R,
    ANIMTARGET_MATCOLOR_TEVK2G,
    ANIMTARGET_MATCOLOR_TEVK2B,
    ANIMTARGET_MATCOLOR_TEVK2A,

    ANIMTARGET_MATCOLOR_TEVK3R,
    ANIMTARGET_MATCOLOR_TEVK3G,
    ANIMTARGET_MATCOLOR_TEVK3B,
    ANIMTARGET_MATCOLOR_TEVK3A,

    ANIMTARGET_MATCOLOR_MAX
};

/******************************************************************************
 *
 * AnimTargetTexSRT
 *
 ******************************************************************************/
enum AnimTargetTexSRT {
    ANIMTARGET_TEXSRT_TRANSX,
    ANIMTARGET_TEXSRT_TRANSY,

    ANIMTARGET_TEXSRT_ROT,

    ANIMTARGET_TEXSRT_SCALEX,
    ANIMTARGET_TEXSRT_SCALEY,

    ANIMTARGET_TEXSRT_MAX
};

/******************************************************************************
 *
 * AnimTargetTexPat
 *
 ******************************************************************************/
enum AnimTargetTexPat {
    ANIMTARGET_TEXPATTURN_IMAGE,

    ANIMTARGET_TEXPATTURN_MAX
};

void BindAnimation(Group *pGroup, AnimTransform *pAnimTrans, bool bRecursive, bool bDisable);

} // namespace lyt
} // namespace nw4r

#endif
