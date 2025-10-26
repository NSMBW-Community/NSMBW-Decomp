#pragma once

class dGraph_c {
public:
    void (*mpPainterFunc)();

    static dGraph_c *ms_Instance;
};
