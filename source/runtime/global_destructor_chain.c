#include <types.h>
#include <runtime/global_destructor_chain.h>

objectRef *__global_destructor_chain;

void __register_global_object(void *pObj, dtorPtr pDtor, objectRef *pLastRef) {
    pLastRef->mpLastObject = __global_destructor_chain;
    pLastRef->mpDtor = pDtor;
    pLastRef->mpObject = pObj;
    __global_destructor_chain = pLastRef;
    return;
}

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
