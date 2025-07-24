#pragma once

namespace EGG {
    class Heap;

    class Disposer {
    public:
        Disposer();

        virtual ~Disposer();

        Heap *mpContainingHeap;
        // [TODO: nw4r::ut::Link]
    };
}
