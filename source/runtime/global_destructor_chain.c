#include <types.h>
#include <runtime/global_destructor_chain.h>
/// @file

/// @brief The global object chain.
objectRef *__global_destructor_chain;

/**
 * @brief Registers a global object for later destruction.
 *
 * @param pObj A pointer to the object.
 * @param pDtor A pointer to the object's default destructor.
 * @param pLastRef A pointer to the previous objectRef.
 * @return A pointer to the object (pass through).
 */
void *__register_global_object(void *pObj, dtorPtr pDtor, objectRef *pLastRef) {
    pLastRef->mpLastObject = __global_destructor_chain;
    pLastRef->mpDtor = pDtor;
    pLastRef->mpObject = pObj;
    __global_destructor_chain = pLastRef;
    return pObj;
}


/// @brief Destroys all constructed global objects.
/// @details [Since the game's main loop never exits, this is unused].
void __destroy_global_chain() {
    objectRef *pRef;
    while ((pRef = __global_destructor_chain) != nullptr) {
        __global_destructor_chain = __global_destructor_chain->mpLastObject;
        void *pObj = pRef->mpObject;
        pRef->mpDtor(pObj, -1);
    }
}

#pragma section ".dtors$10"
DECL_SECTION(".dtors$10")
DECL_WEAK
extern void * const __destroy_global_chain_reference = __destroy_global_chain;
