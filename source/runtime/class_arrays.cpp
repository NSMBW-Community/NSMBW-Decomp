#include <types.h>
#include <runtime/class_arrays.hpp>
/// @file

extern "C" {

/**
 * @brief Allocates and constructs an array of objects.
 *
 * @param pBlock Pointer to allocated memory (+8 bytes).
 * @param pCtor Pointer to default constructor function	(or null).
 * @param pDtor Pointer to default destructor function (or null).
 * @param size Size of one array element.
 * @param n Number of array elements.
 * @return Pointer to first object in array.
 */
void *__construct_new_array(void *pBlock, ctorPtr pCtor, dtorPtr pDtor, u32 size, u32 n) {
    char *ptr = (char *)pBlock;
    if (ptr != nullptr) {
        u32 *p = (u32 *)ptr;
        p[0] = size;
        p[1] = n;
        ptr += 16;

        if (pCtor != nullptr) {
            __partial_array_destructor pad(ptr, size, n, pDtor);

            char *p = (char *)ptr;
            for (pad.mCurrNum = 0; pad.mCurrNum < n; p += size) {
                pCtor(p, 1);
                pad.mCurrNum++;
            }
        }
    }

    return ptr;
}

/**
 * @brief Constructs an array of objects.
 *
 * @param pBlock Pointer to array memory.
 * @param pCtor Pointer to default constructor function.
 * @param pDtor Pointer to default destructor function (or null).
 * @param size Size of one array element.
 * @param n Number of array elements.
 */
void __construct_array(void *pBlock, ctorPtr pCtor, dtorPtr pDtor, u32 size, u32 n) {
    __partial_array_destructor pad(pBlock, size, n, pDtor);

    char *p = (char *)pBlock;
    for (pad.mCurrNum = 0; pad.mCurrNum < n; p += size) {
        pCtor(p, 1);
        pad.mCurrNum++;
    }
}

/**
 * @brief Destroys an array of objects.
 *
 * @param pBlock Pointer to array memory.
 * @param pDtor Pointer to default destructor function.
 * @param size Size of one array element.
 * @param n Number of array elements.
 */
void __destroy_arr(void *pBlock, dtorPtr pDtor, u32 size, u32 n) {
    for (char *p = (char *)pBlock + size * n; n > 0; n--) {
        p -= size;
        pDtor(p, -1);
    }
}

/**
 * @brief Destroys and deletes an array of objects.
 *
 * @param pBlock Pointer to allocated array memory (or null).
 * @param pDtor Pointer to default destructor function (or null).
 */
void __destroy_new_array(void *pBlock, dtorPtr pDtor) {
    if (pBlock != nullptr) {
        if (pDtor != nullptr) {
            u32 i, objects, objectsize;
            objectsize = ((u32 *) ((char *) pBlock - 16))[0];
            objects = ((u32 *) ((char *) pBlock - 16))[1];

            char *p = (char *) pBlock + objectsize * objects;
            for (i = 0; i < objects; i++) {
                p -= objectsize;
                pDtor(p, -1);
            }
        }

        delete[] ((char *)pBlock - 16);
    }
}

} // extern "C"
