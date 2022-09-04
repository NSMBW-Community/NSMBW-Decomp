#pragma once
#include <types.h>

typedef void (*ctorPtr)(void *, int);
typedef void (*dtorPtr)(void *, int);

class __partial_array_destructor {
public:
    void *mpArray;
    u32 mSize;
    u32 mCount;
    dtorPtr mpDtor;
    u32 mCurrNum;

    __partial_array_destructor::__partial_array_destructor(void *pArray, u32 elSize, u32 elCount, dtorPtr pDtor) {
        mpArray = pArray;
        mSize = elSize;
        mCount = elCount;
        mpDtor = pDtor;
        mCurrNum = mCount;
    }

    __partial_array_destructor::~__partial_array_destructor() {
        if (mCurrNum < mCount && mpDtor) {
            for (char *ptr = (char *)mpArray + mSize * mCurrNum; mCurrNum > 0; mCurrNum--) {
                ptr -= mSize;
                mpDtor(ptr, -1);
            }
        }
    }
};
