typedef unsigned int uint;
typedef void (*ctorPtr)(void *, int);
typedef void (*dtorPtr)(void *, int);

class __partial_array_destructor {
public:
    void *arrP;
    uint size;
    uint n;
    dtorPtr dtor;
    uint i;

    __partial_array_destructor(void *array, uint elSize, uint elCount, dtorPtr dtor) {
        arrP = array;
        size = elSize;
        n = elCount;
        this->dtor = dtor;
        i = n;
    }

    ~__partial_array_destructor() {
        if(i < n && dtor) {
            for(char *ptr = (char *)arrP + size * i; i > 0; i--) {
                ptr -= size;
                dtor(ptr, -1);
            }
        }
    }
};

extern "C" {

void *__construct_new_array(void *block, ctorPtr ctor, dtorPtr dtor, uint size, uint n) {
    char *ptr = (char *) block;
    if (ptr != 0){
        uint *p = (uint *)ptr;
        p[0] = size;
        p[1] = n;
        ptr += 16;

        if (ctor) {
            __partial_array_destructor pad(ptr, size, n, dtor);

            char *p = (char *)ptr;
            for (pad.i = 0; pad.i < n; p += size) {
                ctor(p, 1);
                pad.i++;
            }
        }
    }
    return ptr;
}

void __construct_array(void *ptr, ctorPtr ctor, dtorPtr dtor, uint size, uint n) {
    __partial_array_destructor pad(ptr, size, n, dtor);

    char *p = (char *)ptr;
    for (pad.i = 0; pad.i < n; p += size) {
        ctor(p, 1);
        pad.i++;
    }
}

void __destroy_arr(void *block, dtorPtr dtor, uint size, uint n) {
    for (char *p = (char *)block + size * n; n > 0; n--){
        p -= size;
        dtor(p, -1);
    }
}

void __destroy_new_array(void *block, dtorPtr dtor) {
    if (block) {
        if (dtor) {
            uint i, objects, objectsize;

            objectsize = *(uint *) ((char *) block - 16);
            objects = ((uint *) ((char *) block - 16))[1];
            char *p = (char *) block + objectsize * objects;
            for(i = 0; i < objects; i++){
                p -= objectsize;
                dtor(p, -1);
            }
        }
        ::operator delete[] ((char *)block - 16);
    }
}

}
