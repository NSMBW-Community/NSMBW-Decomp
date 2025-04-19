#pragma once

namespace EGG {
    class Heap;

    class Disposer {
    public:
        Disposer();

        virtual ~Disposer();

        Heap *mpContainingHeap;
    };
}
