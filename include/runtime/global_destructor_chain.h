#pragma once

typedef void (*dtorPtr)(void *, int);

typedef struct objectRef {
    struct objectRef *mpLastObject;
    dtorPtr mpDtor;
    void *mpObject;
} objectRef;
