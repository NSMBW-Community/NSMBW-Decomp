#pragma once
#include <game/bases/d_enemy.hpp>
#include <game/bases/d_heap_allocator.hpp>

/**
 * @brief A decorative leafy bush used for Yoshi fruits.
 * @ingroup bases
 * @details The actor can be placed in layer 2.
 * @paramtable
 * @bug Bushes with sizes larger than #SIZE_SMALL spawn too late when approached from above due
 * to the bounding box not being updated.
 */
class daObjFruitTree_c : public dEn_c {
public:

    /// @brief The possible bush styles.
    enum STYLE_e {
        STYLE_REGULAR, ///< The regular bush style.
        STYLE_MOUNTAIN, ///< The mountain bush style, with a yellow-ish color.
    };

    /// @brief The possible bush sizes.
    enum SIZE_e {
        SIZE_SMALL,
        SIZE_MEDIUM,
        SIZE_LARGE,
        SIZE_EXTRA_LARGE,
    };

    daObjFruitTree_c() {} ///< Creates a new daObjFruitTree_c.

    virtual int create();
    virtual int doDelete();
    virtual int execute();
    virtual int draw();
    virtual ~daObjFruitTree_c() {} ///< Deletes the daObjFruitTree_c.

    void createModel(nw4r::g3d::ResMdl resMdl) {
        mBushModel.create(resMdl, &mHeapAllocator, 0x20, 1, nullptr);
    }
    void createAnim2(nw4r::g3d::ResMdl resMdl, nw4r::g3d::ResAnmChr animChr) {
        mBushAnim.create(resMdl, animChr, &mHeapAllocator, nullptr);
    }
    void createAnim(nw4r::g3d::ResMdl resMdl,nw4r::g3d::ResAnmChr animChr) {
        createAnim2(resMdl, animChr);
    }

    dHeapAllocator_c mHeapAllocator; ///< The allocator.
    nw4r::g3d::ResFile mResFile; ///< The resource file.
    m3d::mdl_c mBushModel; ///< The bush model.
    m3d::anmChr_c mBushAnim; ///< The bush model animation.

    ACTOR_PARAM_CONFIG(Size, 0, 4); ///< The size of the bush. @see SIZE_e
    ACTOR_PARAM_CONFIG(Style, 4, 4); ///< The style of the bush. @see STYLE_e
};
