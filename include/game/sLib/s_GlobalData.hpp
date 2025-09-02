#pragma once

/**
 * @brief A templated global data holder class.
 * @tparam T The class that contains the global data struct.
 * @unofficial
 */
template <typename T>
class sGlobalData_c {
public:
    static typename T::GlobalData_t mData; ///< @unofficial
};
