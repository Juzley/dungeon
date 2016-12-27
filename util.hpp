#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <cmath>

namespace util {
    /*
     * hash_combine
     *
     * Combine multiple std::hash values.
     */
    template <class T>
    static inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    static inline float symmetric_ceil(float value)
    {
        float res = std::ceil(std::fabs(value));
        return value > 0 ? res : -res;
    }
}

#endif /* __UTIL_HPP */
