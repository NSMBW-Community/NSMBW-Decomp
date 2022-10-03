#pragma once
#include <types.h>

typedef void (*ctorPtr)(void *, int);
typedef void (*dtorPtr)(void *, int);

/**
 * @brief This class is used to guarantee correct destruction of partially
 * constructed arrays if an exception is thrown from an array-element constructor.
 */
class __partial_array_destructor {
public:
    void *mpArray; ///< Pointer to array memory.
    u32 mSize; ///< Size of one array element.
    u32 mCount; ///< Total number of array elements.
    dtorPtr mpDtor; ///< Pointer to destructor function (or null).
    u32 mCurrNum; ///< Number of currently constructed array elements.

    /**
     * @brief Constructs a partial array destructor.
     *
     * @param pArray Pointer to array memory.
     * @param elSize Size of one array element.
     * @param elCount Total number of array elements.
     * @param pDtor Pointer to destructor function (or null).
     */
    __partial_array_destructor::__partial_array_destructor(void *pArray, u32 elSize, u32 elCount, dtorPtr pDtor) {
        mpArray = pArray;
        mSize = elSize;
        mCount = elCount;
        mpDtor = pDtor;
        mCurrNum = mCount;
    }

    /// @brief Calls mpDtor() for each array element in reverse order if mCurrNum() is less than mCount().
    __partial_array_destructor::~__partial_array_destructor() {
        if (mCurrNum < mCount && mpDtor) {
            for (char *ptr = (char *)mpArray + mSize * mCurrNum; mCurrNum > 0; mCurrNum--) {
                ptr -= mSize;
                mpDtor(ptr, -1);
            }
        }
    }
};
