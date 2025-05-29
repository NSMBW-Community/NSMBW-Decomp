// https://raw.githubusercontent.com/doldecomp/ogws/d5505bee60a51db06eab3800421bdb1237c45e92/include/nw4r/lyt/lyt_types.h

#pragma once

namespace nw4r {
namespace lyt {
namespace detail {

/******************************************************************************
 *
 * Bit operations
 *
 ******************************************************************************/
template <typename T> inline void SetBit(T* pBits, int pos, bool value) {
    T mask = ~(1 << pos);
    *pBits &= mask;
    *pBits |= (value ? 1 : 0) << pos;
}
template <typename T> inline bool TestBit(T bits, int pos) {
    T mask = 1 << pos;
    return (bits & mask) != 0;
}
template <typename T> inline T GetBits(T bits, int pos, int len) {
    T mask = ~(static_cast<T>(-1) << len);
    return bits >> pos & mask;
}

} // namespace detail
} // namespace lyt
} // namespace nw4r
