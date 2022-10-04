#pragma once

typedef void (*dtorPtr)(void *, int);

/// @brief Global destruction chain entry.
/// @details If mpObject() is @p nullptr , this entry is a marker.
typedef struct objectRef {
    struct objectRef *mpLastObject; ///< A pointer to the previously generated objectRef.
    dtorPtr mpDtor; ///< A pointer to the object's default destructor.
    void *mpObject; ///< A pointer to the object.
} objectRef;
