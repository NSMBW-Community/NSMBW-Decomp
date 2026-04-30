#pragma once

/**
 * @brief A templated global data holder class.
 * @tparam T The class that contains the global data struct.
 * @unofficial
 */
template <typename T>
class sGlobalData_c {
public:
    static const typename T::GlobalData_t mData; ///< @unofficial
};

// Some beautiful template magic to get quick access to the global data of a class.

template <class T>
struct remove_reference { typedef T type; };
template <class T>
struct remove_reference<T&> { typedef T type; };

#define GLOBAL_DATA sGlobalData_c<remove_reference<decltype(*this)>::type>::mData
