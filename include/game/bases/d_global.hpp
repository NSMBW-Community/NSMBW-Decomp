#pragma once

template <typename T>
class dGlobalData_c {
public:
    static typename T::GlobalData_s data;
};
