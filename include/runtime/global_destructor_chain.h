#pragma once

typedef void (*dtorPtr)(void *, int);

/// @brief Global destruction chain entry.
typedef struct objectRef {
    struct objectRef *mpLastObject; ///< Pointer to the previously generated global object.
    dtorPtr mpDtor; ///< The object's destructor function.
    void *mpObject; ///< The object's memory location.
} objectRef;
