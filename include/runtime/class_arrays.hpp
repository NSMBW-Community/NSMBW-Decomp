#pragma once
#include <types.h>
/// @file

typedef void (*ctorPtr)(void *, int); ///< Constructor function pointer.
typedef void (*dtorPtr)(void *, int); ///< Destructor function pointer.

/// @brief Guarantees the correct destruction of an array if an exception is thrown during its construction.
/// @details [Since the game doesn't support exceptions, this is unused].
class __partial_array_destructor {
public:
    /**
     * @brief Creates a partial array destructor.
     *
     * @param pArray A pointer to the array memory.
     * @param elSize The size of one array element.
     * @param elCount The total number of array elements.
     * @param pDtor A pointer to the elements' default destructor, or @p nullptr .
     */
    __partial_array_destructor::__partial_array_destructor(void *pArray, u32 elSize, u32 elCount, dtorPtr pDtor) :
        mpArray(pArray),
        mSize(elSize),
        mCount(elCount),
        mpDtor(pDtor) {
        mCurrNum = mCount;
    }

    /// @brief Destroys each array element in reverse order.
    /// @details The function only acts if the array isn't fully constructed.
    __partial_array_destructor::~__partial_array_destructor() {
        if (mCurrNum < mCount && mpDtor) {
            for (char *ptr = (char *)mpArray + mSize * mCurrNum; mCurrNum > 0; mCurrNum--) {
                ptr -= mSize;
                mpDtor(ptr, -1);
            }
        }
    }

    void *mpArray; ///< A pointer to the array memory.
    u32 mSize; ///< The size of one array element.
    u32 mCount; ///< The total number of array elements.
    dtorPtr mpDtor; ///< A pointer to the elements' default destructor.
    u32 mCurrNum; ///< The number of currently constructed array elements.
};
