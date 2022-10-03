#include <types.h>
#include <runtime/global_destructor_chain.h>
/// @file

/// @brief Global object chain.
objectRef *__global_destructor_chain;

/**
 * @brief Registers a global object for later destruction.
 *
 * @param pObj Pointer to global object.
 * @param pDtor Pointer to destructor function.
 * @param pLastRef Pointer to the previous objectRef.
 * @return Pointer to global object (pass through).
 */
void *__register_global_object(void *pObj, dtorPtr pDtor, objectRef *pLastRef) {
    pLastRef->mpLastObject = __global_destructor_chain;
    pLastRef->mpDtor = pDtor;
    pLastRef->mpObject = pObj;
    __global_destructor_chain = pLastRef;
    return pObj;
}


/// @brief Destroys all constructed global objects.
void __destroy_global_chain() {
    objectRef *pRef;
    while ((pRef = __global_destructor_chain) != nullptr) {
        __global_destructor_chain = __global_destructor_chain->mpLastObject;
        void *pObj = pRef->mpObject;
        pRef->mpDtor(pObj, -1);
    }
}

#pragma section ".dtors$10"
__declspec(section ".dtors$10") __declspec(weak)
    extern void * const __destroy_global_chain_reference = __destroy_global_chain;
