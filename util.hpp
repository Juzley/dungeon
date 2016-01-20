#ifndef __UTIL_HPP__
#define __UTIL_HPP__

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
}

#endif /* __UTIL_HPP */
