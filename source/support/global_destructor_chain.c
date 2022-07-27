typedef void (*dtorPtr)(void *, int);

typedef struct objectRef {
    struct objectRef *lastObject;
    dtorPtr dtor;
    void *object;
} objectRef;

objectRef *__global_destructor_chain;

void __register_global_object(void *obj, dtorPtr dtor, objectRef *lastRef) {
    lastRef->lastObject = __global_destructor_chain;
    lastRef->dtor = dtor;
    lastRef->object = obj;
    __global_destructor_chain = lastRef;
    return;
}

void __destroy_global_chain() {
    objectRef *ref;
    void *obj;
    while ((ref = __global_destructor_chain) != (objectRef *)0x0) {
        __global_destructor_chain = __global_destructor_chain->lastObject;
        obj = ref->object;
        ref->dtor(obj, -1);
    }
}
